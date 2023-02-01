#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"

#include <fstream>
#include <numeric>
#include <iostream>
#include <string_view>
#include <unordered_map>
#include <unordered_set>

#include <fmt/format.h>
#include <fmt/ostream.h>

#include <openssl/sha.h>

#include "interface.h"

struct FunctionParams {
    std::vector<clang::QualType> param_types;
};

struct ThunkedCallback : FunctionParams {
    clang::QualType return_type;

    bool is_stub = false;  // Callback will be replaced by a stub that calls std::abort
    bool is_guest = false; // Callback will never be called on the host
    bool is_variadic = false;
};

/**
 * Guest<->Host transition point.
 *
 * These are normally used to translate the public API of the guest to host
 * function calls (ThunkedAPIFunction), but a thunk library may also define
 * internal thunks that don't correspond to any function in the implemented
 * API.
 */
struct ThunkedFunction : FunctionParams {
    std::string function_name;
    clang::QualType return_type;

    // If true, param_types contains an extra size_t and the valist for marshalling through an internal function
    bool is_variadic = false;

    // If true, the unpacking function will call a custom fexfn_impl function
    // to be provided manually instead of calling the host library function
    // directly.
    // This is implied e.g. for thunks generated for variadic functions
    bool custom_host_impl = false;

    std::string GetOriginalFunctionName() const {
        const std::string suffix = "_internal";
        assert(function_name.length() > suffix.size());
        assert((std::string_view { &*function_name.end() - suffix.size(), suffix.size() } == suffix));
        return function_name.substr(0, function_name.size() - suffix.size());
    }

    // Maps parameter index to ThunkedCallback
    std::unordered_map<unsigned, ThunkedCallback> callbacks;

    clang::FunctionDecl* decl;
};

/**
 * Function that is part of the API of the thunked library.
 *
 * For each of these, there is:
 * - A publicly visible guest entrypoint (usually auto-generated but may be manually defined)
 * - A pointer to the native host library function loaded through dlsym (or a user-provided function specified via host_loader)
 * - A ThunkedFunction with the same function_name (possibly suffixed with _internal)
 */
struct ThunkedAPIFunction : FunctionParams {
    std::string function_name;

    clang::QualType return_type;

    // name of the function to load the native host symbol with
    std::string host_loader;

    // If true, no guest-side implementation of this function will be autogenerated
    bool custom_guest_impl;

    bool is_variadic;

    // Index of the symbol table to store this export in (see guest_symtables).
    // If empty, a library export is created, otherwise the function is entered into a function pointer array
    std::optional<std::size_t> symtable_namespace;
};

struct NamespaceInfo {
    clang::DeclContext* context;

    std::string name;

    // Function to load native host library functions with.
    // This function must be defined manually with the signature "void* func(void*, const char*)"
    std::string host_loader;

    bool generate_guest_symtable;

    bool indirect_guest_calls;
};

static std::vector<clang::DeclContext*> decl_contexts;

struct ClangDiagnosticAsException {
    std::pair<clang::SourceLocation, unsigned> diagnostic;

    void Report(clang::DiagnosticsEngine& diagnostics) const {
        diagnostics.Report(diagnostic.first, diagnostic.second);
    }
};

// Helper class to build a custom DiagID from the given message and store it in a throwable object
struct ErrorReporter {
    clang::ASTContext& context;

    template<std::size_t N>
    [[nodiscard]] ClangDiagnosticAsException operator()(clang::SourceLocation loc, const char (&message)[N]) {
        auto id = context.getDiagnostics().getCustomDiagID(clang::DiagnosticsEngine::Error, message);
        return { std::pair(loc, id) };
    }
};

struct NamespaceAnnotations {
    std::optional<unsigned> version;
    std::optional<std::string> load_host_endpoint_via;
    bool generate_guest_symtable = false;
    bool indirect_guest_calls = false;
};

static NamespaceAnnotations GetNamespaceAnnotations(clang::ASTContext& context, clang::CXXRecordDecl* decl) {
    if (!decl->hasDefinition()) {
        return {};
    }

    ErrorReporter report_error { context };
    NamespaceAnnotations ret;

    for (const clang::CXXBaseSpecifier& base : decl->bases()) {
        auto annotation = base.getType().getAsString();
        if (annotation == "fexgen::generate_guest_symtable") {
            ret.generate_guest_symtable = true;
        } else if (annotation == "fexgen::indirect_guest_calls") {
            ret.indirect_guest_calls = true;
        } else {
            throw report_error(base.getSourceRange().getBegin(), "Unknown namespace annotation");
        }
    }

    for (const clang::FieldDecl* field : decl->fields()) {
        auto name = field->getNameAsString();
        if (name == "load_host_endpoint_via") {
            auto loader_function_expr = field->getInClassInitializer()->IgnoreCasts();
            auto loader_function_str = llvm::dyn_cast_or_null<clang::StringLiteral>(loader_function_expr);
            if (loader_function_expr && !loader_function_str) {
                throw report_error(loader_function_expr->getBeginLoc(),
                                          "Must initialize load_host_endpoint_via with a string");
            }
            if (loader_function_str) {
                ret.load_host_endpoint_via = loader_function_str->getString();
            }
        } else if (name == "version") {
            auto initializer = field->getInClassInitializer()->IgnoreCasts();
            auto version_literal = llvm::dyn_cast_or_null<clang::IntegerLiteral>(initializer);
            if (!initializer || !version_literal) {
                throw report_error(field->getBeginLoc(), "No version given (expected integral typed member, e.g. \"int version = 5;\")");
            }
            ret.version = version_literal->getValue().getZExtValue();
        } else {
            throw report_error(field->getBeginLoc(), "Unknown namespace annotation");
        }
    }

    return ret;
}

enum class CallbackStrategy {
    Default,
    Stub,
    Guest,
};

struct Annotations {
    bool custom_host_impl = false;
    bool custom_guest_entrypoint = false;

    bool returns_guest_pointer = false;

    std::optional<clang::QualType> uniform_va_type;

    CallbackStrategy callback_strategy = CallbackStrategy::Default;
};

static Annotations GetAnnotations(clang::ASTContext& context, clang::CXXRecordDecl* decl) {
    ErrorReporter report_error { context };
    Annotations ret;

    for (const auto& base : decl->bases()) {
        auto annotation = base.getType().getAsString();
        if (annotation == "fexgen::returns_guest_pointer") {
            ret.returns_guest_pointer = true;
        } else if (annotation == "fexgen::custom_host_impl") {
            ret.custom_host_impl = true;
        } else if (annotation == "fexgen::callback_stub") {
            ret.callback_strategy = CallbackStrategy::Stub;
        } else if (annotation == "fexgen::callback_guest") {
            ret.callback_strategy = CallbackStrategy::Guest;
        } else if (annotation == "fexgen::custom_guest_entrypoint") {
            ret.custom_guest_entrypoint = true;
        } else {
            throw report_error(base.getSourceRange().getBegin(), "Unknown annotation");
        }
    }

    for (const auto& child_decl : decl->getPrimaryContext()->decls()) {
        if (auto field = llvm::dyn_cast_or_null<clang::FieldDecl>(child_decl)) {
            throw report_error(field->getBeginLoc(), "Unknown field annotation");
        } else if (auto type_alias = llvm::dyn_cast_or_null<clang::TypedefNameDecl>(child_decl)) {
            auto name = type_alias->getNameAsString();
            if (name == "uniform_va_type") {
                ret.uniform_va_type = type_alias->getUnderlyingType();
            } else {
                throw report_error(type_alias->getBeginLoc(), "Unknown type alias annotation");
            }
        }
    }

    return ret;
}

class ASTVisitor : public clang::RecursiveASTVisitor<ASTVisitor> {
public:
    /**
     * Matches "template<auto> struct fex_gen_config { ... }"
     */
    bool VisitClassTemplateDecl(clang::ClassTemplateDecl* decl) {
        if (decl->getName() != "fex_gen_config") {
            return true;
        }

        if (llvm::dyn_cast<clang::NamespaceDecl>(decl->getDeclContext())) {
            decl_contexts.push_back(decl->getDeclContext());
        }

        return true;
    }
};

class ASTConsumer : public clang::ASTConsumer {
public:
    void HandleTranslationUnit(clang::ASTContext& context) override {
        ASTVisitor{}.TraverseDecl(context.getTranslationUnitDecl());
    }
};

class GenerateThunkLibsAction : public clang::ASTFrontendAction {
public:
    GenerateThunkLibsAction(const std::string& libname, const OutputFilenames&);

    void ExecuteAction() override;

    std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(clang::CompilerInstance&, clang::StringRef /*file*/) override;

private:
    // Build the internal API representation by processing fex_gen_config and other annotated entities
    void ParseInterface(clang::ASTContext&);

    // Generate helper code for thunk libraries and write them to the output file
    void EmitOutput();

    const std::string& libfilename;
    std::string libname; // sanitized filename, usable as part of emitted function names
    const OutputFilenames& output_filenames;

    std::vector<ThunkedFunction> thunks;
    std::vector<ThunkedAPIFunction> thunked_api;
    std::unordered_set<const clang::Type*> funcptr_types;
    std::optional<unsigned> lib_version;
    std::vector<NamespaceInfo> namespaces;
};

GenerateThunkLibsAction::GenerateThunkLibsAction(const std::string& libname_, const OutputFilenames& output_filenames_)
    : libfilename(libname_), libname(libname_), output_filenames(output_filenames_) {
    for (auto& c : libname) {
        if (c == '-') {
            c = '_';
        }
    }

    decl_contexts.clear();
    decl_contexts.push_back(nullptr); // global namespace (replaced by getTranslationUnitDecl later)
}

template<typename Fn>
static std::string format_function_args(const FunctionParams& params, Fn&& format_arg) {
    std::string ret;
    for (std::size_t idx = 0; idx < params.param_types.size(); ++idx) {
        ret += std::forward<Fn>(format_arg)(idx) + ", ";
    }
    // drop trailing ", "
    ret.resize(ret.size() > 2 ? ret.size() - 2 : 0);
    return ret;
};

static clang::ClassTemplateDecl*
FindClassTemplateDeclByName(clang::DeclContext& decl_context, std::string_view symbol_name) {
    auto& ast_context = decl_context.getParentASTContext();
    auto* ident = &ast_context.Idents.get(symbol_name);
    auto declname = ast_context.DeclarationNames.getIdentifier(ident);
    auto result = decl_context.noload_lookup(declname);
    if (result.empty()) {
        return nullptr;
    } else if (std::next(result.begin()) == result.end()) {
        return llvm::dyn_cast<clang::ClassTemplateDecl>(*result.begin());
    } else {
        throw std::runtime_error("Found multiple matches to symbol " + std::string { symbol_name });
    }
}

void GenerateThunkLibsAction::ExecuteAction() {
    clang::ASTFrontendAction::ExecuteAction();

    // Post-processing happens here rather than in an overridden EndSourceFileAction implementation.
    // We can't move the logic to the latter since this code might still raise errors, but
    // clang's diagnostics engine is already shut down by the time EndSourceFileAction is called.

    auto& context = getCompilerInstance().getASTContext();
    if (context.getDiagnostics().hasErrorOccurred()) {
        return;
    }
    decl_contexts.front() = context.getTranslationUnitDecl();

    try {
        ParseInterface(context);
        EmitOutput();
    } catch (ClangDiagnosticAsException& exception) {
        exception.Report(context.getDiagnostics());
    }
}

void GenerateThunkLibsAction::ParseInterface(clang::ASTContext& context) {
    ErrorReporter report_error { context };

    if (auto template_decl = FindClassTemplateDeclByName(*context.getTranslationUnitDecl(), "fex_gen_type")) {
        for (auto* decl : template_decl->specializations()) {
            const auto& template_args = decl->getTemplateArgs();
            assert(template_args.size() == 1);

            // NOTE: Function types that are equivalent but use differently
            //       named types (e.g. GLuint/GLenum) are represented by
            //       different Type instances. The canonical type they refer
            //       to is unique, however.
            auto type = context.getCanonicalType(template_args[0].getAsType()).getTypePtr();
            funcptr_types.insert(type);
        }
    }

    // Process declarations and specializations of fex_gen_config,
    // i.e. the function descriptions of the thunked API
    for (auto& decl_context : decl_contexts) {
        if (const auto template_decl = FindClassTemplateDeclByName(*decl_context, "fex_gen_config")) {
            // Gather general information about symbols in this namespace
            const auto annotations = GetNamespaceAnnotations(context, template_decl->getTemplatedDecl());

            auto namespace_decl = llvm::dyn_cast<clang::NamespaceDecl>(decl_context);
            namespaces.push_back({  namespace_decl,
                                    namespace_decl ? namespace_decl->getNameAsString() : "",
                                    annotations.load_host_endpoint_via.value_or(""),
                                    annotations.generate_guest_symtable,
                                    annotations.indirect_guest_calls });
            const auto namespace_idx = namespaces.size() - 1;
            const NamespaceInfo& namespace_info = namespaces.back();

            if (annotations.version) {
                if (namespace_decl) {
                    throw report_error(template_decl->getBeginLoc(), "Library version must be defined in the global namespace");
                }
                lib_version = annotations.version;
            }

            // Process specializations of template fex_gen_config
            for (auto* decl : template_decl->specializations()) {
                if (decl->getSpecializationKind() == clang::TSK_ExplicitInstantiationDefinition) {
                    throw report_error(decl->getBeginLoc(), "fex_gen_config may not be partially specialized\n");
                }

                const auto& template_args = decl->getTemplateArgs();
                assert(template_args.size() == 1);

                auto emitted_function = llvm::dyn_cast<clang::FunctionDecl>(template_args[0].getAsDecl());
                assert(emitted_function && "Argument is not a function");
                auto return_type = emitted_function->getReturnType();

                const auto annotations = GetAnnotations(context, decl);
                if (return_type->isFunctionPointerType() && !annotations.returns_guest_pointer) {
                    throw report_error( decl->getBeginLoc(),
                                        "Function pointer return types require explicit annotation\n");
                }

                // TODO: Use the types as written in the signature instead?
                ThunkedFunction data;
                data.function_name = emitted_function->getName().str();
                data.return_type = return_type;
                data.is_variadic = emitted_function->isVariadic();

                data.decl = emitted_function;

                data.custom_host_impl = annotations.custom_host_impl;

                for (std::size_t param_idx = 0; param_idx < emitted_function->param_size(); ++param_idx) {
                    auto* param = emitted_function->getParamDecl(param_idx);
                    data.param_types.push_back(param->getType());

                    if (param->getType()->isFunctionPointerType()) {
                        auto funcptr = param->getFunctionType()->getAs<clang::FunctionProtoType>();
                        ThunkedCallback callback;
                        callback.return_type = funcptr->getReturnType();
                        for (auto& cb_param : funcptr->getParamTypes()) {
                            callback.param_types.push_back(cb_param);
                        }
                        callback.is_stub = annotations.callback_strategy == CallbackStrategy::Stub;
                        callback.is_guest = annotations.callback_strategy == CallbackStrategy::Guest;
                        callback.is_variadic = funcptr->isVariadic();

                        if (callback.is_guest && !data.custom_host_impl) {
                            throw report_error(decl->getBeginLoc(), "callback_guest can only be used with custom_host_impl");
                        }

                        data.callbacks.emplace(param_idx, callback);
                        if (!callback.is_stub && !callback.is_guest) {
                            funcptr_types.insert(context.getCanonicalType(funcptr));
                        }

                        if (data.callbacks.size() != 1) {
                            throw report_error(decl->getBeginLoc(), "Support for more than one callback is untested");
                        }
                        if (funcptr->isVariadic() && !callback.is_stub) {
                            throw report_error(decl->getBeginLoc(), "Variadic callbacks are not supported");
                        }
                    }
                }

                thunked_api.push_back(ThunkedAPIFunction { (const FunctionParams&)data, data.function_name, data.return_type,
                                                            namespace_info.host_loader.empty() ? "dlsym_default" : namespace_info.host_loader,
                                                            data.is_variadic || annotations.custom_guest_entrypoint,
                                                            data.is_variadic,
                                                            std::nullopt });
                if (namespace_info.generate_guest_symtable) {
                    thunked_api.back().symtable_namespace = namespace_idx;
                }

                if (data.is_variadic) {
                    if (!annotations.uniform_va_type) {
                        throw report_error(decl->getBeginLoc(), "Variadic functions must be annotated with parameter type using uniform_va_type");
                    }

                    // Convert variadic argument list into a count + pointer pair
                    data.param_types.push_back(context.getSizeType());
                    data.param_types.push_back(context.getPointerType(*annotations.uniform_va_type));
                }

                if (data.is_variadic) {
                    // This function is thunked through an "_internal" symbol since its signature
                    // is different from the one in the native host/guest libraries.
                    data.function_name = data.function_name + "_internal";
                    if (data.custom_host_impl) {
                        throw report_error(decl->getBeginLoc(), "Custom host impl requested but this is implied by the function signature already");
                    }
                    data.custom_host_impl = true;
                }

                // For indirect calls, register the function signature as a function pointer type
                if (namespace_info.indirect_guest_calls) {
                    funcptr_types.insert(context.getCanonicalType(emitted_function->getFunctionType()));
                }

                thunks.push_back(std::move(data));
            }
        }
    }
}

void GenerateThunkLibsAction::EmitOutput() {
    static auto format_decl = [](clang::QualType type, const std::string_view& name) {
        if (type->isFunctionPointerType()) {
            auto signature = type.getAsString();
            const char needle[] = { '(', '*', ')' };
            auto it = std::search(signature.begin(), signature.end(), std::begin(needle), std::end(needle));
            if (it == signature.end()) {
                // It's *probably* a typedef, so this should be safe after all
                return fmt::format("{} {}", signature, name);
            } else {
                signature.insert(it + 2, name.begin(), name.end());
                return signature;
            }
        } else {
            return type.getAsString() + " " + std::string(name);
        }
    };

    auto format_struct_members = [](const FunctionParams& params, const char* indent) {
        std::string ret;
        for (std::size_t idx = 0; idx < params.param_types.size(); ++idx) {
            ret += indent + format_decl(params.param_types[idx].getUnqualifiedType(), fmt::format("a_{}", idx)) + ";\n";
        }
        return ret;
    };

    auto format_function_params = [](const FunctionParams& params) {
        std::string ret;
        for (std::size_t idx = 0; idx < params.param_types.size(); ++idx) {
            auto& type = params.param_types[idx];
            ret += format_decl(type, fmt::format("a_{}", idx)) + ", ";
        }
        // drop trailing ", "
        ret.resize(ret.size() > 2 ? ret.size() - 2 : 0);
        return ret;
    };

    auto get_sha256 = [this](const std::string& function_name) {
        std::string sha256_message = libname + ":" + function_name;
        std::vector<unsigned char> sha256(SHA256_DIGEST_LENGTH);
        SHA256(reinterpret_cast<const unsigned char*>(sha256_message.data()),
               sha256_message.size(),
               sha256.data());
        return sha256;
    };

    auto get_callback_name = [](std::string_view function_name, unsigned param_index) -> std::string {
        return fmt::format("{}CBFN{}", function_name, param_index);
    };

    // Files used guest-side
    if (!output_filenames.guest.empty()) {
        std::ofstream file(output_filenames.guest);

        // Guest->Host transition points for API functions
        file << "extern \"C\" {\n";
        for (auto& thunk : thunks) {
            const auto& function_name = thunk.function_name;
            auto sha256 = get_sha256(function_name);
            fmt::print( file, "MAKE_THUNK({}, {}, \"{:#02x}\")\n",
                        libname, function_name, fmt::join(sha256, ", "));
        }
        file << "}\n";

        // Guest->Host transition points for invoking runtime host-function pointers based on their signature
        for (auto type_it = funcptr_types.begin(); type_it != funcptr_types.end(); ++type_it) {
            auto* type = *type_it;
            std::string funcptr_signature = clang::QualType { type, 0 }.getAsString();

            auto cb_sha256 = get_sha256("fexcallback_" + funcptr_signature);

            // Thunk used for guest-side calls to host function pointers
            file << "  // " << funcptr_signature << "\n";
            auto funcptr_idx = std::distance(funcptr_types.begin(), type_it);
            fmt::print( file, "  MAKE_CALLBACK_THUNK(callback_{}, {}, \"{:#02x}\");\n",
                        funcptr_idx, funcptr_signature, fmt::join(cb_sha256, ", "));
        }

        // Thunks-internal packing functions
        file << "extern \"C\" {\n";
        for (auto& data : thunks) {
            const auto& function_name = data.function_name;
            bool is_void = data.return_type->isVoidType();
            file << "FEX_PACKFN_LINKAGE auto fexfn_pack_" << function_name << "(";
            for (std::size_t idx = 0; idx < data.param_types.size(); ++idx) {
                auto& type = data.param_types[idx];
                file << (idx == 0 ? "" : ", ") << format_decl(type, fmt::format("a_{}", idx));
            }
            // Using trailing return type as it makes handling function pointer returns much easier
            file << ") -> " << data.return_type.getAsString() << " {\n";
            file << "  struct {\n";
            for (std::size_t idx = 0; idx < data.param_types.size(); ++idx) {
                auto& type = data.param_types[idx];
                file << "    " << format_decl(type.getUnqualifiedType(), fmt::format("a_{}", idx)) << ";\n";
            }
            if (!is_void) {
                file << "    " << format_decl(data.return_type, "rv") << ";\n";
            } else if (data.param_types.size() == 0) {
                // Avoid "empty struct has size 0 in C, size 1 in C++" warning
                file << "    char force_nonempty;\n";
            }
            file << "  } args;\n";

            for (std::size_t idx = 0; idx < data.param_types.size(); ++idx) {
                auto cb = data.callbacks.find(idx);

                file << "  args.a_" << idx << " = ";
                if (cb == data.callbacks.end() || cb->second.is_stub || cb->second.is_guest) {
                    file << "a_" << idx << ";\n";
                } else {
                    // Before passing guest function pointers to the host, wrap them in a host-callable trampoline
                    fmt::print(file, "AllocateHostTrampolineForGuestFunction(a_{});\n", idx);
                }
            }
            file << "  fexthunks_" << libname << "_" << function_name << "(&args);\n";
            if (!is_void) {
                file << "  return args.rv;\n";
            }
            file << "}\n";
        }
        file << "}\n";

        // Publicly exports equivalent to symbols exported from the native guest library
        file << "extern \"C\" {\n";
        for (auto& data : thunked_api) {
            if (data.custom_guest_impl) {
                continue;
            }

            const auto& function_name = data.function_name;

            file << "__attribute__((alias(\"fexfn_pack_" << function_name << "\"))) auto " << function_name << "(";
            for (std::size_t idx = 0; idx < data.param_types.size(); ++idx) {
                auto& type = data.param_types[idx];
                file << (idx == 0 ? "" : ", ") << format_decl(type, "a_" + std::to_string(idx));
            }
            file << ") -> " << data.return_type.getAsString() << ";\n";
        }
        file << "}\n";

        // Symbol enumerators
        for (std::size_t namespace_idx = 0; namespace_idx < namespaces.size(); ++namespace_idx) {
            const auto& ns = namespaces[namespace_idx];
            file << "#define FOREACH_" << ns.name << (ns.name.empty() ? "" : "_") << "SYMBOL(EXPAND) \\\n";
            for (auto& symbol : thunked_api) {
                if (symbol.symtable_namespace.value_or(0) == namespace_idx) {
                    file << "  EXPAND(" << symbol.function_name << ", \"TODO\") \\\n";
                }
            }
            file << "\n";
        }
    }

    // Files used host-side
    if (!output_filenames.host.empty()) {
        std::ofstream file(output_filenames.host);

        // Forward declarations for symbols loaded from the native host library
        for (auto& import : thunked_api) {
            const auto& function_name = import.function_name;
            const char* variadic_ellipsis = import.is_variadic ? ", ..." : "";
            file << "using fexldr_type_" << libname << "_" << function_name << " = auto " << "(" << format_function_params(import) << variadic_ellipsis << ") -> " << import.return_type.getAsString() << ";\n";
            file << "static fexldr_type_" << libname << "_" << function_name << " *fexldr_ptr_" << libname << "_" << function_name << ";\n";
        }

        file << "extern \"C\" {\n";
        for (auto& thunk : thunks) {
            const auto& function_name = thunk.function_name;

            // Generate stub callbacks
            for (auto& [cb_idx, cb] : thunk.callbacks) {
                if (cb.is_stub) {
                    const char* variadic_ellipsis = cb.is_variadic ? ", ..." : "";
                    auto cb_function_name = "fexfn_unpack_" + get_callback_name(function_name, cb_idx) + "_stub";
                    file << "[[noreturn]] static " << cb.return_type.getAsString() << " "
                         << cb_function_name << "("
                         << format_function_params(cb) << variadic_ellipsis << ") {\n";
                    file << "  fprintf(stderr, \"FATAL: Attempted to invoke callback stub for " << function_name << "\\n\");\n";
                    file << "  std::abort();\n";
                    file << "}\n";
                }
            }

            // Forward declarations for user-provided implementations
            if (thunk.custom_host_impl) {
                file << "static auto fexfn_impl_" << libname << "_" << function_name << "(";
                for (std::size_t idx = 0; idx < thunk.param_types.size(); ++idx) {
                    // TODO: fex_guest_function_ptr for guest callbacks?
                    auto& type = thunk.param_types[idx];

                    file << (idx == 0 ? "" : ", ");

                    auto cb = thunk.callbacks.find(idx);
                    if (cb != thunk.callbacks.end() && cb->second.is_guest) {
                        file << "fex_guest_function_ptr a_" << idx;
                    } else {
                      file << format_decl(type, fmt::format("a_{}", idx));
                    }
                }
                // Using trailing return type as it makes handling function pointer returns much easier
                file << ") -> " << thunk.return_type.getAsString() << ";\n";
            }

            // Packed argument structs used in fexfn_unpack_*
            auto GeneratePackedArgs = [&](const auto &function_name, const auto &thunk) -> std::string {
                std::string struct_name = "fexfn_packed_args_" + libname + "_" + function_name;
                file << "struct " << struct_name << " {\n";

                file << format_struct_members(thunk, "  ");
                if (!thunk.return_type->isVoidType()) {
                    file << "  " << format_decl(thunk.return_type, "rv") << ";\n";
                } else if (thunk.param_types.size() == 0) {
                    // Avoid "empty struct has size 0 in C, size 1 in C++" warning
                    file << "    char force_nonempty;\n";
                }
                file << "};\n";
                return struct_name;
            };
            auto struct_name = GeneratePackedArgs(function_name, thunk);

            // Unpacking functions
            auto function_to_call = "fexldr_ptr_" + libname + "_" + function_name;
            if (thunk.custom_host_impl) {
                function_to_call = "fexfn_impl_" + libname + "_" + function_name;
            }

            file << "static void fexfn_unpack_" << libname << "_" << function_name << "(" << struct_name << "* args) {\n";
            file << (thunk.return_type->isVoidType() ? "  " : "  args->rv = ") << function_to_call << "(";
            {
                auto format_param = [&](std::size_t idx) {
                    auto cb = thunk.callbacks.find(idx);
                    if (cb != thunk.callbacks.end() && cb->second.is_stub) {
                        return "fexfn_unpack_" + get_callback_name(function_name, cb->first) + "_stub";
                    } else if (cb != thunk.callbacks.end() && cb->second.is_guest) {
                        return fmt::format("fex_guest_function_ptr {{ args->a_{} }}", idx);
                    } else if (cb != thunk.callbacks.end()) {
                        auto arg_name = fmt::format("args->a_{}", idx);
                        // Use comma operator to inject a function call before returning the argument
                        return "(FinalizeHostTrampolineForGuestFunction(" + arg_name + "), " + arg_name + ")";

                    } else {
                        return fmt::format("args->a_{}", idx);
                    }
                };

                file << format_function_args(thunk, format_param);
            }
            file << ");\n";
            file << "}\n";
        }
        file << "}\n";

        // Endpoints for Guest->Host invocation of API functions
        file << "static ExportEntry exports[] = {\n";
        for (auto& thunk : thunks) {
            const auto& function_name = thunk.function_name;
            auto sha256 = get_sha256(function_name);
            fmt::print( file, "  {{(uint8_t*)\"\\x{:02x}\", (void(*)(void *))&fexfn_unpack_{}_{}}}, // {}:{}\n",
                        fmt::join(sha256, "\\x"), libname, function_name, libname, function_name);
        }

        // Endpoints for Guest->Host invocation of runtime host-function pointers
        for (auto& type : funcptr_types) {
            std::string mangled_name = clang::QualType { type, 0 }.getAsString();
            auto cb_sha256 = get_sha256("fexcallback_" + mangled_name);
            fmt::print( file, "  {{(uint8_t*)\"\\x{:02x}\", (void(*)(void *))&CallbackUnpack<{}>::ForIndirectCall}},\n",
                        fmt::join(cb_sha256, "\\x"), mangled_name);
        }
        file << "  { nullptr, nullptr }\n";
        file << "};\n";

        // Symbol lookup from native host library
        file << "static void* fexldr_ptr_" << libname << "_so;\n";
        file << "extern \"C\" bool fexldr_init_" << libname << "() {\n";

        std::string version_suffix;
        if (lib_version) {
          version_suffix = '.' + std::to_string(*lib_version);
        }
        const std::string library_filename = libfilename + ".so" + version_suffix;

        // Load the host library in the global symbol namespace.
        // This follows how these libraries get loaded in a non-emulated environment,
        // Either by directly linking to the library or a loader (In OpenGL or Vulkan) putting everything in the global namespace.
        file << "  fexldr_ptr_" << libname << "_so = dlopen(\"" << library_filename << "\", RTLD_GLOBAL | RTLD_LAZY);\n";

        file << "  if (!fexldr_ptr_" << libname << "_so) { return false; }\n\n";
        for (auto& import : thunked_api) {
            fmt::print( file, "  (void*&)fexldr_ptr_{}_{} = {}(fexldr_ptr_{}_so, \"{}\");\n",
                        libname, import.function_name, import.host_loader, libname, import.function_name);
        }
        file << "  return true;\n";
        file << "}\n";
    }
}

std::unique_ptr<clang::ASTConsumer> GenerateThunkLibsAction::CreateASTConsumer(clang::CompilerInstance&, clang::StringRef) {
    return std::make_unique<ASTConsumer>();
}

std::unique_ptr<clang::FrontendAction> GenerateThunkLibsActionFactory::create() {
    return std::make_unique<GenerateThunkLibsAction>(libname, output_filenames);
}

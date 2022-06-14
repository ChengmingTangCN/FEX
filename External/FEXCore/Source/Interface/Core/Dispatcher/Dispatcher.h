#pragma once

#include <FEXCore/Core/CPUBackend.h>

#include "Interface/Context/Context.h"
#include "Interface/Core/ArchHelpers/MContext.h"

#include <cstdint>
#include <signal.h>
#include <stddef.h>
#include <stack>
#include <tuple>
#include <vector>

namespace FEXCore {
struct GuestSigAction;
}

namespace FEXCore::Core {
struct CpuStateFrame;
struct InternalThreadState;
}

namespace FEXCore::CPU {

struct DispatcherConfig {
  bool InterpreterDispatch = false;
  uintptr_t ExitFunctionLink = 0;
  bool SupportsStaticRegisterAllocation = false;
};

class Dispatcher {
public:
  virtual ~Dispatcher() = default;
  
  /**
   * @name Dispatch Helper functions
   * @{ */
  uint64_t ThreadStopHandlerAddress{};
  uint64_t ThreadStopHandlerAddressSpillSRA{};
  uint64_t AbsoluteLoopTopAddress{};
  uint64_t AbsoluteLoopTopAddressFillSRA{};
  uint64_t ThreadPauseHandlerAddress{};
  uint64_t ThreadPauseHandlerAddressSpillSRA{};
  uint64_t ExitFunctionLinkerAddress{};
  uint64_t SignalHandlerReturnAddress{};
  uint64_t UnimplementedInstructionAddress{};
  uint64_t OverflowExceptionInstructionAddress{};
  uint64_t IntCallbackReturnAddress{};

  uint64_t PauseReturnInstruction{};

  /**  @} */

  uint64_t Start{};
  uint64_t End{};

  bool HandleGuestSignal(FEXCore::Core::InternalThreadState *Thread, int Signal, void *info, void *ucontext, GuestSigAction *GuestAction, stack_t *GuestStack);
  bool HandleSIGILL(FEXCore::Core::InternalThreadState *Thread, int Signal, void *info, void *ucontext);
  bool HandleSignalPause(FEXCore::Core::InternalThreadState *Thread, int Signal, void *info, void *ucontext);

  bool IsAddressInDispatcher(uint64_t Address) const {
    return Address >= Start && Address < End;
  }

  virtual void InitThreadPointers(FEXCore::Core::InternalThreadState *Thread) = 0;

  static std::unique_ptr<Dispatcher> CreateX86(FEXCore::Context::Context *CTX, DispatcherConfig &Config);
  static std::unique_ptr<Dispatcher> CreateArm64(FEXCore::Context::Context *CTX, DispatcherConfig &Config);
  
  void ExecuteDispatch(FEXCore::Core::CpuStateFrame *Frame) {
    DispatchPtr(Frame);
  }

  void ExecuteJITCallback(FEXCore::Core::CpuStateFrame *Frame, uint64_t RIP) {
    CallbackPtr(Frame, RIP);
  }

protected:
  Dispatcher(FEXCore::Context::Context *ctx)
    : CTX {ctx}
    {}

  ArchHelpers::Context::ContextBackup* StoreThreadState(FEXCore::Core::InternalThreadState *Thread, int Signal, void *ucontext);
  void RestoreThreadState(FEXCore::Core::InternalThreadState *Thread, void *ucontext);
  std::stack<uint64_t, std::vector<uint64_t>> SignalFrames;

  bool SRAEnabled = false;
  virtual void SpillSRA(FEXCore::Core::InternalThreadState *Thread, void *ucontext, uint32_t IgnoreMask) {}

  FEXCore::Context::Context *CTX;

  static void SleepThread(FEXCore::Context::Context *ctx, FEXCore::Core::CpuStateFrame *Frame);

  static uint64_t GetCompileBlockPtr();

  using AsmDispatch = void(*)(FEXCore::Core::CpuStateFrame *Frame);
  using JITCallback = void(*)(FEXCore::Core::CpuStateFrame *Frame, uint64_t RIP);

  AsmDispatch DispatchPtr;
  JITCallback CallbackPtr;
};

}

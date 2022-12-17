%ifdef CONFIG
{
  "HostFeatures": ["AVX"],
  "RegData": {
    "XMM1":  ["0x20A121A222A323A4", "0x28A929AA2AAB2BAC", "0x0000000000000000", "0x0000000000000000"],
    "XMM2":  ["0x0041004300450047", "0x0051005300550057", "0x0000000000000000", "0x0000000000000000"],
    "XMM3":  ["0x0000000000000000", "0x0000000000000000", "0x0000000000000000", "0x0000000000000000"],
    "XMM4":  ["0x0000000000000000", "0x0000000000000000", "0x0000000000000000", "0x0000000000000000"],
    "XMM5":  ["0x0000000000000000", "0x0000000000000000", "0x0000000000000000", "0x0000000000000000"],
    "XMM6":  ["0x20A121A222A323A4", "0x28A929AA2AAB2BAC", "0x20A121A222A323A4", "0x28A929AA2AAB2BAC"],
    "XMM7":  ["0x0041004300450047", "0x0051005300550057", "0x0041004300450047", "0x0051005300550057"],
    "XMM8":  ["0x0000000000000000", "0x0000000000000000", "0x0000000000000000", "0x0000000000000000"],
    "XMM9":  ["0x0000000000000000", "0x0000000000000000", "0x0000000000000000", "0x0000000000000000"],
    "XMM10": ["0x0000000000000000", "0x0000000000000000", "0x0000000000000000", "0x0000000000000000"],
    "XMM12": ["0x20A121A222A323A4", "0x28A929AA2AAB2BAC", "0x0000000000000000", "0x0000000000000000"],
    "XMM13": ["0x20A121A222A323A4", "0x28A929AA2AAB2BAC", "0x20A121A222A323A4", "0x28A929AA2AAB2BAC"]
  },
  "MemoryRegions": {
    "0x100000000": "4096"
  }
}
%endif

lea rdx, [rel .data]

vmovapd ymm0, [rdx]

vpsrlw xmm1, xmm0, [rdx + 32 * 1]
vpsrlw xmm2, xmm0, [rdx + 32 * 2]
vpsrlw xmm3, xmm0, [rdx + 32 * 3]
vpsrlw xmm4, xmm0, [rdx + 32 * 4]
vpsrlw xmm5, xmm0, [rdx + 32 * 5]

vpsrlw ymm6, ymm0, [rdx + 32 * 1]
vpsrlw ymm7, ymm0, [rdx + 32 * 2]
vpsrlw ymm8, ymm0, [rdx + 32 * 3]
vpsrlw ymm9, ymm0, [rdx + 32 * 4]
vpsrlw ymm10, ymm0, [rdx + 32 * 5]

vmovapd ymm11, [rdx + 32]

vpsrlw xmm12, xmm0, xmm11
vpsrlw ymm13, ymm0, xmm11


hlt

align 32
.data:
dq 0x4142434445464748
dq 0x5152535455565758
dq 0x4142434445464748
dq 0x5152535455565758

dq 0x0000000000000001
dq 0x0000000000000000
dq 0x0000000000000000
dq 0x0000000000000000

dq 0x0000000000000008
dq 0x0000000000000000
dq 0x0000000000000000
dq 0x0000000000000000

dq 0x0000000000000010
dq 0x0000000000000000
dq 0x0000000000000000
dq 0x0000000000000000

dq 0x0000000000000020
dq 0x0000000000000000
dq 0x0000000000000000
dq 0x0000000000000000

dq 0x0000000000000040
dq 0x0000000000000000
dq 0x0000000000000000
dq 0x0000000000000000
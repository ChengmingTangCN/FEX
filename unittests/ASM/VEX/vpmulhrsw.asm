%ifdef CONFIG
{
  "HostFeatures": ["AVX"],
  "RegData": {
    "XMM2": ["0x31A6343B36E09E7A", "0x48134B294E4F5186", "0x0000000000000000", "0x0000000000000000"],
    "XMM3": ["0x31A6343B36E09E7A", "0x48134B294E4F5186", "0x0000000000000000", "0x0000000000000000"],
    "XMM4": ["0x31A6343B36E09E7A", "0x48134B294E4F5186", "0x31A6343B36E09E7A", "0x48134B294E4F5186"],
    "XMM5": ["0x31A6343B36E09E7A", "0x48134B294E4F5186", "0x31A6343B36E09E7A", "0x48134B294E4F5186"]
  }
}
%endif

lea rdx, [rel .data]

vmovaps ymm0, [rdx]
vmovaps ymm1, [rdx + 32]

vpmulhrsw xmm2, xmm0, xmm1
vpmulhrsw xmm3, xmm0, [rdx + 32]

vpmulhrsw ymm4, ymm0, ymm1
vpmulhrsw ymm5, ymm0, [rdx + 32]

hlt

align 32
.data:
dq 0x4142434445468748
dq 0x5152535455565758
dq 0x4142434445468748
dq 0x5152535455565758

dq 0x6162636465666768
dq 0x7172737475767778
dq 0x6162636465666768
dq 0x7172737475767778

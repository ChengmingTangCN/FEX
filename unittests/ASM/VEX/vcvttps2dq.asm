%ifdef CONFIG
{
  "HostFeatures": ["AVX"],
  "RegData": {
    "XMM0":  ["0x0000004500000053", "0x0000000D00000029", "0x0000000000000000", "0x0000000000000000"],
    "XMM1":  ["0x0000001500000005", "0x0000000500000009", "0x0000000000000000", "0x0000000000000000"],
    "XMM2":  ["0x000000420000001D", "0x0000005B00000013", "0x0000000000000000", "0x0000000000000000"],
    "XMM3":  ["0x0000003200000028", "0x0000001700000020", "0x0000000000000000", "0x0000000000000000"],
    "XMM4":  ["0x000000180000005A", "0x0000005B00000062", "0x0000000000000000", "0x0000000000000000"],
    "XMM5":  ["0x000000630000005A", "0x0000004A00000040", "0x0000000000000000", "0x0000000000000000"],
    "XMM6":  ["0x0000001900000023", "0x0000005A00000005", "0x0000000000000000", "0x0000000000000000"],
    "XMM7":  ["0x0000003400000021", "0x0000000A0000003A", "0x0000000000000000", "0x0000000000000000"],
    "XMM8":  ["0x000000540000002F", "0x000000410000005A", "0x000000540000002F", "0x000000410000005A"],
    "XMM9":  ["0x0000000600000060", "0x0000005F00000019", "0x0000000600000060", "0x0000005F00000019"],
    "XMM10": ["0x0000002500000058", "0x0000000A00000031", "0x0000002500000058", "0x0000000A00000031"],
    "XMM11": ["0x000000140000004D", "0x0000002900000009", "0x000000140000004D", "0x0000002900000009"],
    "XMM12": ["0x000000390000000F", "0x0000003700000009", "0x000000390000000F", "0x0000003700000009"],
    "XMM13": ["0x0000000400000034", "0x0000000300000048", "0x0000000400000034", "0x0000000300000048"],
    "XMM14": ["0x0000004700000038", "0x000000580000003D", "0x0000004700000038", "0x000000580000003D"],
    "XMM15": ["0x000000180000002F", "0x0000006000000021", "0x000000180000002F", "0x0000006000000021"]
  }
}
%endif

lea rdx, [rel .data]

vcvttps2dq xmm0,  [rdx + 32 * 0]
vcvttps2dq xmm1,  [rdx + 32 * 1]
vcvttps2dq xmm2,  [rdx + 32 * 2]
vcvttps2dq xmm3,  [rdx + 32 * 3]
vcvttps2dq xmm4,  [rdx + 32 * 4]
vcvttps2dq xmm5,  [rdx + 32 * 5]
vcvttps2dq xmm6,  [rdx + 32 * 6]
vcvttps2dq xmm7,  [rdx + 32 * 7]
vcvttps2dq ymm8,  [rdx + 32 * 8]
vcvttps2dq ymm9,  [rdx + 32 * 9]
vcvttps2dq ymm10, [rdx + 32 * 10]
vcvttps2dq ymm11, [rdx + 32 * 11]
vcvttps2dq ymm12, [rdx + 32 * 12]
vcvttps2dq ymm13, [rdx + 32 * 13]
vcvttps2dq ymm14, [rdx + 32 * 14]
vcvttps2dq ymm15, [rdx + 32 * 15]

hlt

align 32
.data:
dd 83.0999 , 69.50512, 41.02678, 13.05881
dd 83.0999 , 69.50512, 41.02678, 13.05881

dd 5.35242 , 21.9932 , 9.67383 , 5.32372
dd 5.35242 , 21.9932 , 9.67383 , 5.32372

dd 29.02872, 66.50151, 19.30764, 91.3633
dd 29.02872, 66.50151, 19.30764, 91.3633

dd 40.45086, 50.96153, 32.64489, 23.97574
dd 40.45086, 50.96153, 32.64489, 23.97574

dd 90.64316, 24.22547, 98.9394 , 91.21715
dd 90.64316, 24.22547, 98.9394 , 91.21715

dd 90.80143, 99.48407, 64.97245, 74.39838
dd 90.80143, 99.48407, 64.97245, 74.39838

dd 35.22761, 25.35321, 5.8732  , 90.19956
dd 35.22761, 25.35321, 5.8732  , 90.19956

dd 33.03133, 52.02952, 58.38554, 10.17531
dd 33.03133, 52.02952, 58.38554, 10.17531

dd 47.84703, 84.04831, 90.02965, 65.81329
dd 47.84703, 84.04831, 90.02965, 65.81329

dd 96.27991, 6.64479 , 25.58971, 95.00694
dd 96.27991, 6.64479 , 25.58971, 95.00694

dd 88.1929 , 37.16964, 49.52602, 10.27223
dd 88.1929 , 37.16964, 49.52602, 10.27223

dd 77.70605, 20.21439, 9.8056  , 41.29389
dd 77.70605, 20.21439, 9.8056  , 41.29389

dd 15.4071 , 57.54286, 9.61117 , 55.54302
dd 15.4071 , 57.54286, 9.61117 , 55.54302

dd 52.90745, 4.88086 , 72.52882, 3.0201
dd 52.90745, 4.88086 , 72.52882, 3.0201

dd 56.55091, 71.22749, 61.84736, 88.74295
dd 56.55091, 71.22749, 61.84736, 88.74295

dd 47.72641, 24.17404, 33.70564, 96.71303
dd 47.72641, 24.17404, 33.70564, 96.71303
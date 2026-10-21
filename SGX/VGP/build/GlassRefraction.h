/*************************************** 
  Source File	: SGX/VGP/build/GlassRefraction
  Compiler		: version 1.1
  Date			: Wed Oct 21 06:01:05 2026
 
  Optimized code
****************************************/

const unsigned int vgp_GlassRefraction[] = {

0x20504756, // magic vgp id
0xffff0101, // vgp version
0x00000020, // code size (in instructions)
0x00000300, // clipping planes

0x00000200, // Section 0
0x01001f03, // Section 1
0xf0000000, // Section 2
0xf0000000, // Section 3
0xf0000000, // Section 4
0xf0000000, // Section 5
0xf0000000, // Section 6
0xf0000000, // Section 7
0xf0000000, // Section 8
0xf0000000, // Section 9
0xf0000000, // Section 10
0xf0000000, // Section 11
0xf0000000, // Section 12
0xf0000000, // Section 13
0xf0000000, // Section 14
0xf0000000, // Section 15

0x0000e401, 0x03c00000,   // (S00)  mov opos,v0
0x0000c601, 0x07c00001,   // (S00)  mov od0,v1.zyxw
0x0000e401, 0x0fc00002,   // (S00)  mov ot0,v2

0x00e4e407, 0x50410035,   // (S01)  dp3 r4.x,c5,v3
0x00e4e407, 0x50810036,   // (S01)  dp3 r4.y,c6,v3
0x00e4e407, 0x51010037,   // (S01)  dp3 r4.z,c7,v3
0x00e4e408, 0x4c410001,   // (S01)  dp4 r3.x,c1,v0
0x00e4e408, 0x4c810002,   // (S01)  dp4 r3.y,c2,v0
0x00e4e408, 0x4d010003,   // (S01)  dp4 r3.z,c3,v0
0x00e4e408, 0x4e010004,   // (S01)  dp4 r3.w,c4,v0
0x0000c601, 0x07c1000a,   // (S01)  mov od0,c10.zyxw
0x00e4e407, 0x544a0044,   // (S01)  dp3 r5.x,r4,r4
0x00e40004, 0x584600c3,   // (S01)  mul r6.x,r3.xxxx,c12
0x0000e401, 0x4bc20003,   // (S01)  mov r2,r3
0x00000006, 0x54420005,   // (S01)  rsq r5.x,r5.xxxx
0x00e4ff03, 0x586606d3,   // (S01)  mad r6.x,r3.wwww,c13,r6.xxxx
0x00e45504, 0x588600e3,   // (S01)  mul r6.y,r3.yyyy,c14
0x00e4e408, 0x1c491021,   // (S01)  dp4 oc2.x,c17,r2
0x00e4e408, 0x1c891022,   // (S01)  dp4 oc2.y,c18,r2
0x0000e401, 0x03c20002,   // (S01)  mov opos,r2
0x0000e404, 0x51ca0054,   // (S01)  mul r4.xyz,r4,r5.xxxx
0x55e4ff03, 0x58a606f3,   // (S01)  mad r6.y,r3.wwww,c15,r6.yyyy
0x00aae442, 0x5489004b,   // (S01)  add r5.y,c11,-r4.zzzz
0x00550004, 0x550a0054,   // (S01)  mul r5.z,r4.xxxx,r5.yyyy
0x0000aa04, 0x55060095,   // (S01)  mul r5.z,r5.zzzz,c9.xxxx
0x00ffaa03, 0x586a0635,   // (S01)  mad r6.x,r5.zzzz,r3.wwww,r6.xxxx
0x00555504, 0x550a0054,   // (S01)  mul r5.z,r4.yyyy,r5.yyyy
0x0055aa04, 0x55060095,   // (S01)  mul r5.z,r5.zzzz,c9.yyyy
0x55ffaa03, 0x58aa0635,   // (S01)  mad r6.y,r5.zzzz,r3.wwww,r6.yyyy
0x0000e401, 0x59011000,   // (S01)  mov r6.z,c16
0x0000ff01, 0x5a020003,   // (S01)  mov r6.w,r3.wwww
0x0000e401, 0x0fc20006,   // (S01)  mov ot0,r6
0x00000030,// Code Flags : 48
0x00010001,// Code Compatibility Version : 65537
0x00000001, // Section 0 Flags : 1
0x00000002, // Section 1 Flags : 2
0x00000000, // Section 2 Flags : 0
0x00000000, // Section 3 Flags : 0
0x00000000, // Section 4 Flags : 0
0x00000000, // Section 5 Flags : 0
0x00000000, // Section 6 Flags : 0
0x00000000, // Section 7 Flags : 0
0x00000000, // Section 8 Flags : 0
0x00000000, // Section 9 Flags : 0
0x00000000, // Section 10 Flags : 0
0x00000000, // Section 11 Flags : 0
0x00000000, // Section 12 Flags : 0
0x00000000, // Section 13 Flags : 0
0x00000000, // Section 14 Flags : 0
0x00000000, // Section 15 Flags : 0
0x00000022,// Num Bindings : 34
0x00000003, // Binding type : 3
0x00000000, // Constant number : 0
0x00000000, // x : 0
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000003, // Section Flags: 3
0x00000005, // Binding type : 5
0x00000001, // Constant number : 1
0x00000000, // x : 0
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000001, // Section Flags: 1
0x00000007, // Binding type : 7
0x00000002, // Constant number : 2
0x00000000, // x : 0
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000001, // Section Flags: 1
0x00000004, // Binding type : 4
0x00000003, // Constant number : 3
0x00000000, // x : 0
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x0000003f, // Binding type : 63
0x00000000, // Constant number : 0
0x3f800000, // x : 1
0xbf800000, // y : -1
0x00000000, // z : 0
0x00000000, // w : 0
0x00000000, // Section Flags: 0
0x0000002d, // Binding type : 45
0x00000001, // Constant number : 1
0x00000000, // x : 0
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x0000002e, // Binding type : 46
0x00000002, // Constant number : 2
0x00000000, // x : 0
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x0000002f, // Binding type : 47
0x00000003, // Constant number : 3
0x00000000, // x : 0
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x00000030, // Binding type : 48
0x00000004, // Constant number : 4
0x00000000, // x : 0
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x00000025, // Binding type : 37
0x00000005, // Constant number : 5
0x00000000, // x : 0
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x00000026, // Binding type : 38
0x00000006, // Constant number : 6
0x00000000, // x : 0
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x00000027, // Binding type : 39
0x00000007, // Constant number : 7
0x00000000, // x : 0
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x00000028, // Binding type : 40
0x00000008, // Constant number : 8
0x00000000, // x : 0
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x0000003d, // Binding type : 61
0x00000009, // Constant number : 9
0x41000000, // x : 8
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x0000003d, // Binding type : 61
0x0000000a, // Constant number : 10
0x41100000, // x : 9
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x0000003f, // Binding type : 63
0x0000000b, // Constant number : 11
0x3f800000, // x : 1
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x0000003f, // Binding type : 63
0x0000000c, // Constant number : 12
0x3f000000, // x : 0.5
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x0000003f, // Binding type : 63
0x0000000d, // Constant number : 13
0x3f000000, // x : 0.5
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x0000003f, // Binding type : 63
0x0000000e, // Constant number : 14
0x3f000000, // x : 0.5
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x0000003f, // Binding type : 63
0x0000000f, // Constant number : 15
0x3f000000, // x : 0.5
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x0000003f, // Binding type : 63
0x00000010, // Constant number : 16
0x00000000, // x : 0
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x00000021, // Binding type : 33
0x00000011, // Constant number : 17
0x00000000, // x : 0
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x00000022, // Binding type : 34
0x00000012, // Constant number : 18
0x00000000, // x : 0
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x00000001, // Binding type : 1
0x00000000, // Constant number : 0
0x00000000, // x : 0
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000000, // Section Flags: 0
0x00000001, // Binding type : 1
0x00000001, // Constant number : 1
0x00000000, // x : 0
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000000, // Section Flags: 0
0x00000001, // Binding type : 1
0x00000002, // Constant number : 2
0x00000000, // x : 0
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x00000001, // Binding type : 1
0x00000003, // Constant number : 3
0x00000000, // x : 0
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x00000001, // Binding type : 1
0x00000004, // Constant number : 4
0x00000000, // x : 0
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x00000001, // Binding type : 1
0x00000005, // Constant number : 5
0x00000000, // x : 0
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x00000001, // Binding type : 1
0x00000006, // Constant number : 6
0x00000000, // x : 0
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x00000042, // Binding type : 66
0x00000000, // Constant number : 0
0x00000000, // x : 0
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000003, // Section Flags: 3
0x00000043, // Binding type : 67
0x00000001, // Constant number : 1
0x00000000, // x : 0
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000003, // Section Flags: 3
0x00000045, // Binding type : 69
0x00000002, // Constant number : 2
0x00000000, // x : 0
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000003, // Section Flags: 3
0x00000047, // Binding type : 71
0x00000003, // Constant number : 3
0x40000000, // x : 2
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0};

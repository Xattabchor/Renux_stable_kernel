/*************************************** 
  Source File	: SGX/VGP/build/MBXWaterLite
  Compiler		: version 1.1
  Date			: Wed Oct 21 07:46:41 2026
 
  Optimized code
****************************************/

const unsigned int vgp_MBXWaterLite[] = {

0x20504756, // magic vgp id
0xffff0101, // vgp version
0x0000003f, // code size (in instructions)
0x00000300, // clipping planes

0x00000200, // Section 0
0x01003d03, // Section 1
0x01003e3e, // Section 2
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

0x0000e401, 0x0fc00002,   // (S00)  mov ot0,v2
0x0000e401, 0x03c00000,   // (S00)  mov opos,v0
0x0000c601, 0x07c00001,   // (S00)  mov od0,v1.zyxw

0x00e4e407, 0x58410035,   // (S01)  dp3 r6.x,c5,v3
0x00e4e407, 0x58810036,   // (S01)  dp3 r6.y,c6,v3
0x00e4e407, 0x59010037,   // (S01)  dp3 r6.z,c7,v3
0x0000e401, 0x4fc00000,   // (S01)  mov r3,v0
0x0000ff01, 0x43c00003,   // (S01)  mov r0,v3.wwww
0x0000c601, 0x06011009,   // (S01)  mov od0.w,c25.zyxw
0x00e4e407, 0x5c4a0066,   // (S01)  dp3 r7.x,r6,r6
0x00e45502, 0x4c880000,   // (S01)  add r3.y,v0.yyyy,r0
0x00000021, 0x43c11002,   // (S01)  mov r0,-c18.xxxx
0x00e4000a, 0x5c461037,   // (S01)  max r7.x,r7.xxxx,c19
0x00e4e408, 0x54490035,   // (S01)  dp4 r5.x,c5,r3
0x00e4e408, 0x54890036,   // (S01)  dp4 r5.y,c6,r3
0x00e4e408, 0x55090037,   // (S01)  dp4 r5.z,c7,r3
0x00e4e408, 0x56090038,   // (S01)  dp4 r5.w,c8,r3
0x00000006, 0x5c420007,   // (S01)  rsq r7.x,r7.xxxx
0x00e4e408, 0x68490039,   // (S01)  dp4 r10.x,c9,r3
0x00e4e408, 0x6c49003d,   // (S01)  dp4 r11.x,c13,r3
0x00e4e408, 0x6889003a,   // (S01)  dp4 r10.y,c10,r3
0x00e4e408, 0x6c89003e,   // (S01)  dp4 r11.y,c14,r3
0x00e4e408, 0x6909003b,   // (S01)  dp4 r10.z,c11,r3
0x00e4e408, 0x6d09003f,   // (S01)  dp4 r11.z,c15,r3
0x00e4e408, 0x50490031,   // (S01)  dp4 r4.x,c1,r3
0x00e4e408, 0x6a09003c,   // (S01)  dp4 r10.w,c12,r3
0x00e4e408, 0x6e091030,   // (S01)  dp4 r11.w,c16,r3
0x00e4e408, 0x50890032,   // (S01)  dp4 r4.y,c2,r3
0x00e4e408, 0x51090033,   // (S01)  dp4 r4.z,c3,r3
0x00e4e408, 0x52090034,   // (S01)  dp4 r4.w,c4,r3
0x0000e404, 0x59ca0076,   // (S01)  mul r6.xyz,r6,r7.xxxx
0x0000e421, 0x61c20005,   // (S01)  mov r8.xyz,-r5
0x00e4e402, 0x70491008,   // (S01)  add r12.x,c24,r0
0x0000e401, 0x4bc20004,   // (S01)  mov r2,r4
0x00e4e407, 0x5c4a0088,   // (S01)  dp3 r7.x,r8,r8
0x0000e401, 0x03c20002,   // (S01)  mov opos,r2
0x00e4e408, 0x1449102a,   // (S01)  dp4 oc2.x,c26,r2
0x00e4e408, 0x1489102b,   // (S01)  dp4 oc2.y,c27,r2
0x00e4000a, 0x5c461047,   // (S01)  max r7.x,r7.xxxx,c20
0x00000006, 0x5c420007,   // (S01)  rsq r7.x,r7.xxxx
0x0000e404, 0x61ca0078,   // (S01)  mul r8.xyz,r8,r7.xxxx
0x00e4e407, 0x644a0086,   // (S01)  dp3 r9.x,r6,r8
0x00e4000a, 0x64461059,   // (S01)  max r9.x,r9.xxxx,c21
0x00e40009, 0x64461069,   // (S01)  min r9.x,r9.xxxx,c22
0x0000e442, 0x64891097,   // (S01)  add r9.y,c23,-r9.xxxx
0x00555504, 0x650a0099,   // (S01)  mul r9.z,r9.yyyy,r9.yyyy
0x0055aa04, 0x650a0099,   // (S01)  mul r9.z,r9.zzzz,r9.yyyy
0x0055aa04, 0x650a0099,   // (S01)  mul r9.z,r9.zzzz,r9.yyyy
0x0055aa04, 0x650a0099,   // (S01)  mul r9.z,r9.zzzz,r9.yyyy
0x0000aa03, 0x645a12c9,   // (S01)  mad r9.x,r9.zzzz,r12.xxxx,c18.xxxx
0x00000004, 0x70461016,   // (S01)  mul r12.x,r6.xxxx,c17.xxxx
0x00000001, 0x05c20009,   // (S01)  mov od0.xyz,r9.xxxx
0x00ff0003, 0x686a0aac,   // (S01)  mad r10.x,r12.xxxx,r10.wwww,r10.xxxx
0x0055aa04, 0x70461016,   // (S01)  mul r12.x,r6.zzzz,c17.yyyy
0x55ff0003, 0x68aa0aac,   // (S01)  mad r10.y,r12.xxxx,r10.wwww,r10.yyyy
0x00aa0004, 0x70461016,   // (S01)  mul r12.x,r6.xxxx,c17.zzzz
0x0000e401, 0x0fc2000a,   // (S01)  mov ot0,r10
0x00ff0003, 0x6c6a0bbc,   // (S01)  mad r11.x,r12.xxxx,r11.wwww,r11.xxxx
0x00ffaa04, 0x70461016,   // (S01)  mul r12.x,r6.zzzz,c17.wwww
0x55ff0003, 0x6caa0bbc,   // (S01)  mad r11.y,r12.xxxx,r11.wwww,r11.yyyy
0x0000e401, 0x77c2000b,   // (S01)  mov r13,r11
0x0000e401, 0x13c2000d,   // (S01)  mov ot1,r13

0x0000e401, 0x0fc2000d,   // (S02)  mov ot0,r13
0x00000231,// Code Flags : 561
0x00010001,// Code Compatibility Version : 65537
0x00000001, // Section 0 Flags : 1
0x00000002, // Section 1 Flags : 2
0x00000006, // Section 2 Flags : 6
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
0x00000033,// Num Bindings : 51
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
0x00000007, // Binding type : 7
0x00000003, // Constant number : 3
0x3f800000, // x : 1
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
0x00000000, // x : 0
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x0000003d, // Binding type : 61
0x0000000a, // Constant number : 10
0x3f800000, // x : 1
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x0000003d, // Binding type : 61
0x0000000b, // Constant number : 11
0x40000000, // x : 2
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x0000003d, // Binding type : 61
0x0000000c, // Constant number : 12
0x40400000, // x : 3
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x0000003d, // Binding type : 61
0x0000000d, // Constant number : 13
0x40800000, // x : 4
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x0000003d, // Binding type : 61
0x0000000e, // Constant number : 14
0x40a00000, // x : 5
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x0000003d, // Binding type : 61
0x0000000f, // Constant number : 15
0x40c00000, // x : 6
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x0000003d, // Binding type : 61
0x00000010, // Constant number : 16
0x40e00000, // x : 7
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x0000003d, // Binding type : 61
0x00000011, // Constant number : 17
0x41000000, // x : 8
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x0000003d, // Binding type : 61
0x00000012, // Constant number : 18
0x41100000, // x : 9
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x0000003f, // Binding type : 63
0x00000013, // Constant number : 19
0x38d1b717, // x : 0.0001
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x0000003f, // Binding type : 63
0x00000014, // Constant number : 20
0x38d1b717, // x : 0.0001
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x0000003f, // Binding type : 63
0x00000015, // Constant number : 21
0x00000000, // x : 0
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x0000003f, // Binding type : 63
0x00000016, // Constant number : 22
0x3f800000, // x : 1
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x0000003f, // Binding type : 63
0x00000017, // Constant number : 23
0x3f800000, // x : 1
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x0000003f, // Binding type : 63
0x00000018, // Constant number : 24
0x3f800000, // x : 1
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x0000003f, // Binding type : 63
0x00000019, // Constant number : 25
0x3f800000, // x : 1
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x00000021, // Binding type : 33
0x0000001a, // Constant number : 26
0x00000000, // x : 0
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x00000022, // Binding type : 34
0x0000001b, // Constant number : 27
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
0x00000001, // Binding type : 1
0x00000007, // Constant number : 7
0x00000000, // x : 0
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x00000001, // Binding type : 1
0x00000008, // Constant number : 8
0x00000000, // x : 0
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x00000001, // Binding type : 1
0x00000009, // Constant number : 9
0x00000000, // x : 0
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x00000001, // Binding type : 1
0x0000000a, // Constant number : 10
0x00000000, // x : 0
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x00000001, // Binding type : 1
0x0000000b, // Constant number : 11
0x00000000, // x : 0
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x00000001, // Binding type : 1
0x0000000c, // Constant number : 12
0x00000000, // x : 0
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x00000001, // Binding type : 1
0x0000000d, // Constant number : 13
0x00000000, // x : 0
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000006, // Section Flags: 6
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
0x00000007, // Section Flags: 7
0x00000045, // Binding type : 69
0x00000003, // Constant number : 3
0x3f800000, // x : 1
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x00000047, // Binding type : 71
0x00000004, // Constant number : 4
0x40000000, // x : 2
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0};

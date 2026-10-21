/*************************************** 
  Source File	: SGX/VGP/build/DynamicBakedShotFlashLighting
  Compiler		: version 1.1
  Date			: Wed Oct 21 06:17:53 2026
 
  Optimized code
****************************************/

const unsigned int vgp_DynamicBakedShotFlashLighting[] = {

0x20504756, // magic vgp id
0xffff0101, // vgp version
0x00000041, // code size (in instructions)
0x00000300, // clipping planes

0x00000200, // Section 0
0x01004003, // Section 1
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

0x00e4e408, 0x50410005,   // (S01)  dp4 r4.x,c5,v0
0x00e4e407, 0x54410035,   // (S01)  dp3 r5.x,c5,v3
0x00e4e408, 0x50810006,   // (S01)  dp4 r4.y,c6,v0
0x00e4e407, 0x54810036,   // (S01)  dp3 r5.y,c6,v3
0x00e4e408, 0x51010007,   // (S01)  dp4 r4.z,c7,v0
0x00e4e407, 0x55010037,   // (S01)  dp3 r5.z,c7,v3
0x00e4e408, 0x52010008,   // (S01)  dp4 r4.w,c8,v0
0x00e4e408, 0x4c410001,   // (S01)  dp4 r3.x,c1,v0
0x00e4e408, 0x4c810002,   // (S01)  dp4 r3.y,c2,v0
0x00e4e408, 0x4d010003,   // (S01)  dp4 r3.z,c3,v0
0x00e4e408, 0x4e010004,   // (S01)  dp4 r3.w,c4,v0
0x0000c601, 0x0601100c,   // (S01)  mov od0.w,c28.zyxw
0x00e4e407, 0x604a0055,   // (S01)  dp3 r8.x,r5,r5
0x00e4e442, 0x59c90049,   // (S01)  add r6.xyz,c9,-r4
0x0000e401, 0x0fc00002,   // (S01)  mov ot0,v2
0x0000e401, 0x4bc20003,   // (S01)  mov r2,r3
0x00e4000a, 0x60461008,   // (S01)  max r8.x,r8.xxxx,c16
0x0000e401, 0x03c20002,   // (S01)  mov opos,r2
0x00e4e408, 0x1c49102d,   // (S01)  dp4 oc2.x,c29,r2
0x00000006, 0x64420008,   // (S01)  rsq r9.x,r8.xxxx
0x00e4e407, 0x604a0066,   // (S01)  dp3 r8.x,r6,r6
0x00e4e408, 0x1c89102e,   // (S01)  dp4 oc2.y,c30,r2
0x0000e404, 0x55ca0095,   // (S01)  mul r5.xyz,r5,r9.xxxx
0x00e4000a, 0x60461018,   // (S01)  max r8.x,r8.xxxx,c17
0x00ff0004, 0x6c460098,   // (S01)  mul r11.x,r8.xxxx,c9.wwww
0x00000006, 0x64420008,   // (S01)  rsq r9.x,r8.xxxx
0x0000e442, 0x6c4910b3,   // (S01)  add r11.x,c19,-r11.xxxx
0x0000e404, 0x5dca0096,   // (S01)  mul r7.xyz,r6,r9.xxxx
0x00000004, 0x704a0099,   // (S01)  mul r12.x,r9.xxxx,r9.xxxx
0x00e4000a, 0x6c46104b,   // (S01)  max r11.x,r11.xxxx,c20
0x00e4e442, 0x59c9004b,   // (S01)  add r6.xyz,c11,-r4
0x00e4e407, 0x684a0075,   // (S01)  dp3 r10.x,r5,r7
0x00000004, 0x6c4a00bb,   // (S01)  mul r11.x,r11.xxxx,r11.xxxx
0x00e4e407, 0x604a0066,   // (S01)  dp3 r8.x,r6,r6
0x00e4000a, 0x6846102a,   // (S01)  max r10.x,r10.xxxx,c18
0x00000004, 0x704a00bc,   // (S01)  mul r12.x,r12.xxxx,r11.xxxx
0x00e4000a, 0x60461058,   // (S01)  max r8.x,r8.xxxx,c21
0x00000004, 0x704a00ac,   // (S01)  mul r12.x,r12.xxxx,r10.xxxx
0x00000006, 0x64420008,   // (S01)  rsq r9.x,r8.xxxx
0x00ff0004, 0x704600ac,   // (S01)  mul r12.x,r12.xxxx,c10.wwww
0x0000e404, 0x5dca0096,   // (S01)  mul r7.xyz,r6,r9.xxxx
0x0000e404, 0x75c900ca,   // (S01)  mul r13.xyz,c10,r12.xxxx
0x00ff0004, 0x704600b8,   // (S01)  mul r12.x,r8.xxxx,c11.wwww
0x00e4e427, 0x6c4600c7,   // (S01)  dp3 r11.x,-r7,c12
0x00e4e407, 0x684a0075,   // (S01)  dp3 r10.x,r5,r7
0x0000e442, 0x704910c9,   // (S01)  add r12.x,c25,-r12.xxxx
0x55000003, 0x6c560eeb,   // (S01)  mad r11.x,r11.xxxx,c14.xxxx,c14.yyyy
0x00e4000a, 0x6846106a,   // (S01)  max r10.x,r10.xxxx,c22
0x00e4000a, 0x704610ac,   // (S01)  max r12.x,r12.xxxx,c26
0x00e4000a, 0x6c46107b,   // (S01)  max r11.x,r11.xxxx,c23
0x00000004, 0x704a00cc,   // (S01)  mul r12.x,r12.xxxx,r12.xxxx
0x00e40009, 0x6c46108b,   // (S01)  min r11.x,r11.xxxx,c24
0x00000004, 0x704a00bc,   // (S01)  mul r12.x,r12.xxxx,r11.xxxx
0x00000004, 0x704a009c,   // (S01)  mul r12.x,r12.xxxx,r9.xxxx
0x00000004, 0x704a009c,   // (S01)  mul r12.x,r12.xxxx,r9.xxxx
0x00000004, 0x704a00ac,   // (S01)  mul r12.x,r12.xxxx,r10.xxxx
0x00ff0004, 0x704600dc,   // (S01)  mul r12.x,r12.xxxx,c13.wwww
0xe400e403, 0x75e90dcd,   // (S01)  mad r13.xyz,c13,r12.xxxx,r13
0x00e4e402, 0x75c600fd,   // (S01)  add r13.xyz,r13,c15
0x00e4e402, 0x75c2001d,   // (S01)  add r13.xyz,r13,v1
0x00e4e409, 0x75c610bd,   // (S01)  min r13.xyz,r13,c27
0x0000c601, 0x05c2000d,   // (S01)  mov od0.xyz,r13.zyxw
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
0x00000035,// Num Bindings : 53
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
0x00000003, // Section Flags: 3
0x00000007, // Binding type : 7
0x00000002, // Constant number : 2
0x00000000, // x : 0
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000003, // Section Flags: 3
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
0x0000003d, // Binding type : 61
0x0000000b, // Constant number : 11
0x41200000, // x : 10
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x0000003d, // Binding type : 61
0x0000000c, // Constant number : 12
0x41300000, // x : 11
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x0000003d, // Binding type : 61
0x0000000d, // Constant number : 13
0x41400000, // x : 12
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x0000003d, // Binding type : 61
0x0000000e, // Constant number : 14
0x41500000, // x : 13
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x0000003d, // Binding type : 61
0x0000000f, // Constant number : 15
0x41600000, // x : 14
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x0000003f, // Binding type : 63
0x00000010, // Constant number : 16
0x38d1b717, // x : 0.0001
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x0000003f, // Binding type : 63
0x00000011, // Constant number : 17
0x38d1b717, // x : 0.0001
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x0000003f, // Binding type : 63
0x00000012, // Constant number : 18
0x00000000, // x : 0
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x0000003f, // Binding type : 63
0x00000013, // Constant number : 19
0x3f800000, // x : 1
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x0000003f, // Binding type : 63
0x00000014, // Constant number : 20
0x00000000, // x : 0
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x0000003f, // Binding type : 63
0x00000015, // Constant number : 21
0x38d1b717, // x : 0.0001
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x0000003f, // Binding type : 63
0x00000016, // Constant number : 22
0x00000000, // x : 0
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x0000003f, // Binding type : 63
0x00000017, // Constant number : 23
0x00000000, // x : 0
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
0x0000003f, // Binding type : 63
0x0000001a, // Constant number : 26
0x00000000, // x : 0
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x0000003f, // Binding type : 63
0x0000001b, // Constant number : 27
0x3f800000, // x : 1
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x0000003f, // Binding type : 63
0x0000001c, // Constant number : 28
0x3f800000, // x : 1
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x00000021, // Binding type : 33
0x0000001d, // Constant number : 29
0x00000000, // x : 0
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x00000022, // Binding type : 34
0x0000001e, // Constant number : 30
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

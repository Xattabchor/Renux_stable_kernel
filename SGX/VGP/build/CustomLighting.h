/*************************************** 
  Source File	: SGX/VGP/build/CustomLighting
  Compiler		: version 1.1
  Date			: Wed Oct 21 09:21:33 2026
 
  Optimized code
****************************************/

const unsigned int vgp_CustomLighting[] = {

0x20504756, // magic vgp id
0xffff0101, // vgp version
0x00000059, // code size (in instructions)
0x00000300, // clipping planes

0x00000200, // Section 0
0x02005803, // Section 1
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
0x00e4e408, 0x4c410005,   // (S01)  dp4 r3.x,c5,v0
0x00e4e408, 0x4c810006,   // (S01)  dp4 r3.y,c6,v0
0x00e4e408, 0x4d010007,   // (S01)  dp4 r3.z,c7,v0
0x00e4e408, 0x4e010008,   // (S01)  dp4 r3.w,c8,v0
0x00e4e408, 0x48410001,   // (S01)  dp4 r2.x,c1,v0
0x00e4e408, 0x48810002,   // (S01)  dp4 r2.y,c2,v0
0x00e4e407, 0x544a0044,   // (S01)  dp3 r5.x,r4,r4
0x00e4e408, 0x49010003,   // (S01)  dp4 r2.z,c3,v0
0x00e4e408, 0x4a010004,   // (S01)  dp4 r2.w,c4,v0
0x0000c601, 0x0601200b,   // (S01)  mov od0.w,c43.zyxw
0x0000e401, 0x0fc00002,   // (S01)  mov ot0,v2
0x00e4000a, 0x544610b5,   // (S01)  max r5.x,r5.xxxx,c27
0x0000e401, 0x03c20002,   // (S01)  mov opos,r2
0x00e4e408, 0x1c49202c,   // (S01)  dp4 oc2.x,c44,r2
0x00e4e408, 0x1c89202d,   // (S01)  dp4 oc2.y,c45,r2
0x00000006, 0x58420005,   // (S01)  rsq r6.x,r5.xxxx
0x0000e404, 0x51ca0064,   // (S01)  mul r4.xyz,r4,r6.xxxx
0x0000e401, 0x5201100c,   // (S01)  mov r4.w,c28
0x00550004, 0x5c4a0044,   // (S01)  mul r7.x,r4.xxxx,r4.yyyy
0x00aa5504, 0x5c8a0044,   // (S01)  mul r7.y,r4.yyyy,r4.zzzz
0x00e4e408, 0x68490049,   // (S01)  dp4 r10.x,c9,r4
0x00e4e408, 0x6889004a,   // (S01)  dp4 r10.y,c10,r4
0x00e4e408, 0x6909004b,   // (S01)  dp4 r10.z,c11,r4
0x00aaaa04, 0x5d0a0044,   // (S01)  mul r7.z,r4.zzzz,r4.zzzz
0x00aa0004, 0x5e0a0044,   // (S01)  mul r7.w,r4.xxxx,r4.zzzz
0x00e4e408, 0x6c49007c,   // (S01)  dp4 r11.x,c12,r7
0x00e4e408, 0x6c89007d,   // (S01)  dp4 r11.y,c13,r7
0x00e4e408, 0x6d09007e,   // (S01)  dp4 r11.z,c14,r7
0x00000004, 0x5c4a0044,   // (S01)  mul r7.x,r4.xxxx,r4.xxxx
0x00555504, 0x5c8a0044,   // (S01)  mul r7.y,r4.yyyy,r4.yyyy
0x00e4e402, 0x69ca00ba,   // (S01)  add r10.xyz,r10,r11
0x00e4e407, 0x6c461004,   // (S01)  dp3 r11.x,r4,c16
0x00550042, 0x5c4a0077,   // (S01)  add r7.x,r7.xxxx,-r7.yyyy
0x00e4000a, 0x6c4610db,   // (S01)  max r11.x,r11.xxxx,c29
0xe400e403, 0x69e90a7f,   // (S01)  mad r10.xyz,c15,r7.xxxx,r10
0x00e4e442, 0x5dc91034,   // (S01)  add r7.xyz,c20,-r3
0x00ff0004, 0x6c46101b,   // (S01)  mul r11.x,r11.xxxx,c17.wwww
0x00e4e407, 0x544a0077,   // (S01)  dp3 r5.x,r7,r7
0xe400e403, 0x69e91ab1,   // (S01)  mad r10.xyz,c17,r11.xxxx,r10
0x00e4e407, 0x6c461024,   // (S01)  dp3 r11.x,r4,c18
0x00e4000a, 0x544610f5,   // (S01)  max r5.x,r5.xxxx,c31
0x00e4000a, 0x6c4610eb,   // (S01)  max r11.x,r11.xxxx,c30
0x00000006, 0x58420005,   // (S01)  rsq r6.x,r5.xxxx
0x00ff0004, 0x6c46103b,   // (S01)  mul r11.x,r11.xxxx,c19.wwww
0x0000e404, 0x61ca0067,   // (S01)  mul r8.xyz,r7,r6.xxxx
0x00e4e442, 0x5dc91036,   // (S01)  add r7.xyz,c22,-r3
0xe400e403, 0x69e91ab3,   // (S01)  mad r10.xyz,c19,r11.xxxx,r10
0x00e4e407, 0x6c4a0084,   // (S01)  dp3 r11.x,r4,r8
0x00000004, 0x644a0066,   // (S01)  mul r9.x,r6.xxxx,r6.xxxx
0x00e4000a, 0x6c46200b,   // (S01)  max r11.x,r11.xxxx,c32
0x00ff0004, 0x6c861045,   // (S01)  mul r11.y,r5.xxxx,c20.wwww
0x00e4e407, 0x544a0077,   // (S01)  dp3 r5.x,r7,r7
0x0055e442, 0x6c8920b1,   // (S01)  add r11.y,c33,-r11.yyyy
0x00e4000a, 0x54462035,   // (S01)  max r5.x,r5.xxxx,c35
0x00e4550a, 0x6c86202b,   // (S01)  max r11.y,r11.yyyy,c34
0x00000006, 0x58420005,   // (S01)  rsq r6.x,r5.xxxx
0x00555504, 0x6c8a00bb,   // (S01)  mul r11.y,r11.yyyy,r11.yyyy
0x0000e404, 0x61ca0067,   // (S01)  mul r8.xyz,r7,r6.xxxx
0x00550004, 0x644a00b9,   // (S01)  mul r9.x,r9.xxxx,r11.yyyy
0x00000004, 0x644a00b9,   // (S01)  mul r9.x,r9.xxxx,r11.xxxx
0x00e4e407, 0x6c4a0084,   // (S01)  dp3 r11.x,r4,r8
0x00ff0004, 0x64461059,   // (S01)  mul r9.x,r9.xxxx,c21.wwww
0x00e4000a, 0x6c46204b,   // (S01)  max r11.x,r11.xxxx,c36
0x00e4e427, 0x6c861078,   // (S01)  dp3 r11.y,-r8,c23
0xe400e403, 0x69e91a95,   // (S01)  mad r10.xyz,c21,r9.xxxx,r10
0x00000004, 0x644a0066,   // (S01)  mul r9.x,r6.xxxx,r6.xxxx
0x55005503, 0x6c96199b,   // (S01)  mad r11.y,r11.yyyy,c25.xxxx,c25.yyyy
0x00e4550a, 0x6c86205b,   // (S01)  max r11.y,r11.yyyy,c37
0x00e45509, 0x6c86206b,   // (S01)  min r11.y,r11.yyyy,c38
0x00ff0004, 0x6d061065,   // (S01)  mul r11.z,r5.xxxx,c22.wwww
0x00aae442, 0x6d0920b7,   // (S01)  add r11.z,c39,-r11.zzzz
0x00e4aa0a, 0x6d06208b,   // (S01)  max r11.z,r11.zzzz,c40
0x00aaaa04, 0x6d0a00bb,   // (S01)  mul r11.z,r11.zzzz,r11.zzzz
0x00aa0004, 0x644a00b9,   // (S01)  mul r9.x,r9.xxxx,r11.zzzz
0x00550004, 0x644a00b9,   // (S01)  mul r9.x,r9.xxxx,r11.yyyy
0x00000004, 0x644a00b9,   // (S01)  mul r9.x,r9.xxxx,r11.xxxx
0x00ff0004, 0x64461089,   // (S01)  mul r9.x,r9.xxxx,c24.wwww
0xe400e403, 0x69e91a98,   // (S01)  mad r10.xyz,c24,r9.xxxx,r10
0x00e4e402, 0x69c2001a,   // (S01)  add r10.xyz,r10,v1
0x00e4e402, 0x69c610aa,   // (S01)  add r10.xyz,r10,c26
0x00e4e40a, 0x69c6209a,   // (S01)  max r10.xyz,r10,c41
0x00e4e409, 0x69c620aa,   // (S01)  min r10.xyz,r10,c42
0x0000c601, 0x05c2000a,   // (S01)  mov od0.xyz,r10.zyxw
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
0x00000042,// Num Bindings : 66
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
0x40e00000, // x : 7
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x0000003d, // Binding type : 61
0x0000000a, // Constant number : 10
0x41000000, // x : 8
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x0000003d, // Binding type : 61
0x0000000b, // Constant number : 11
0x41100000, // x : 9
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x0000003d, // Binding type : 61
0x0000000c, // Constant number : 12
0x41200000, // x : 10
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x0000003d, // Binding type : 61
0x0000000d, // Constant number : 13
0x41300000, // x : 11
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x0000003d, // Binding type : 61
0x0000000e, // Constant number : 14
0x41400000, // x : 12
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x0000003d, // Binding type : 61
0x0000000f, // Constant number : 15
0x41500000, // x : 13
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x0000003e, // Binding type : 62
0x00000010, // Constant number : 16
0x00000000, // x : 0
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x0000003e, // Binding type : 62
0x00000011, // Constant number : 17
0x3f800000, // x : 1
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x0000003e, // Binding type : 62
0x00000012, // Constant number : 18
0x40000000, // x : 2
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x0000003e, // Binding type : 62
0x00000013, // Constant number : 19
0x40400000, // x : 3
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x0000003e, // Binding type : 62
0x00000014, // Constant number : 20
0x40800000, // x : 4
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x0000003e, // Binding type : 62
0x00000015, // Constant number : 21
0x40a00000, // x : 5
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x0000003e, // Binding type : 62
0x00000016, // Constant number : 22
0x40c00000, // x : 6
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x0000003e, // Binding type : 62
0x00000017, // Constant number : 23
0x40e00000, // x : 7
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x0000003e, // Binding type : 62
0x00000018, // Constant number : 24
0x41000000, // x : 8
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x0000003e, // Binding type : 62
0x00000019, // Constant number : 25
0x41100000, // x : 9
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x0000003e, // Binding type : 62
0x0000001a, // Constant number : 26
0x41200000, // x : 10
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x0000003f, // Binding type : 63
0x0000001b, // Constant number : 27
0x38d1b717, // x : 0.0001
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
0x0000003f, // Binding type : 63
0x0000001d, // Constant number : 29
0x00000000, // x : 0
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x0000003f, // Binding type : 63
0x0000001e, // Constant number : 30
0x00000000, // x : 0
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x0000003f, // Binding type : 63
0x0000001f, // Constant number : 31
0x38d1b717, // x : 0.0001
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x0000003f, // Binding type : 63
0x00000020, // Constant number : 32
0x00000000, // x : 0
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x0000003f, // Binding type : 63
0x00000021, // Constant number : 33
0x3f800000, // x : 1
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x0000003f, // Binding type : 63
0x00000022, // Constant number : 34
0x00000000, // x : 0
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x0000003f, // Binding type : 63
0x00000023, // Constant number : 35
0x38d1b717, // x : 0.0001
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x0000003f, // Binding type : 63
0x00000024, // Constant number : 36
0x00000000, // x : 0
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x0000003f, // Binding type : 63
0x00000025, // Constant number : 37
0x00000000, // x : 0
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x0000003f, // Binding type : 63
0x00000026, // Constant number : 38
0x3f800000, // x : 1
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x0000003f, // Binding type : 63
0x00000027, // Constant number : 39
0x3f800000, // x : 1
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x0000003f, // Binding type : 63
0x00000028, // Constant number : 40
0x00000000, // x : 0
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x0000003f, // Binding type : 63
0x00000029, // Constant number : 41
0x00000000, // x : 0
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x0000003f, // Binding type : 63
0x0000002a, // Constant number : 42
0x3f800000, // x : 1
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x0000003f, // Binding type : 63
0x0000002b, // Constant number : 43
0x3f800000, // x : 1
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x00000021, // Binding type : 33
0x0000002c, // Constant number : 44
0x00000000, // x : 0
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x00000022, // Binding type : 34
0x0000002d, // Constant number : 45
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

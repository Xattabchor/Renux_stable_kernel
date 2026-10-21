/*************************************** 
  Source File	: /tmp/vgp-bones-hh3olrol/BoneSkinning_05
  Compiler		: version 1.1
  Date			: Wed Oct 21 17:23:31 2026
 
  Optimized code
****************************************/

const unsigned int vgp_BoneSkinning_05[] = {

0x20504756, // magic vgp id
0xffff0101, // vgp version
0x00000040, // code size (in instructions)
0x00000300, // clipping planes

0x00000200, // Section 0
0x0f003f03, // Section 1
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

0x0000e401, 0x53c11005,   // (S01)  mov r4,c21
0x00e40004, 0x54441064,   // (S01)  mul r5.x,v4.xxxx,c22
0x0000e401, 0x4fc11004,   // (S01)  mov r3,c20
0x0000e401, 0x43c12000,   // (S01)  mov r0,c32
0x0000c601, 0x06012001,   // (S01)  mov od0.w,c33.zyxw
0x0000e401, 0x0fc00002,   // (S01)  mov ot0,v2
0x00000012, 0x03c20005,   // (S01)  amv a0,r5.xxxx
0x00e4e407, 0x58418035,   // (S01)  dp3 r6.x,c[a0.x+5],v3
0x00e4e407, 0x58818036,   // (S01)  dp3 r6.y,c[a0.x+6],v3
0x00e4e407, 0x59018037,   // (S01)  dp3 r6.z,c[a0.x+7],v3
0x00e4e408, 0x54418005,   // (S01)  dp4 r5.x,c[a0.x+5],v0
0x00e4e408, 0x54818006,   // (S01)  dp4 r5.y,c[a0.x+6],v0
0x00e4e408, 0x55018007,   // (S01)  dp4 r5.z,c[a0.x+7],v0
0xe400e403, 0x51e20456,   // (S01)  mad r4.xyz,r6,v5.xxxx,r4
0xe400e403, 0x4de20355,   // (S01)  mad r3.xyz,r5,v5.xxxx,r3
0xffe40003, 0x4e241375,   // (S01)  mad r3.w,v5.xxxx,c23,r3.wwww
0x00e45504, 0x54441084,   // (S01)  mul r5.x,v4.yyyy,c24
0x00000012, 0x03c20005,   // (S01)  amv a0,r5.xxxx
0x00e4e407, 0x58418035,   // (S01)  dp3 r6.x,c[a0.x+5],v3
0x00e4e407, 0x58818036,   // (S01)  dp3 r6.y,c[a0.x+6],v3
0x00e4e407, 0x59018037,   // (S01)  dp3 r6.z,c[a0.x+7],v3
0x00e4e408, 0x54418005,   // (S01)  dp4 r5.x,c[a0.x+5],v0
0x00e4e408, 0x54818006,   // (S01)  dp4 r5.y,c[a0.x+6],v0
0x00e4e408, 0x55018007,   // (S01)  dp4 r5.z,c[a0.x+7],v0
0xe455e403, 0x51e20456,   // (S01)  mad r4.xyz,r6,v5.yyyy,r4
0xe455e403, 0x4de20355,   // (S01)  mad r3.xyz,r5,v5.yyyy,r3
0xffe45503, 0x4e241395,   // (S01)  mad r3.w,v5.yyyy,c25,r3.wwww
0x00e4aa04, 0x544410a4,   // (S01)  mul r5.x,v4.zzzz,c26
0x00000012, 0x03c20005,   // (S01)  amv a0,r5.xxxx
0x00e4e407, 0x58418035,   // (S01)  dp3 r6.x,c[a0.x+5],v3
0x00e4e408, 0x54418005,   // (S01)  dp4 r5.x,c[a0.x+5],v0
0x00e4e407, 0x58818036,   // (S01)  dp3 r6.y,c[a0.x+6],v3
0x00e4e408, 0x54818006,   // (S01)  dp4 r5.y,c[a0.x+6],v0
0x00e4e407, 0x59018037,   // (S01)  dp3 r6.z,c[a0.x+7],v3
0x00e4e408, 0x55018007,   // (S01)  dp4 r5.z,c[a0.x+7],v0
0xe4aae403, 0x51e20456,   // (S01)  mad r4.xyz,r6,v5.zzzz,r4
0xe4aae403, 0x4de20355,   // (S01)  mad r3.xyz,r5,v5.zzzz,r3
0xffe4aa03, 0x4e2413b5,   // (S01)  mad r3.w,v5.zzzz,c27,r3.wwww
0x00e4ff04, 0x544410c4,   // (S01)  mul r5.x,v4.wwww,c28
0x00000012, 0x03c20005,   // (S01)  amv a0,r5.xxxx
0x00e4e408, 0x54418005,   // (S01)  dp4 r5.x,c[a0.x+5],v0
0x00e4e408, 0x54818006,   // (S01)  dp4 r5.y,c[a0.x+6],v0
0x00e4e407, 0x58418035,   // (S01)  dp3 r6.x,c[a0.x+5],v3
0x00e4e408, 0x55018007,   // (S01)  dp4 r5.z,c[a0.x+7],v0
0x00e4e407, 0x58818036,   // (S01)  dp3 r6.y,c[a0.x+6],v3
0x00e4e407, 0x59018037,   // (S01)  dp3 r6.z,c[a0.x+7],v3
0xe4ffe403, 0x4de20355,   // (S01)  mad r3.xyz,r5,v5.wwww,r3
0xffe4ff03, 0x4e2413d5,   // (S01)  mad r3.w,v5.wwww,c29,r3.wwww
0xe4ffe403, 0x51e20456,   // (S01)  mad r4.xyz,r6,v5.wwww,r4
0x00e4e408, 0x48490031,   // (S01)  dp4 r2.x,c1,r3
0x00e4e408, 0x48890032,   // (S01)  dp4 r2.y,c2,r3
0x00e4e407, 0x544a0044,   // (S01)  dp3 r5.x,r4,r4
0x00e4e408, 0x49090033,   // (S01)  dp4 r2.z,c3,r3
0x00e4e408, 0x4a090034,   // (S01)  dp4 r2.w,c4,r3
0x00e4000a, 0x544610e5,   // (S01)  max r5.x,r5.xxxx,c30
0x0000e401, 0x03c20002,   // (S01)  mov opos,r2
0x00e4e408, 0x1c492022,   // (S01)  dp4 oc2.x,c34,r2
0x00e4e408, 0x1c892023,   // (S01)  dp4 oc2.y,c35,r2
0x00000006, 0x54420005,   // (S01)  rsq r5.x,r5.xxxx
0x0000e404, 0x51ca0054,   // (S01)  mul r4.xyz,r4,r5.xxxx
0xc6c6c603, 0x05e610f4,   // (S01)  mad od0.xyz,r4.zyxw,c31.zyxw,r0.zyxw
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
0x00000036,// Num Bindings : 54
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
0x00000003, // Section Flags: 3
0x00000004, // Binding type : 4
0x00000003, // Constant number : 3
0x00000000, // x : 0
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x00000002, // Binding type : 2
0x00000004, // Constant number : 4
0x3f800000, // x : 1
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x00000002, // Binding type : 2
0x00000005, // Constant number : 5
0x40000000, // x : 2
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
0x0000003e, // Binding type : 62
0x00000005, // Constant number : 5
0x00000000, // x : 0
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x0000003e, // Binding type : 62
0x00000006, // Constant number : 6
0x3f800000, // x : 1
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x0000003e, // Binding type : 62
0x00000007, // Constant number : 7
0x40000000, // x : 2
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x0000003e, // Binding type : 62
0x00000008, // Constant number : 8
0x40400000, // x : 3
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x0000003e, // Binding type : 62
0x00000009, // Constant number : 9
0x40800000, // x : 4
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x0000003e, // Binding type : 62
0x0000000a, // Constant number : 10
0x40a00000, // x : 5
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x0000003e, // Binding type : 62
0x0000000b, // Constant number : 11
0x40c00000, // x : 6
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x0000003e, // Binding type : 62
0x0000000c, // Constant number : 12
0x40e00000, // x : 7
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x0000003e, // Binding type : 62
0x0000000d, // Constant number : 13
0x41000000, // x : 8
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x0000003e, // Binding type : 62
0x0000000e, // Constant number : 14
0x41100000, // x : 9
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x0000003e, // Binding type : 62
0x0000000f, // Constant number : 15
0x41200000, // x : 10
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x0000003e, // Binding type : 62
0x00000010, // Constant number : 16
0x41300000, // x : 11
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x0000003e, // Binding type : 62
0x00000011, // Constant number : 17
0x41400000, // x : 12
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x0000003e, // Binding type : 62
0x00000012, // Constant number : 18
0x41500000, // x : 13
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x0000003e, // Binding type : 62
0x00000013, // Constant number : 19
0x41600000, // x : 14
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
0x00000000, // x : 0
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x0000003f, // Binding type : 63
0x00000016, // Constant number : 22
0x40400000, // x : 3
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
0x40400000, // x : 3
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
0x40400000, // x : 3
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
0x40400000, // x : 3
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x0000003f, // Binding type : 63
0x0000001d, // Constant number : 29
0x3f800000, // x : 1
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x0000003f, // Binding type : 63
0x0000001e, // Constant number : 30
0x38d1b717, // x : 0.0001
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x0000003f, // Binding type : 63
0x0000001f, // Constant number : 31
0x3f000000, // x : 0.5
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x0000003f, // Binding type : 63
0x00000020, // Constant number : 32
0x3f000000, // x : 0.5
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
0x00000021, // Binding type : 33
0x00000022, // Constant number : 34
0x00000000, // x : 0
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x00000022, // Binding type : 34
0x00000023, // Constant number : 35
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
0x00000041, // Binding type : 65
0x00000000, // Constant number : 0
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

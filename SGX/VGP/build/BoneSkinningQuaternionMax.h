/*************************************** 
  Source File	: /tmp/vgp-quaternion-bones-8b_dx6r6/BoneSkinningQuaternion_16
  Compiler		: version 1.1
  Date			: Wed Oct 21 17:33:31 2026
 
  Optimized code
****************************************/

const unsigned int vgp_BoneSkinningQuaternion_16[] = {

0x20504756, // magic vgp id
0xffff0101, // vgp version
0x00000198, // code size (in instructions)
0x00000300, // clipping planes

0x00000200, // Section 0
0x0f009703, // Section 1
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

0x00e40004, 0x5c442074,   // (S01)  mul r7.x,v4.xxxx,c39
0x0000e401, 0x53c12006,   // (S01)  mov r4,c38
0x0000e401, 0x4fc12005,   // (S01)  mov r3,c37
0x0000e401, 0x43c1400d,   // (S01)  mov r0,c77
0x0000c601, 0x0601400e,   // (S01)  mov od0.w,c78.zyxw
0x00000012, 0x03c20007,   // (S01)  amv a0,r7.xxxx
0x00e45504, 0x5c443004,   // (S01)  mul r7.x,v4.yyyy,c48
0x0000e401, 0x0fc00002,   // (S01)  mov ot0,v2
0x00aa5504, 0x58418005,   // (S01)  mul r6.x,c[a0.x+5].yyyy,v0.zzzz
0x00000000, 0x00000000,   // (S01)  nop 
0x00000000, 0x00000000,   // (S01)  nop 
0x00000000, 0x00000000,   // (S01)  nop 
0x0055aa23, 0x58618605,   // (S01)  mad r6.x,-c[a0.x+5].zzzz,v0.yyyy,r6.xxxx
0x0000aa04, 0x58818005,   // (S01)  mul r6.y,c[a0.x+5].zzzz,v0.xxxx
0x00000000, 0x00000000,   // (S01)  nop 
0x00000000, 0x00000000,   // (S01)  nop 
0x00000000, 0x00000000,   // (S01)  nop 
0x55aa0023, 0x58a18605,   // (S01)  mad r6.y,-c[a0.x+5].xxxx,v0.zzzz,r6.yyyy
0x00550004, 0x59018005,   // (S01)  mul r6.z,c[a0.x+5].xxxx,v0.yyyy
0x00000000, 0x00000000,   // (S01)  nop 
0x00000000, 0x00000000,   // (S01)  nop 
0x00000000, 0x00000000,   // (S01)  nop 
0xaa005523, 0x59218605,   // (S01)  mad r6.z,-c[a0.x+5].yyyy,v0.xxxx,r6.zzzz
0x00aa5504, 0x54498065,   // (S01)  mul r5.x,c[a0.x+5].yyyy,r6.zzzz
0x00000000, 0x00000000,   // (S01)  nop 
0x00000000, 0x00000000,   // (S01)  nop 
0x00000000, 0x00000000,   // (S01)  nop 
0x0055aa23, 0x54698565,   // (S01)  mad r5.x,-c[a0.x+5].zzzz,r6.yyyy,r5.xxxx
0x00000000, 0x00000000,   // (S01)  nop 
0x00000000, 0x00000000,   // (S01)  nop 
0x00000000, 0x00000000,   // (S01)  nop 
0x0000ff03, 0x54698565,   // (S01)  mad r5.x,c[a0.x+5].wwww,r6.xxxx,r5.xxxx
0x00e40004, 0x54462085,   // (S01)  mul r5.x,r5.xxxx,c40
0x00000002, 0x54420005,   // (S01)  add r5.x,r5.xxxx,v0.xxxx
0x0000aa04, 0x54898065,   // (S01)  mul r5.y,c[a0.x+5].zzzz,r6.xxxx
0x00000000, 0x00000000,   // (S01)  nop 
0x00000000, 0x00000000,   // (S01)  nop 
0x00000000, 0x00000000,   // (S01)  nop 
0x55aa0023, 0x54a98565,   // (S01)  mad r5.y,-c[a0.x+5].xxxx,r6.zzzz,r5.yyyy
0x00000000, 0x00000000,   // (S01)  nop 
0x00000000, 0x00000000,   // (S01)  nop 
0x00000000, 0x00000000,   // (S01)  nop 
0x5555ff03, 0x54a98565,   // (S01)  mad r5.y,c[a0.x+5].wwww,r6.yyyy,r5.yyyy
0x00e45504, 0x54862095,   // (S01)  mul r5.y,r5.yyyy,c41
0x00555502, 0x54820005,   // (S01)  add r5.y,r5.yyyy,v0.yyyy
0x00550004, 0x55098065,   // (S01)  mul r5.z,c[a0.x+5].xxxx,r6.yyyy
0x00000000, 0x00000000,   // (S01)  nop 
0x00000000, 0x00000000,   // (S01)  nop 
0x00000000, 0x00000000,   // (S01)  nop 
0xaa005523, 0x55298565,   // (S01)  mad r5.z,-c[a0.x+5].yyyy,r6.xxxx,r5.zzzz
0x00000000, 0x00000000,   // (S01)  nop 
0x00000000, 0x00000000,   // (S01)  nop 
0x00000000, 0x00000000,   // (S01)  nop 
0xaaaaff03, 0x55298565,   // (S01)  mad r5.z,c[a0.x+5].wwww,r6.zzzz,r5.zzzz
0x00aa5504, 0x58418035,   // (S01)  mul r6.x,c[a0.x+5].yyyy,v3.zzzz
0x00e4aa04, 0x550620a5,   // (S01)  mul r5.z,r5.zzzz,c42
0x0055aa23, 0x58618635,   // (S01)  mad r6.x,-c[a0.x+5].zzzz,v3.yyyy,r6.xxxx
0x0000aa04, 0x58818035,   // (S01)  mul r6.y,c[a0.x+5].zzzz,v3.xxxx
0x00aaaa02, 0x55020005,   // (S01)  add r5.z,r5.zzzz,v0.zzzz
0x00000000, 0x00000000,   // (S01)  nop 
0x00000000, 0x00000000,   // (S01)  nop 
0x00000000, 0x00000000,   // (S01)  nop 
0x55aa0023, 0x58a18635,   // (S01)  mad r6.y,-c[a0.x+5].xxxx,v3.zzzz,r6.yyyy
0x00550004, 0x59018035,   // (S01)  mul r6.z,c[a0.x+5].xxxx,v3.yyyy
0x00e4e402, 0x55c68065,   // (S01)  add r5.xyz,r5,c[a0.x+6]
0x0000e401, 0x5601200b,   // (S01)  mov r5.w,c43
0x00000000, 0x00000000,   // (S01)  nop 
0x00000000, 0x00000000,   // (S01)  nop 
0x00000000, 0x00000000,   // (S01)  nop 
0xaa005523, 0x59218635,   // (S01)  mad r6.z,-c[a0.x+5].yyyy,v3.xxxx,r6.zzzz
0xe400e403, 0x4de20355,   // (S01)  mad r3.xyz,r5,v5.xxxx,r3
0xffe40003, 0x4e2423c5,   // (S01)  mad r3.w,v5.xxxx,c44,r3.wwww
0x00aa5504, 0x54498065,   // (S01)  mul r5.x,c[a0.x+5].yyyy,r6.zzzz
0x00000000, 0x00000000,   // (S01)  nop 
0x00000000, 0x00000000,   // (S01)  nop 
0x00000000, 0x00000000,   // (S01)  nop 
0x0055aa23, 0x54698565,   // (S01)  mad r5.x,-c[a0.x+5].zzzz,r6.yyyy,r5.xxxx
0x00000000, 0x00000000,   // (S01)  nop 
0x00000000, 0x00000000,   // (S01)  nop 
0x00000000, 0x00000000,   // (S01)  nop 
0x0000ff03, 0x54698565,   // (S01)  mad r5.x,c[a0.x+5].wwww,r6.xxxx,r5.xxxx
0x00e40004, 0x544620d5,   // (S01)  mul r5.x,r5.xxxx,c45
0x00000002, 0x54420035,   // (S01)  add r5.x,r5.xxxx,v3.xxxx
0x0000aa04, 0x54898065,   // (S01)  mul r5.y,c[a0.x+5].zzzz,r6.xxxx
0x00000000, 0x00000000,   // (S01)  nop 
0x00000000, 0x00000000,   // (S01)  nop 
0x00000000, 0x00000000,   // (S01)  nop 
0x55aa0023, 0x54a98565,   // (S01)  mad r5.y,-c[a0.x+5].xxxx,r6.zzzz,r5.yyyy
0x00000000, 0x00000000,   // (S01)  nop 
0x00000000, 0x00000000,   // (S01)  nop 
0x00000000, 0x00000000,   // (S01)  nop 
0x5555ff03, 0x54a98565,   // (S01)  mad r5.y,c[a0.x+5].wwww,r6.yyyy,r5.yyyy
0x00e45504, 0x548620e5,   // (S01)  mul r5.y,r5.yyyy,c46
0x00555502, 0x54820035,   // (S01)  add r5.y,r5.yyyy,v3.yyyy
0x00550004, 0x55098065,   // (S01)  mul r5.z,c[a0.x+5].xxxx,r6.yyyy
0x00000000, 0x00000000,   // (S01)  nop 
0x00000000, 0x00000000,   // (S01)  nop 
0x00000000, 0x00000000,   // (S01)  nop 
0xaa005523, 0x55298565,   // (S01)  mad r5.z,-c[a0.x+5].yyyy,r6.xxxx,r5.zzzz
0x00000000, 0x00000000,   // (S01)  nop 
0x00000000, 0x00000000,   // (S01)  nop 
0x00000000, 0x00000000,   // (S01)  nop 
0xaaaaff03, 0x55298565,   // (S01)  mad r5.z,c[a0.x+5].wwww,r6.zzzz,r5.zzzz
0x00000012, 0x03c20007,   // (S01)  amv a0,r7.xxxx
0x00e4aa04, 0x5c443094,   // (S01)  mul r7.x,v4.zzzz,c57
0x00e4aa04, 0x550620f5,   // (S01)  mul r5.z,r5.zzzz,c47
0x00aa5504, 0x58418005,   // (S01)  mul r6.x,c[a0.x+5].yyyy,v0.zzzz
0x00aaaa02, 0x55020035,   // (S01)  add r5.z,r5.zzzz,v3.zzzz
0x0055aa23, 0x58618605,   // (S01)  mad r6.x,-c[a0.x+5].zzzz,v0.yyyy,r6.xxxx
0x0000aa04, 0x58818005,   // (S01)  mul r6.y,c[a0.x+5].zzzz,v0.xxxx
0xe400e403, 0x51e20455,   // (S01)  mad r4.xyz,r5,v5.xxxx,r4
0x00000000, 0x00000000,   // (S01)  nop 
0x00000000, 0x00000000,   // (S01)  nop 
0x00000000, 0x00000000,   // (S01)  nop 
0x55aa0023, 0x58a18605,   // (S01)  mad r6.y,-c[a0.x+5].xxxx,v0.zzzz,r6.yyyy
0x00550004, 0x59018005,   // (S01)  mul r6.z,c[a0.x+5].xxxx,v0.yyyy
0x00000000, 0x00000000,   // (S01)  nop 
0x00000000, 0x00000000,   // (S01)  nop 
0x00000000, 0x00000000,   // (S01)  nop 
0xaa005523, 0x59218605,   // (S01)  mad r6.z,-c[a0.x+5].yyyy,v0.xxxx,r6.zzzz
0x00aa5504, 0x54498065,   // (S01)  mul r5.x,c[a0.x+5].yyyy,r6.zzzz
0x00000000, 0x00000000,   // (S01)  nop 
0x00000000, 0x00000000,   // (S01)  nop 
0x00000000, 0x00000000,   // (S01)  nop 
0x0055aa23, 0x54698565,   // (S01)  mad r5.x,-c[a0.x+5].zzzz,r6.yyyy,r5.xxxx
0x00000000, 0x00000000,   // (S01)  nop 
0x00000000, 0x00000000,   // (S01)  nop 
0x00000000, 0x00000000,   // (S01)  nop 
0x0000ff03, 0x54698565,   // (S01)  mad r5.x,c[a0.x+5].wwww,r6.xxxx,r5.xxxx
0x00e40004, 0x54463015,   // (S01)  mul r5.x,r5.xxxx,c49
0x00000002, 0x54420005,   // (S01)  add r5.x,r5.xxxx,v0.xxxx
0x0000aa04, 0x54898065,   // (S01)  mul r5.y,c[a0.x+5].zzzz,r6.xxxx
0x00000000, 0x00000000,   // (S01)  nop 
0x00000000, 0x00000000,   // (S01)  nop 
0x00000000, 0x00000000,   // (S01)  nop 
0x55aa0023, 0x54a98565,   // (S01)  mad r5.y,-c[a0.x+5].xxxx,r6.zzzz,r5.yyyy
0x00000000, 0x00000000,   // (S01)  nop 
0x00000000, 0x00000000,   // (S01)  nop 
0x00000000, 0x00000000,   // (S01)  nop 
0x5555ff03, 0x54a98565,   // (S01)  mad r5.y,c[a0.x+5].wwww,r6.yyyy,r5.yyyy
0x00e45504, 0x54863025,   // (S01)  mul r5.y,r5.yyyy,c50
0x00555502, 0x54820005,   // (S01)  add r5.y,r5.yyyy,v0.yyyy
0x00550004, 0x55098065,   // (S01)  mul r5.z,c[a0.x+5].xxxx,r6.yyyy
0x00000000, 0x00000000,   // (S01)  nop 
0x00000000, 0x00000000,   // (S01)  nop 
0x00000000, 0x00000000,   // (S01)  nop 
0xaa005523, 0x55298565,   // (S01)  mad r5.z,-c[a0.x+5].yyyy,r6.xxxx,r5.zzzz
0x00000000, 0x00000000,   // (S01)  nop 
0x00000000, 0x00000000,   // (S01)  nop 
0x00000000, 0x00000000,   // (S01)  nop 
0xaaaaff03, 0x55298565,   // (S01)  mad r5.z,c[a0.x+5].wwww,r6.zzzz,r5.zzzz
0x00aa5504, 0x58418035,   // (S01)  mul r6.x,c[a0.x+5].yyyy,v3.zzzz
0x00e4aa04, 0x55063035,   // (S01)  mul r5.z,r5.zzzz,c51
0x0055aa23, 0x58618635,   // (S01)  mad r6.x,-c[a0.x+5].zzzz,v3.yyyy,r6.xxxx
0x0000aa04, 0x58818035,   // (S01)  mul r6.y,c[a0.x+5].zzzz,v3.xxxx
0x00aaaa02, 0x55020005,   // (S01)  add r5.z,r5.zzzz,v0.zzzz
0x00000000, 0x00000000,   // (S01)  nop 
0x00000000, 0x00000000,   // (S01)  nop 
0x00000000, 0x00000000,   // (S01)  nop 
0x55aa0023, 0x58a18635,   // (S01)  mad r6.y,-c[a0.x+5].xxxx,v3.zzzz,r6.yyyy
0x00550004, 0x59018035,   // (S01)  mul r6.z,c[a0.x+5].xxxx,v3.yyyy
0x00e4e402, 0x55c68065,   // (S01)  add r5.xyz,r5,c[a0.x+6]
0x0000e401, 0x56013004,   // (S01)  mov r5.w,c52
0x00000000, 0x00000000,   // (S01)  nop 
0x00000000, 0x00000000,   // (S01)  nop 
0x00000000, 0x00000000,   // (S01)  nop 
0xaa005523, 0x59218635,   // (S01)  mad r6.z,-c[a0.x+5].yyyy,v3.xxxx,r6.zzzz
0xe455e403, 0x4de20355,   // (S01)  mad r3.xyz,r5,v5.yyyy,r3
0xffe45503, 0x4e243355,   // (S01)  mad r3.w,v5.yyyy,c53,r3.wwww
0x00aa5504, 0x54498065,   // (S01)  mul r5.x,c[a0.x+5].yyyy,r6.zzzz
0x00000000, 0x00000000,   // (S01)  nop 
0x00000000, 0x00000000,   // (S01)  nop 
0x00000000, 0x00000000,   // (S01)  nop 
0x0055aa23, 0x54698565,   // (S01)  mad r5.x,-c[a0.x+5].zzzz,r6.yyyy,r5.xxxx
0x00000000, 0x00000000,   // (S01)  nop 
0x00000000, 0x00000000,   // (S01)  nop 
0x00000000, 0x00000000,   // (S01)  nop 
0x0000ff03, 0x54698565,   // (S01)  mad r5.x,c[a0.x+5].wwww,r6.xxxx,r5.xxxx
0x00e40004, 0x54463065,   // (S01)  mul r5.x,r5.xxxx,c54
0x00000002, 0x54420035,   // (S01)  add r5.x,r5.xxxx,v3.xxxx
0x0000aa04, 0x54898065,   // (S01)  mul r5.y,c[a0.x+5].zzzz,r6.xxxx
0x00000000, 0x00000000,   // (S01)  nop 
0x00000000, 0x00000000,   // (S01)  nop 
0x00000000, 0x00000000,   // (S01)  nop 
0x55aa0023, 0x54a98565,   // (S01)  mad r5.y,-c[a0.x+5].xxxx,r6.zzzz,r5.yyyy
0x00000000, 0x00000000,   // (S01)  nop 
0x00000000, 0x00000000,   // (S01)  nop 
0x00000000, 0x00000000,   // (S01)  nop 
0x5555ff03, 0x54a98565,   // (S01)  mad r5.y,c[a0.x+5].wwww,r6.yyyy,r5.yyyy
0x00e45504, 0x54863075,   // (S01)  mul r5.y,r5.yyyy,c55
0x00555502, 0x54820035,   // (S01)  add r5.y,r5.yyyy,v3.yyyy
0x00550004, 0x55098065,   // (S01)  mul r5.z,c[a0.x+5].xxxx,r6.yyyy
0x00000000, 0x00000000,   // (S01)  nop 
0x00000000, 0x00000000,   // (S01)  nop 
0x00000000, 0x00000000,   // (S01)  nop 
0xaa005523, 0x55298565,   // (S01)  mad r5.z,-c[a0.x+5].yyyy,r6.xxxx,r5.zzzz
0x00000000, 0x00000000,   // (S01)  nop 
0x00000000, 0x00000000,   // (S01)  nop 
0x00000000, 0x00000000,   // (S01)  nop 
0xaaaaff03, 0x55298565,   // (S01)  mad r5.z,c[a0.x+5].wwww,r6.zzzz,r5.zzzz
0x00000012, 0x03c20007,   // (S01)  amv a0,r7.xxxx
0x00e4ff04, 0x5c444024,   // (S01)  mul r7.x,v4.wwww,c66
0x00e4aa04, 0x55063085,   // (S01)  mul r5.z,r5.zzzz,c56
0x00aa5504, 0x58418005,   // (S01)  mul r6.x,c[a0.x+5].yyyy,v0.zzzz
0x00aaaa02, 0x55020035,   // (S01)  add r5.z,r5.zzzz,v3.zzzz
0x0055aa23, 0x58618605,   // (S01)  mad r6.x,-c[a0.x+5].zzzz,v0.yyyy,r6.xxxx
0x0000aa04, 0x58818005,   // (S01)  mul r6.y,c[a0.x+5].zzzz,v0.xxxx
0xe455e403, 0x51e20455,   // (S01)  mad r4.xyz,r5,v5.yyyy,r4
0x00000000, 0x00000000,   // (S01)  nop 
0x00000000, 0x00000000,   // (S01)  nop 
0x00000000, 0x00000000,   // (S01)  nop 
0x55aa0023, 0x58a18605,   // (S01)  mad r6.y,-c[a0.x+5].xxxx,v0.zzzz,r6.yyyy
0x00550004, 0x59018005,   // (S01)  mul r6.z,c[a0.x+5].xxxx,v0.yyyy
0x00000000, 0x00000000,   // (S01)  nop 
0x00000000, 0x00000000,   // (S01)  nop 
0x00000000, 0x00000000,   // (S01)  nop 
0xaa005523, 0x59218605,   // (S01)  mad r6.z,-c[a0.x+5].yyyy,v0.xxxx,r6.zzzz
0x00aa5504, 0x54498065,   // (S01)  mul r5.x,c[a0.x+5].yyyy,r6.zzzz
0x00000000, 0x00000000,   // (S01)  nop 
0x00000000, 0x00000000,   // (S01)  nop 
0x00000000, 0x00000000,   // (S01)  nop 
0x0055aa23, 0x54698565,   // (S01)  mad r5.x,-c[a0.x+5].zzzz,r6.yyyy,r5.xxxx
0x00000000, 0x00000000,   // (S01)  nop 
0x00000000, 0x00000000,   // (S01)  nop 
0x00000000, 0x00000000,   // (S01)  nop 
0x0000ff03, 0x54698565,   // (S01)  mad r5.x,c[a0.x+5].wwww,r6.xxxx,r5.xxxx
0x00e40004, 0x544630a5,   // (S01)  mul r5.x,r5.xxxx,c58
0x00000002, 0x54420005,   // (S01)  add r5.x,r5.xxxx,v0.xxxx
0x0000aa04, 0x54898065,   // (S01)  mul r5.y,c[a0.x+5].zzzz,r6.xxxx
0x00000000, 0x00000000,   // (S01)  nop 
0x00000000, 0x00000000,   // (S01)  nop 
0x00000000, 0x00000000,   // (S01)  nop 
0x55aa0023, 0x54a98565,   // (S01)  mad r5.y,-c[a0.x+5].xxxx,r6.zzzz,r5.yyyy
0x00000000, 0x00000000,   // (S01)  nop 
0x00000000, 0x00000000,   // (S01)  nop 
0x00000000, 0x00000000,   // (S01)  nop 
0x5555ff03, 0x54a98565,   // (S01)  mad r5.y,c[a0.x+5].wwww,r6.yyyy,r5.yyyy
0x00e45504, 0x548630b5,   // (S01)  mul r5.y,r5.yyyy,c59
0x00555502, 0x54820005,   // (S01)  add r5.y,r5.yyyy,v0.yyyy
0x00550004, 0x55098065,   // (S01)  mul r5.z,c[a0.x+5].xxxx,r6.yyyy
0x00000000, 0x00000000,   // (S01)  nop 
0x00000000, 0x00000000,   // (S01)  nop 
0x00000000, 0x00000000,   // (S01)  nop 
0xaa005523, 0x55298565,   // (S01)  mad r5.z,-c[a0.x+5].yyyy,r6.xxxx,r5.zzzz
0x00000000, 0x00000000,   // (S01)  nop 
0x00000000, 0x00000000,   // (S01)  nop 
0x00000000, 0x00000000,   // (S01)  nop 
0xaaaaff03, 0x55298565,   // (S01)  mad r5.z,c[a0.x+5].wwww,r6.zzzz,r5.zzzz
0x00aa5504, 0x58418035,   // (S01)  mul r6.x,c[a0.x+5].yyyy,v3.zzzz
0x00e4aa04, 0x550630c5,   // (S01)  mul r5.z,r5.zzzz,c60
0x0055aa23, 0x58618635,   // (S01)  mad r6.x,-c[a0.x+5].zzzz,v3.yyyy,r6.xxxx
0x0000aa04, 0x58818035,   // (S01)  mul r6.y,c[a0.x+5].zzzz,v3.xxxx
0x00aaaa02, 0x55020005,   // (S01)  add r5.z,r5.zzzz,v0.zzzz
0x00000000, 0x00000000,   // (S01)  nop 
0x00000000, 0x00000000,   // (S01)  nop 
0x00000000, 0x00000000,   // (S01)  nop 
0x55aa0023, 0x58a18635,   // (S01)  mad r6.y,-c[a0.x+5].xxxx,v3.zzzz,r6.yyyy
0x00550004, 0x59018035,   // (S01)  mul r6.z,c[a0.x+5].xxxx,v3.yyyy
0x00e4e402, 0x55c68065,   // (S01)  add r5.xyz,r5,c[a0.x+6]
0x0000e401, 0x5601300d,   // (S01)  mov r5.w,c61
0x00000000, 0x00000000,   // (S01)  nop 
0x00000000, 0x00000000,   // (S01)  nop 
0x00000000, 0x00000000,   // (S01)  nop 
0xaa005523, 0x59218635,   // (S01)  mad r6.z,-c[a0.x+5].yyyy,v3.xxxx,r6.zzzz
0xe4aae403, 0x4de20355,   // (S01)  mad r3.xyz,r5,v5.zzzz,r3
0xffe4aa03, 0x4e2433e5,   // (S01)  mad r3.w,v5.zzzz,c62,r3.wwww
0x00aa5504, 0x54498065,   // (S01)  mul r5.x,c[a0.x+5].yyyy,r6.zzzz
0x00000000, 0x00000000,   // (S01)  nop 
0x00000000, 0x00000000,   // (S01)  nop 
0x00000000, 0x00000000,   // (S01)  nop 
0x0055aa23, 0x54698565,   // (S01)  mad r5.x,-c[a0.x+5].zzzz,r6.yyyy,r5.xxxx
0x00000000, 0x00000000,   // (S01)  nop 
0x00000000, 0x00000000,   // (S01)  nop 
0x00000000, 0x00000000,   // (S01)  nop 
0x0000ff03, 0x54698565,   // (S01)  mad r5.x,c[a0.x+5].wwww,r6.xxxx,r5.xxxx
0x00e40004, 0x544630f5,   // (S01)  mul r5.x,r5.xxxx,c63
0x00000002, 0x54420035,   // (S01)  add r5.x,r5.xxxx,v3.xxxx
0x0000aa04, 0x54898065,   // (S01)  mul r5.y,c[a0.x+5].zzzz,r6.xxxx
0x00000000, 0x00000000,   // (S01)  nop 
0x00000000, 0x00000000,   // (S01)  nop 
0x00000000, 0x00000000,   // (S01)  nop 
0x55aa0023, 0x54a98565,   // (S01)  mad r5.y,-c[a0.x+5].xxxx,r6.zzzz,r5.yyyy
0x00000000, 0x00000000,   // (S01)  nop 
0x00000000, 0x00000000,   // (S01)  nop 
0x00000000, 0x00000000,   // (S01)  nop 
0x5555ff03, 0x54a98565,   // (S01)  mad r5.y,c[a0.x+5].wwww,r6.yyyy,r5.yyyy
0x00e45504, 0x54864005,   // (S01)  mul r5.y,r5.yyyy,c64
0x00555502, 0x54820035,   // (S01)  add r5.y,r5.yyyy,v3.yyyy
0x00550004, 0x55098065,   // (S01)  mul r5.z,c[a0.x+5].xxxx,r6.yyyy
0x00000000, 0x00000000,   // (S01)  nop 
0x00000000, 0x00000000,   // (S01)  nop 
0x00000000, 0x00000000,   // (S01)  nop 
0xaa005523, 0x55298565,   // (S01)  mad r5.z,-c[a0.x+5].yyyy,r6.xxxx,r5.zzzz
0x00000000, 0x00000000,   // (S01)  nop 
0x00000000, 0x00000000,   // (S01)  nop 
0x00000000, 0x00000000,   // (S01)  nop 
0xaaaaff03, 0x55298565,   // (S01)  mad r5.z,c[a0.x+5].wwww,r6.zzzz,r5.zzzz
0x00000012, 0x03c20007,   // (S01)  amv a0,r7.xxxx
0x00e4aa04, 0x55064015,   // (S01)  mul r5.z,r5.zzzz,c65
0x00aa5504, 0x58418005,   // (S01)  mul r6.x,c[a0.x+5].yyyy,v0.zzzz
0x00aaaa02, 0x55020035,   // (S01)  add r5.z,r5.zzzz,v3.zzzz
0x0055aa23, 0x58618605,   // (S01)  mad r6.x,-c[a0.x+5].zzzz,v0.yyyy,r6.xxxx
0x0000aa04, 0x58818005,   // (S01)  mul r6.y,c[a0.x+5].zzzz,v0.xxxx
0xe4aae403, 0x51e20455,   // (S01)  mad r4.xyz,r5,v5.zzzz,r4
0x00000000, 0x00000000,   // (S01)  nop 
0x00000000, 0x00000000,   // (S01)  nop 
0x00000000, 0x00000000,   // (S01)  nop 
0x55aa0023, 0x58a18605,   // (S01)  mad r6.y,-c[a0.x+5].xxxx,v0.zzzz,r6.yyyy
0x00550004, 0x59018005,   // (S01)  mul r6.z,c[a0.x+5].xxxx,v0.yyyy
0x00000000, 0x00000000,   // (S01)  nop 
0x00000000, 0x00000000,   // (S01)  nop 
0x00000000, 0x00000000,   // (S01)  nop 
0xaa005523, 0x59218605,   // (S01)  mad r6.z,-c[a0.x+5].yyyy,v0.xxxx,r6.zzzz
0x00aa5504, 0x54498065,   // (S01)  mul r5.x,c[a0.x+5].yyyy,r6.zzzz
0x00000000, 0x00000000,   // (S01)  nop 
0x00000000, 0x00000000,   // (S01)  nop 
0x00000000, 0x00000000,   // (S01)  nop 
0x0055aa23, 0x54698565,   // (S01)  mad r5.x,-c[a0.x+5].zzzz,r6.yyyy,r5.xxxx
0x00000000, 0x00000000,   // (S01)  nop 
0x00000000, 0x00000000,   // (S01)  nop 
0x00000000, 0x00000000,   // (S01)  nop 
0x0000ff03, 0x54698565,   // (S01)  mad r5.x,c[a0.x+5].wwww,r6.xxxx,r5.xxxx
0x00e40004, 0x54464035,   // (S01)  mul r5.x,r5.xxxx,c67
0x00000002, 0x54420005,   // (S01)  add r5.x,r5.xxxx,v0.xxxx
0x0000aa04, 0x54898065,   // (S01)  mul r5.y,c[a0.x+5].zzzz,r6.xxxx
0x00000000, 0x00000000,   // (S01)  nop 
0x00000000, 0x00000000,   // (S01)  nop 
0x00000000, 0x00000000,   // (S01)  nop 
0x55aa0023, 0x54a98565,   // (S01)  mad r5.y,-c[a0.x+5].xxxx,r6.zzzz,r5.yyyy
0x00000000, 0x00000000,   // (S01)  nop 
0x00000000, 0x00000000,   // (S01)  nop 
0x00000000, 0x00000000,   // (S01)  nop 
0x5555ff03, 0x54a98565,   // (S01)  mad r5.y,c[a0.x+5].wwww,r6.yyyy,r5.yyyy
0x00e45504, 0x54864045,   // (S01)  mul r5.y,r5.yyyy,c68
0x00555502, 0x54820005,   // (S01)  add r5.y,r5.yyyy,v0.yyyy
0x00550004, 0x55098065,   // (S01)  mul r5.z,c[a0.x+5].xxxx,r6.yyyy
0x00000000, 0x00000000,   // (S01)  nop 
0x00000000, 0x00000000,   // (S01)  nop 
0x00000000, 0x00000000,   // (S01)  nop 
0xaa005523, 0x55298565,   // (S01)  mad r5.z,-c[a0.x+5].yyyy,r6.xxxx,r5.zzzz
0x00000000, 0x00000000,   // (S01)  nop 
0x00000000, 0x00000000,   // (S01)  nop 
0x00000000, 0x00000000,   // (S01)  nop 
0xaaaaff03, 0x55298565,   // (S01)  mad r5.z,c[a0.x+5].wwww,r6.zzzz,r5.zzzz
0x00aa5504, 0x58418035,   // (S01)  mul r6.x,c[a0.x+5].yyyy,v3.zzzz
0x00e4aa04, 0x55064055,   // (S01)  mul r5.z,r5.zzzz,c69
0x0055aa23, 0x58618635,   // (S01)  mad r6.x,-c[a0.x+5].zzzz,v3.yyyy,r6.xxxx
0x0000aa04, 0x58818035,   // (S01)  mul r6.y,c[a0.x+5].zzzz,v3.xxxx
0x00aaaa02, 0x55020005,   // (S01)  add r5.z,r5.zzzz,v0.zzzz
0x00000000, 0x00000000,   // (S01)  nop 
0x00000000, 0x00000000,   // (S01)  nop 
0x00000000, 0x00000000,   // (S01)  nop 
0x55aa0023, 0x58a18635,   // (S01)  mad r6.y,-c[a0.x+5].xxxx,v3.zzzz,r6.yyyy
0x00550004, 0x59018035,   // (S01)  mul r6.z,c[a0.x+5].xxxx,v3.yyyy
0x00e4e402, 0x55c68065,   // (S01)  add r5.xyz,r5,c[a0.x+6]
0x0000e401, 0x56014006,   // (S01)  mov r5.w,c70
0x00000000, 0x00000000,   // (S01)  nop 
0x00000000, 0x00000000,   // (S01)  nop 
0x00000000, 0x00000000,   // (S01)  nop 
0xaa005523, 0x59218635,   // (S01)  mad r6.z,-c[a0.x+5].yyyy,v3.xxxx,r6.zzzz
0xe4ffe403, 0x4de20355,   // (S01)  mad r3.xyz,r5,v5.wwww,r3
0xffe4ff03, 0x4e244375,   // (S01)  mad r3.w,v5.wwww,c71,r3.wwww
0x00aa5504, 0x54498065,   // (S01)  mul r5.x,c[a0.x+5].yyyy,r6.zzzz
0x00e4e408, 0x48490031,   // (S01)  dp4 r2.x,c1,r3
0x00e4e408, 0x48890032,   // (S01)  dp4 r2.y,c2,r3
0x0055aa23, 0x54698565,   // (S01)  mad r5.x,-c[a0.x+5].zzzz,r6.yyyy,r5.xxxx
0x00e4e408, 0x49090033,   // (S01)  dp4 r2.z,c3,r3
0x00e4e408, 0x4a090034,   // (S01)  dp4 r2.w,c4,r3
0x00000000, 0x00000000,   // (S01)  nop 
0x00000000, 0x00000000,   // (S01)  nop 
0x00000000, 0x00000000,   // (S01)  nop 
0x0000ff03, 0x54698565,   // (S01)  mad r5.x,c[a0.x+5].wwww,r6.xxxx,r5.xxxx
0x0000e401, 0x03c20002,   // (S01)  mov opos,r2
0x00e4e408, 0x1c49402f,   // (S01)  dp4 oc2.x,c79,r2
0x00e4e408, 0x1c895020,   // (S01)  dp4 oc2.y,c80,r2
0x00e40004, 0x54464085,   // (S01)  mul r5.x,r5.xxxx,c72
0x00000002, 0x54420035,   // (S01)  add r5.x,r5.xxxx,v3.xxxx
0x0000aa04, 0x54898065,   // (S01)  mul r5.y,c[a0.x+5].zzzz,r6.xxxx
0x00000000, 0x00000000,   // (S01)  nop 
0x00000000, 0x00000000,   // (S01)  nop 
0x00000000, 0x00000000,   // (S01)  nop 
0x55aa0023, 0x54a98565,   // (S01)  mad r5.y,-c[a0.x+5].xxxx,r6.zzzz,r5.yyyy
0x00000000, 0x00000000,   // (S01)  nop 
0x00000000, 0x00000000,   // (S01)  nop 
0x00000000, 0x00000000,   // (S01)  nop 
0x5555ff03, 0x54a98565,   // (S01)  mad r5.y,c[a0.x+5].wwww,r6.yyyy,r5.yyyy
0x00e45504, 0x54864095,   // (S01)  mul r5.y,r5.yyyy,c73
0x00555502, 0x54820035,   // (S01)  add r5.y,r5.yyyy,v3.yyyy
0x00550004, 0x55098065,   // (S01)  mul r5.z,c[a0.x+5].xxxx,r6.yyyy
0x00000000, 0x00000000,   // (S01)  nop 
0x00000000, 0x00000000,   // (S01)  nop 
0x00000000, 0x00000000,   // (S01)  nop 
0xaa005523, 0x55298565,   // (S01)  mad r5.z,-c[a0.x+5].yyyy,r6.xxxx,r5.zzzz
0x00000000, 0x00000000,   // (S01)  nop 
0x00000000, 0x00000000,   // (S01)  nop 
0x00000000, 0x00000000,   // (S01)  nop 
0xaaaaff03, 0x55298565,   // (S01)  mad r5.z,c[a0.x+5].wwww,r6.zzzz,r5.zzzz
0x00e4aa04, 0x550640a5,   // (S01)  mul r5.z,r5.zzzz,c74
0x00aaaa02, 0x55020035,   // (S01)  add r5.z,r5.zzzz,v3.zzzz
0xe4ffe403, 0x51e20455,   // (S01)  mad r4.xyz,r5,v5.wwww,r4
0x00e4e407, 0x5c4a0044,   // (S01)  dp3 r7.x,r4,r4
0x00e4000a, 0x5c4640b7,   // (S01)  max r7.x,r7.xxxx,c75
0x00000006, 0x5c420007,   // (S01)  rsq r7.x,r7.xxxx
0x0000e404, 0x51ca0074,   // (S01)  mul r4.xyz,r4,r7.xxxx
0xc6c6c603, 0x05e640c4,   // (S01)  mad od0.xyz,r4.zyxw,c76.zyxw,r0.zyxw
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
0x00000064,// Num Bindings : 100
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
0x0000003e, // Binding type : 62
0x00000014, // Constant number : 20
0x41700000, // x : 15
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x0000003d, // Binding type : 61
0x00000015, // Constant number : 21
0x00000000, // x : 0
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x0000003d, // Binding type : 61
0x00000016, // Constant number : 22
0x3f800000, // x : 1
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x0000003d, // Binding type : 61
0x00000017, // Constant number : 23
0x40000000, // x : 2
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x0000003d, // Binding type : 61
0x00000018, // Constant number : 24
0x40400000, // x : 3
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x0000003d, // Binding type : 61
0x00000019, // Constant number : 25
0x40800000, // x : 4
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x0000003d, // Binding type : 61
0x0000001a, // Constant number : 26
0x40a00000, // x : 5
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x0000003d, // Binding type : 61
0x0000001b, // Constant number : 27
0x40c00000, // x : 6
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x0000003d, // Binding type : 61
0x0000001c, // Constant number : 28
0x40e00000, // x : 7
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x0000003d, // Binding type : 61
0x0000001d, // Constant number : 29
0x41000000, // x : 8
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x0000003d, // Binding type : 61
0x0000001e, // Constant number : 30
0x41100000, // x : 9
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x0000003d, // Binding type : 61
0x0000001f, // Constant number : 31
0x41200000, // x : 10
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x0000003d, // Binding type : 61
0x00000020, // Constant number : 32
0x41300000, // x : 11
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x0000003d, // Binding type : 61
0x00000021, // Constant number : 33
0x41400000, // x : 12
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x0000003d, // Binding type : 61
0x00000022, // Constant number : 34
0x41500000, // x : 13
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x0000003d, // Binding type : 61
0x00000023, // Constant number : 35
0x41600000, // x : 14
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x0000003d, // Binding type : 61
0x00000024, // Constant number : 36
0x41700000, // x : 15
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
0x00000000, // x : 0
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x0000003f, // Binding type : 63
0x00000027, // Constant number : 39
0x40000000, // x : 2
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x0000003f, // Binding type : 63
0x00000028, // Constant number : 40
0x40000000, // x : 2
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x0000003f, // Binding type : 63
0x00000029, // Constant number : 41
0x40000000, // x : 2
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x0000003f, // Binding type : 63
0x0000002a, // Constant number : 42
0x40000000, // x : 2
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
0x0000003f, // Binding type : 63
0x0000002c, // Constant number : 44
0x3f800000, // x : 1
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x0000003f, // Binding type : 63
0x0000002d, // Constant number : 45
0x40000000, // x : 2
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x0000003f, // Binding type : 63
0x0000002e, // Constant number : 46
0x40000000, // x : 2
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x0000003f, // Binding type : 63
0x0000002f, // Constant number : 47
0x40000000, // x : 2
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x0000003f, // Binding type : 63
0x00000030, // Constant number : 48
0x40000000, // x : 2
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x0000003f, // Binding type : 63
0x00000031, // Constant number : 49
0x40000000, // x : 2
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x0000003f, // Binding type : 63
0x00000032, // Constant number : 50
0x40000000, // x : 2
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x0000003f, // Binding type : 63
0x00000033, // Constant number : 51
0x40000000, // x : 2
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x0000003f, // Binding type : 63
0x00000034, // Constant number : 52
0x3f800000, // x : 1
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x0000003f, // Binding type : 63
0x00000035, // Constant number : 53
0x3f800000, // x : 1
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x0000003f, // Binding type : 63
0x00000036, // Constant number : 54
0x40000000, // x : 2
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x0000003f, // Binding type : 63
0x00000037, // Constant number : 55
0x40000000, // x : 2
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x0000003f, // Binding type : 63
0x00000038, // Constant number : 56
0x40000000, // x : 2
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x0000003f, // Binding type : 63
0x00000039, // Constant number : 57
0x40000000, // x : 2
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x0000003f, // Binding type : 63
0x0000003a, // Constant number : 58
0x40000000, // x : 2
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x0000003f, // Binding type : 63
0x0000003b, // Constant number : 59
0x40000000, // x : 2
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x0000003f, // Binding type : 63
0x0000003c, // Constant number : 60
0x40000000, // x : 2
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x0000003f, // Binding type : 63
0x0000003d, // Constant number : 61
0x3f800000, // x : 1
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x0000003f, // Binding type : 63
0x0000003e, // Constant number : 62
0x3f800000, // x : 1
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x0000003f, // Binding type : 63
0x0000003f, // Constant number : 63
0x40000000, // x : 2
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x0000003f, // Binding type : 63
0x00000040, // Constant number : 64
0x40000000, // x : 2
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x0000003f, // Binding type : 63
0x00000041, // Constant number : 65
0x40000000, // x : 2
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x0000003f, // Binding type : 63
0x00000042, // Constant number : 66
0x40000000, // x : 2
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x0000003f, // Binding type : 63
0x00000043, // Constant number : 67
0x40000000, // x : 2
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x0000003f, // Binding type : 63
0x00000044, // Constant number : 68
0x40000000, // x : 2
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x0000003f, // Binding type : 63
0x00000045, // Constant number : 69
0x40000000, // x : 2
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x0000003f, // Binding type : 63
0x00000046, // Constant number : 70
0x3f800000, // x : 1
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x0000003f, // Binding type : 63
0x00000047, // Constant number : 71
0x3f800000, // x : 1
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x0000003f, // Binding type : 63
0x00000048, // Constant number : 72
0x40000000, // x : 2
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x0000003f, // Binding type : 63
0x00000049, // Constant number : 73
0x40000000, // x : 2
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x0000003f, // Binding type : 63
0x0000004a, // Constant number : 74
0x40000000, // x : 2
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x0000003f, // Binding type : 63
0x0000004b, // Constant number : 75
0x38d1b717, // x : 0.0001
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x0000003f, // Binding type : 63
0x0000004c, // Constant number : 76
0x3f000000, // x : 0.5
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x0000003f, // Binding type : 63
0x0000004d, // Constant number : 77
0x3f000000, // x : 0.5
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x0000003f, // Binding type : 63
0x0000004e, // Constant number : 78
0x3f800000, // x : 1
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x00000021, // Binding type : 33
0x0000004f, // Constant number : 79
0x00000000, // x : 0
0x00000000, // y : 0
0x00000000, // z : 0
0x00000000, // w : 0
0x00000002, // Section Flags: 2
0x00000022, // Binding type : 34
0x00000050, // Constant number : 80
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

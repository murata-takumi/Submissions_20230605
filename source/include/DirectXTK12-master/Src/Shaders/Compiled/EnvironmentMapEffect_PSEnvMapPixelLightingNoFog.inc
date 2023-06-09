#if 0
//
// Generated by Microsoft (R) D3D Shader Disassembler
//
//
// Input signature:
//
// Name                 Index   Mask Register SysValue  Format   Used
// -------------------- ----- ------ -------- -------- ------- ------
// TEXCOORD                 0   xy          0     NONE   float   xy  
// TEXCOORD                 1   xyzw        1     NONE   float   xyz 
// TEXCOORD                 2   xyz         2     NONE   float   xyz 
// COLOR                    0   xyzw        3     NONE   float   xyzw
//
//
// Output signature:
//
// Name                 Index   Mask Register SysValue  Format   Used
// -------------------- ----- ------ -------- -------- ------- ------
// SV_Target                0   xyzw        0   TARGET   float   xyzw
//
ps_5_1
dcl_globalFlags refactoringAllowed
dcl_constantbuffer CB0[0:0][11], immediateIndexed, space=0
dcl_sampler S0[0:0], mode_default, space=0
dcl_sampler S1[1:1], mode_default, space=0
dcl_resource_texture2d (float,float,float,float) T0[0:0], space=0
dcl_resource_texturecube (float,float,float,float) T1[1:1], space=0
dcl_input_ps linear v0.xy
dcl_input_ps linear v1.xyz
dcl_input_ps linear v2.xyz
dcl_input_ps linear v3.xyzw
dcl_output o0.xyzw
dcl_temps 5
sample r0.xyzw, v0.xyxx, T0[0].xyzw, S0[0]
mul r0.xyzw, r0.xyzw, v3.xyzw
add r1.xyz, -v1.xyzx, CB0[0][10].xyzx
dp3 r1.w, r1.xyzx, r1.xyzx
rsq r1.w, r1.w
mul r1.xyz, r1.wwww, r1.xyzx
dp3 r1.w, v2.xyzx, v2.xyzx
rsq r1.w, r1.w
mul r2.xyz, r1.wwww, v2.xyzx
dp3 r3.x, -CB0[0][4].xyzx, r2.xyzx
dp3 r3.y, -CB0[0][5].xyzx, r2.xyzx
dp3 r3.z, -CB0[0][6].xyzx, r2.xyzx
ge r4.xyz, r3.xyzx, l(0.000000, 0.000000, 0.000000, 0.000000)
and r4.xyz, r4.xyzx, l(0x3f800000, 0x3f800000, 0x3f800000, 0)
mul r3.xyz, r3.xyzx, r4.xyzx
mul r4.xyz, r3.yyyy, CB0[0][8].xyzx
mad r3.xyw, r3.xxxx, CB0[0][7].xyxz, r4.xyxz
mad r3.xyz, r3.zzzz, CB0[0][9].xyzx, r3.xywx
mad r3.xyz, r3.xyzx, CB0[0][2].xyzx, CB0[0][3].xyzx
mul r0.xyz, r0.xyzx, r3.xyzx
dp3 r1.w, -r1.xyzx, r2.xyzx
add r1.w, r1.w, r1.w
mad r1.xyz, r2.xyzx, -r1.wwww, -r1.xyzx
sample r1.xyzw, r1.xyzx, T1[1].xyzw, S1[1]
mul r1.w, r0.w, r1.w
mad r1.xyz, r1.xyzx, r0.wwww, -r0.xyzx
mad r0.xyz, CB0[0][1].xxxx, r1.xyzx, r0.xyzx
mad o0.xyz, CB0[0][0].xyzx, r1.wwww, r0.xyzx
mov o0.w, r0.w
ret 
// Approximately 0 instruction slots used
#endif

const BYTE EnvironmentMapEffect_PSEnvMapPixelLightingNoFog[] =
{
     68,  88,  66,  67, 145,  87, 
    174, 244, 158,  53, 230, 226, 
    215,  60,  36,  66,  73, 141, 
    193, 193,   1,   0,   0,   0, 
    168,   6,   0,   0,   4,   0, 
      0,   0,  48,   0,   0,   0, 
    176,   0,   0,   0, 228,   0, 
      0,   0, 192,   5,   0,   0, 
     73,  83,  71,  78, 120,   0, 
      0,   0,   4,   0,   0,   0, 
      8,   0,   0,   0, 104,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   0,   0,   0,   0, 
      3,   3,   0,   0, 104,   0, 
      0,   0,   1,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   1,   0,   0,   0, 
     15,   7,   0,   0, 104,   0, 
      0,   0,   2,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   2,   0,   0,   0, 
      7,   7,   0,   0, 113,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   3,   0,   0,   0, 
     15,  15,   0,   0,  84,  69, 
     88,  67,  79,  79,  82,  68, 
      0,  67,  79,  76,  79,  82, 
      0, 171,  79,  83,  71,  78, 
     44,   0,   0,   0,   1,   0, 
      0,   0,   8,   0,   0,   0, 
     32,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      3,   0,   0,   0,   0,   0, 
      0,   0,  15,   0,   0,   0, 
     83,  86,  95,  84,  97, 114, 
    103, 101, 116,   0, 171, 171, 
     83,  72,  69,  88, 212,   4, 
      0,   0,  81,   0,   0,   0, 
     53,   1,   0,   0, 106,   8, 
      0,   1,  89,   0,   0,   7, 
     70, 142,  48,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,  11,   0, 
      0,   0,   0,   0,   0,   0, 
     90,   0,   0,   6,  70, 110, 
     48,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
     90,   0,   0,   6,  70, 110, 
     48,   0,   1,   0,   0,   0, 
      1,   0,   0,   0,   1,   0, 
      0,   0,   0,   0,   0,   0, 
     88,  24,   0,   7,  70, 126, 
     48,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,  85,  85,   0,   0, 
      0,   0,   0,   0,  88,  48, 
      0,   7,  70, 126,  48,   0, 
      1,   0,   0,   0,   1,   0, 
      0,   0,   1,   0,   0,   0, 
     85,  85,   0,   0,   0,   0, 
      0,   0,  98,  16,   0,   3, 
     50,  16,  16,   0,   0,   0, 
      0,   0,  98,  16,   0,   3, 
    114,  16,  16,   0,   1,   0, 
      0,   0,  98,  16,   0,   3, 
    114,  16,  16,   0,   2,   0, 
      0,   0,  98,  16,   0,   3, 
    242,  16,  16,   0,   3,   0, 
      0,   0, 101,   0,   0,   3, 
    242,  32,  16,   0,   0,   0, 
      0,   0, 104,   0,   0,   2, 
      5,   0,   0,   0,  69,   0, 
      0,  11, 242,   0,  16,   0, 
      0,   0,   0,   0,  70,  16, 
     16,   0,   0,   0,   0,   0, 
     70, 126,  32,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,  96,  32,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
     56,   0,   0,   7, 242,   0, 
     16,   0,   0,   0,   0,   0, 
     70,  14,  16,   0,   0,   0, 
      0,   0,  70,  30,  16,   0, 
      3,   0,   0,   0,   0,   0, 
      0,  10, 114,   0,  16,   0, 
      1,   0,   0,   0,  70,  18, 
     16, 128,  65,   0,   0,   0, 
      1,   0,   0,   0,  70, 130, 
     48,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,  10,   0, 
      0,   0,  16,   0,   0,   7, 
    130,   0,  16,   0,   1,   0, 
      0,   0,  70,   2,  16,   0, 
      1,   0,   0,   0,  70,   2, 
     16,   0,   1,   0,   0,   0, 
     68,   0,   0,   5, 130,   0, 
     16,   0,   1,   0,   0,   0, 
     58,   0,  16,   0,   1,   0, 
      0,   0,  56,   0,   0,   7, 
    114,   0,  16,   0,   1,   0, 
      0,   0, 246,  15,  16,   0, 
      1,   0,   0,   0,  70,   2, 
     16,   0,   1,   0,   0,   0, 
     16,   0,   0,   7, 130,   0, 
     16,   0,   1,   0,   0,   0, 
     70,  18,  16,   0,   2,   0, 
      0,   0,  70,  18,  16,   0, 
      2,   0,   0,   0,  68,   0, 
      0,   5, 130,   0,  16,   0, 
      1,   0,   0,   0,  58,   0, 
     16,   0,   1,   0,   0,   0, 
     56,   0,   0,   7, 114,   0, 
     16,   0,   2,   0,   0,   0, 
    246,  15,  16,   0,   1,   0, 
      0,   0,  70,  18,  16,   0, 
      2,   0,   0,   0,  16,   0, 
      0,  10,  18,   0,  16,   0, 
      3,   0,   0,   0,  70, 130, 
     48, 128,  65,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   4,   0,   0,   0, 
     70,   2,  16,   0,   2,   0, 
      0,   0,  16,   0,   0,  10, 
     34,   0,  16,   0,   3,   0, 
      0,   0,  70, 130,  48, 128, 
     65,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      5,   0,   0,   0,  70,   2, 
     16,   0,   2,   0,   0,   0, 
     16,   0,   0,  10,  66,   0, 
     16,   0,   3,   0,   0,   0, 
     70, 130,  48, 128,  65,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   6,   0, 
      0,   0,  70,   2,  16,   0, 
      2,   0,   0,   0,  29,   0, 
      0,  10, 114,   0,  16,   0, 
      4,   0,   0,   0,  70,   2, 
     16,   0,   3,   0,   0,   0, 
      2,  64,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   1,   0,   0,  10, 
    114,   0,  16,   0,   4,   0, 
      0,   0,  70,   2,  16,   0, 
      4,   0,   0,   0,   2,  64, 
      0,   0,   0,   0, 128,  63, 
      0,   0, 128,  63,   0,   0, 
    128,  63,   0,   0,   0,   0, 
     56,   0,   0,   7, 114,   0, 
     16,   0,   3,   0,   0,   0, 
     70,   2,  16,   0,   3,   0, 
      0,   0,  70,   2,  16,   0, 
      4,   0,   0,   0,  56,   0, 
      0,   9, 114,   0,  16,   0, 
      4,   0,   0,   0,  86,   5, 
     16,   0,   3,   0,   0,   0, 
     70, 130,  48,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      8,   0,   0,   0,  50,   0, 
      0,  11, 178,   0,  16,   0, 
      3,   0,   0,   0,   6,   0, 
     16,   0,   3,   0,   0,   0, 
     70, 136,  48,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      7,   0,   0,   0,  70,   8, 
     16,   0,   4,   0,   0,   0, 
     50,   0,   0,  11, 114,   0, 
     16,   0,   3,   0,   0,   0, 
    166,  10,  16,   0,   3,   0, 
      0,   0,  70, 130,  48,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   9,   0,   0,   0, 
     70,   3,  16,   0,   3,   0, 
      0,   0,  50,   0,   0,  13, 
    114,   0,  16,   0,   3,   0, 
      0,   0,  70,   2,  16,   0, 
      3,   0,   0,   0,  70, 130, 
     48,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   2,   0, 
      0,   0,  70, 130,  48,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   3,   0,   0,   0, 
     56,   0,   0,   7, 114,   0, 
     16,   0,   0,   0,   0,   0, 
     70,   2,  16,   0,   0,   0, 
      0,   0,  70,   2,  16,   0, 
      3,   0,   0,   0,  16,   0, 
      0,   8, 130,   0,  16,   0, 
      1,   0,   0,   0,  70,   2, 
     16, 128,  65,   0,   0,   0, 
      1,   0,   0,   0,  70,   2, 
     16,   0,   2,   0,   0,   0, 
      0,   0,   0,   7, 130,   0, 
     16,   0,   1,   0,   0,   0, 
     58,   0,  16,   0,   1,   0, 
      0,   0,  58,   0,  16,   0, 
      1,   0,   0,   0,  50,   0, 
      0,  11, 114,   0,  16,   0, 
      1,   0,   0,   0,  70,   2, 
     16,   0,   2,   0,   0,   0, 
    246,  15,  16, 128,  65,   0, 
      0,   0,   1,   0,   0,   0, 
     70,   2,  16, 128,  65,   0, 
      0,   0,   1,   0,   0,   0, 
     69,   0,   0,  11, 242,   0, 
     16,   0,   1,   0,   0,   0, 
     70,   2,  16,   0,   1,   0, 
      0,   0,  70, 126,  32,   0, 
      1,   0,   0,   0,   1,   0, 
      0,   0,   0,  96,  32,   0, 
      1,   0,   0,   0,   1,   0, 
      0,   0,  56,   0,   0,   7, 
    130,   0,  16,   0,   1,   0, 
      0,   0,  58,   0,  16,   0, 
      0,   0,   0,   0,  58,   0, 
     16,   0,   1,   0,   0,   0, 
     50,   0,   0,  10, 114,   0, 
     16,   0,   1,   0,   0,   0, 
     70,   2,  16,   0,   1,   0, 
      0,   0, 246,  15,  16,   0, 
      0,   0,   0,   0,  70,   2, 
     16, 128,  65,   0,   0,   0, 
      0,   0,   0,   0,  50,   0, 
      0,  11, 114,   0,  16,   0, 
      0,   0,   0,   0,   6, 128, 
     48,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   1,   0, 
      0,   0,  70,   2,  16,   0, 
      1,   0,   0,   0,  70,   2, 
     16,   0,   0,   0,   0,   0, 
     50,   0,   0,  11, 114,  32, 
     16,   0,   0,   0,   0,   0, 
     70, 130,  48,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0, 246,  15, 
     16,   0,   1,   0,   0,   0, 
     70,   2,  16,   0,   0,   0, 
      0,   0,  54,   0,   0,   5, 
    130,  32,  16,   0,   0,   0, 
      0,   0,  58,   0,  16,   0, 
      0,   0,   0,   0,  62,   0, 
      0,   1,  82,  84,  83,  48, 
    224,   0,   0,   0,   2,   0, 
      0,   0,   5,   0,   0,   0, 
     24,   0,   0,   0,   0,   0, 
      0,   0, 224,   0,   0,   0, 
     29,   0,   0,   0,   0,   0, 
      0,   0,   5,   0,   0,   0, 
     84,   0,   0,   0,   0,   0, 
      0,   0,   5,   0,   0,   0, 
    116,   0,   0,   0,   0,   0, 
      0,   0,   5,   0,   0,   0, 
    148,   0,   0,   0,   0,   0, 
      0,   0,   5,   0,   0,   0, 
    180,   0,   0,   0,   2,   0, 
      0,   0,   0,   0,   0,   0, 
    212,   0,   0,   0,   1,   0, 
      0,   0,  92,   0,   0,   0, 
      0,   0,   0,   0,   1,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0, 255, 255, 255, 255, 
      1,   0,   0,   0, 124,   0, 
      0,   0,   3,   0,   0,   0, 
      1,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0, 255, 255, 
    255, 255,   1,   0,   0,   0, 
    156,   0,   0,   0,   0,   0, 
      0,   0,   1,   0,   0,   0, 
      1,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
    255, 255, 255, 255,   1,   0, 
      0,   0, 188,   0,   0,   0, 
      3,   0,   0,   0,   1,   0, 
      0,   0,   1,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0, 255, 255, 255, 255, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0
};

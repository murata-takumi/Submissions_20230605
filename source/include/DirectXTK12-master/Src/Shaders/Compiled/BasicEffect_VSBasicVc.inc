#if 0
//
// Generated by Microsoft (R) D3D Shader Disassembler
//
//
// Input signature:
//
// Name                 Index   Mask Register SysValue  Format   Used
// -------------------- ----- ------ -------- -------- ------- ------
// SV_Position              0   xyzw        0     NONE   float   xyzw
// COLOR                    0   xyzw        1     NONE   float   xyzw
//
//
// Output signature:
//
// Name                 Index   Mask Register SysValue  Format   Used
// -------------------- ----- ------ -------- -------- ------- ------
// COLOR                    0   xyzw        0     NONE   float   xyzw
// COLOR                    1   xyzw        1     NONE   float   xyzw
// SV_Position              0   xyzw        2      POS   float   xyzw
//
vs_5_1
dcl_globalFlags refactoringAllowed
dcl_constantbuffer CB0[0:0][26], immediateIndexed, space=0
dcl_input v0.xyzw
dcl_input v1.xyzw
dcl_output o0.xyzw
dcl_output o1.xyzw
dcl_output_siv o2.xyzw, position
dp4 o2.x, v0.xyzw, CB0[0][22].xyzw
dp4 o2.y, v0.xyzw, CB0[0][23].xyzw
dp4 o2.z, v0.xyzw, CB0[0][24].xyzw
dp4 o2.w, v0.xyzw, CB0[0][25].xyzw
dp4_sat o1.w, v0.xyzw, CB0[0][14].xyzw
mul o0.xyzw, v1.xyzw, CB0[0][0].xyzw
mov o1.xyz, l(0,0,0,0)
ret 
// Approximately 0 instruction slots used
#endif

const BYTE BasicEffect_VSBasicVc[] =
{
     68,  88,  66,  67,  74, 167, 
    159,  60,  62, 173, 217,  40, 
     84, 145, 225,  25,   0, 212, 
    130, 216,   1,   0,   0,   0, 
    148,   2,   0,   0,   4,   0, 
      0,   0,  48,   0,   0,   0, 
    132,   0,   0,   0, 240,   0, 
      0,   0,  92,   2,   0,   0, 
     73,  83,  71,  78,  76,   0, 
      0,   0,   2,   0,   0,   0, 
      8,   0,   0,   0,  56,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   0,   0,   0,   0, 
     15,  15,   0,   0,  68,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   1,   0,   0,   0, 
     15,  15,   0,   0,  83,  86, 
     95,  80, 111, 115, 105, 116, 
    105, 111, 110,   0,  67,  79, 
     76,  79,  82,   0, 171, 171, 
     79,  83,  71,  78, 100,   0, 
      0,   0,   3,   0,   0,   0, 
      8,   0,   0,   0,  80,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   0,   0,   0,   0, 
     15,   0,   0,   0,  80,   0, 
      0,   0,   1,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   1,   0,   0,   0, 
     15,   0,   0,   0,  86,   0, 
      0,   0,   0,   0,   0,   0, 
      1,   0,   0,   0,   3,   0, 
      0,   0,   2,   0,   0,   0, 
     15,   0,   0,   0,  67,  79, 
     76,  79,  82,   0,  83,  86, 
     95,  80, 111, 115, 105, 116, 
    105, 111, 110,   0, 171, 171, 
     83,  72,  69,  88, 100,   1, 
      0,   0,  81,   0,   1,   0, 
     89,   0,   0,   0, 106,   8, 
      0,   1,  89,   0,   0,   7, 
     70, 142,  48,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,  26,   0, 
      0,   0,   0,   0,   0,   0, 
     95,   0,   0,   3, 242,  16, 
     16,   0,   0,   0,   0,   0, 
     95,   0,   0,   3, 242,  16, 
     16,   0,   1,   0,   0,   0, 
    101,   0,   0,   3, 242,  32, 
     16,   0,   0,   0,   0,   0, 
    101,   0,   0,   3, 242,  32, 
     16,   0,   1,   0,   0,   0, 
    103,   0,   0,   4, 242,  32, 
     16,   0,   2,   0,   0,   0, 
      1,   0,   0,   0,  17,   0, 
      0,   9,  18,  32,  16,   0, 
      2,   0,   0,   0,  70,  30, 
     16,   0,   0,   0,   0,   0, 
     70, 142,  48,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
     22,   0,   0,   0,  17,   0, 
      0,   9,  34,  32,  16,   0, 
      2,   0,   0,   0,  70,  30, 
     16,   0,   0,   0,   0,   0, 
     70, 142,  48,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
     23,   0,   0,   0,  17,   0, 
      0,   9,  66,  32,  16,   0, 
      2,   0,   0,   0,  70,  30, 
     16,   0,   0,   0,   0,   0, 
     70, 142,  48,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
     24,   0,   0,   0,  17,   0, 
      0,   9, 130,  32,  16,   0, 
      2,   0,   0,   0,  70,  30, 
     16,   0,   0,   0,   0,   0, 
     70, 142,  48,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
     25,   0,   0,   0,  17,  32, 
      0,   9, 130,  32,  16,   0, 
      1,   0,   0,   0,  70,  30, 
     16,   0,   0,   0,   0,   0, 
     70, 142,  48,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
     14,   0,   0,   0,  56,   0, 
      0,   9, 242,  32,  16,   0, 
      0,   0,   0,   0,  70,  30, 
     16,   0,   1,   0,   0,   0, 
     70, 142,  48,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,  54,   0, 
      0,   8, 114,  32,  16,   0, 
      1,   0,   0,   0,   2,  64, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
     62,   0,   0,   1,  82,  84, 
     83,  48,  48,   0,   0,   0, 
      2,   0,   0,   0,   1,   0, 
      0,   0,  24,   0,   0,   0, 
      0,   0,   0,   0,  48,   0, 
      0,   0,  29,   0,   0,   0, 
      2,   0,   0,   0,   0,   0, 
      0,   0,  36,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0
};

#if 0
//
// Generated by Microsoft (R) D3D Shader Disassembler
//
//
// Input signature:
//
// Name                 Index   Mask Register SysValue  Format   Used
// -------------------- ----- ------ -------- -------- ------- ------
// SV_Position              0   xyzw        0      POS   float       
// TEXCOORD                 0   xy          1     NONE   float   xy  
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
dcl_constantbuffer CB0[0:0][1], immediateIndexed, space=0
dcl_sampler S0[0:0], mode_default, space=0
dcl_resource_texture2d (float,float,float,float) T0[0:0], space=0
dcl_input_ps linear v1.xy
dcl_output o0.xyzw
dcl_temps 1
sample r0.xyzw, v1.xyxx, T0[0].xyzw, S0[0]
mul_sat o0.xyz, r0.xyzx, CB0[0][0].xxxx
mov o0.w, r0.w
ret 
// Approximately 0 instruction slots used
#endif

const BYTE ToneMap_PSSaturate[] =
{
     68,  88,  66,  67, 156, 248, 
      6, 200, 221, 128, 218, 119, 
    151, 169,  49,  22, 157,  55, 
     51, 172,   1,   0,   0,   0, 
     64,   2,   0,   0,   4,   0, 
      0,   0,  48,   0,   0,   0, 
    136,   0,   0,   0, 188,   0, 
      0,   0, 168,   1,   0,   0, 
     73,  83,  71,  78,  80,   0, 
      0,   0,   2,   0,   0,   0, 
      8,   0,   0,   0,  56,   0, 
      0,   0,   0,   0,   0,   0, 
      1,   0,   0,   0,   3,   0, 
      0,   0,   0,   0,   0,   0, 
     15,   0,   0,   0,  68,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   1,   0,   0,   0, 
      3,   3,   0,   0,  83,  86, 
     95,  80, 111, 115, 105, 116, 
    105, 111, 110,   0,  84,  69, 
     88,  67,  79,  79,  82,  68, 
      0, 171, 171, 171,  79,  83, 
     71,  78,  44,   0,   0,   0, 
      1,   0,   0,   0,   8,   0, 
      0,   0,  32,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   3,   0,   0,   0, 
      0,   0,   0,   0,  15,   0, 
      0,   0,  83,  86,  95,  84, 
     97, 114, 103, 101, 116,   0, 
    171, 171,  83,  72,  69,  88, 
    228,   0,   0,   0,  81,   0, 
      0,   0,  57,   0,   0,   0, 
    106,   8,   0,   1,  89,   0, 
      0,   7,  70, 142,  48,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      1,   0,   0,   0,   0,   0, 
      0,   0,  90,   0,   0,   6, 
     70, 110,  48,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,  88,  24,   0,   7, 
     70, 126,  48,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,  85,  85, 
      0,   0,   0,   0,   0,   0, 
     98,  16,   0,   3,  50,  16, 
     16,   0,   1,   0,   0,   0, 
    101,   0,   0,   3, 242,  32, 
     16,   0,   0,   0,   0,   0, 
    104,   0,   0,   2,   1,   0, 
      0,   0,  69,   0,   0,  11, 
    242,   0,  16,   0,   0,   0, 
      0,   0,  70,  16,  16,   0, 
      1,   0,   0,   0,  70, 126, 
     32,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,  96, 
     32,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,  56,  32, 
      0,   9, 114,  32,  16,   0, 
      0,   0,   0,   0,  70,   2, 
     16,   0,   0,   0,   0,   0, 
      6, 128,  48,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,  54,   0, 
      0,   5, 130,  32,  16,   0, 
      0,   0,   0,   0,  58,   0, 
     16,   0,   0,   0,   0,   0, 
     62,   0,   0,   1,  82,  84, 
     83,  48, 144,   0,   0,   0, 
      2,   0,   0,   0,   2,   0, 
      0,   0,  24,   0,   0,   0, 
      1,   0,   0,   0,  92,   0, 
      0,   0,  30,   0,   0,   0, 
      0,   0,   0,   0,   5,   0, 
      0,   0,  48,   0,   0,   0, 
      2,   0,   0,   0,   5,   0, 
      0,   0,  80,   0,   0,   0, 
      1,   0,   0,   0,  56,   0, 
      0,   0,   0,   0,   0,   0, 
      1,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0, 255, 255, 
    255, 255,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      3,   0,   0,   0,   3,   0, 
      0,   0,   3,   0,   0,   0, 
      0,   0,   0,   0,  16,   0, 
      0,   0,   4,   0,   0,   0, 
      2,   0,   0,   0,   0,   0, 
      0,   0, 255, 255, 127, 127, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   5,   0,   0,   0
};

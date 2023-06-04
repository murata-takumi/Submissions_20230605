#include"FBXShaderHeader.hlsli"

//FBXモデル用ピクセルシェーダー
//UV座標を基に頂点にテクスチャを付与する
float4 FBXPS(Output input) : SV_TARGET
{
	return tex.Sample(smp,input.uv);
}
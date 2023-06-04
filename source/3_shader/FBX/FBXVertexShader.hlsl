#include"FBXShaderHeader.hlsli"

//FBXモデル用頂点シェーダー
//パイプラインを通して受け取った頂点に対し各行列で座標変換を行う
Output FBXVS
(
	float4 pos : POSITION,
	float4 normal : NORMAL,
	float2 uv : TEXCOORD,
	float3 tangent : TANGENT,
	float4 color : COLOR,
	uint4 ids : IDs,
	float4 weight : WEIGHT
)
{
	Output output;

	//頂点に影響を与える4個のボーン行列をウェイトに応じて加算
	matrix boneTransform = bone[ids.r] * weight.r;
	boneTransform += bone[ids.g] * weight.g;
	boneTransform += bone[ids.b] * weight.b;
	boneTransform += bone[ids.a] * weight.a;

	//座標変換
	float4 localPos = mul(boneTransform, float4(pos));	//ボーン行列を反映
	float4 worldPos = mul(world, localPos);				//ワールド行列を反映
	float4 viewPos = mul(view, worldPos);				//ビュー行列を反映
	float4 projPos = mul(proj, viewPos);				//プロジェクション行列を反映

	output.svpos = projPos;								//頂点座標
	output.color = color;								//カラー
	output.uv = uv;										//UV座標

	return output;
}
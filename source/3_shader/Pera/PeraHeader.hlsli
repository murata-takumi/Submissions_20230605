//画面用シェーダーのヘッダーファイル
//ここでいう画面とは、モデル等のレンダリング結果のことである
//シェーダーで使用するデータをまとめている

Texture2D<float4> tex : register(t0);		//画面テクスチャ
SamplerState smp : register(s0);			//サンプラー

//フェードイン・アウトを決定するスロット
cbuffer Fade : register(b0)
{
	float fade;
};				

//出力用構造体
struct Output
{
	float4 svpos : SV_POSITION;				//頂点情報
	float2 uv : TEXCOORD;					//UV座標
};
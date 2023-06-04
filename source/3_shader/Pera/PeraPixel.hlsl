#include"peraHeader.hlsli"

//画面用ピクセルシェーダー
//画面にフェードイン・アウト等の演出を付与する
float4 PeraPS(Output input) : SV_TARGET
{
	float4 res = tex.Sample(smp, input.uv);		//画面を描画
	res.rgb -= (res.rgb * fade);				//画面テクスチャのRGBを調整し、フェードイン・アウトを実装する

	return res;
}
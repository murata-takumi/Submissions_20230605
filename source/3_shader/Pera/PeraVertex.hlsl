#include"peraHeader.hlsli"

//画面用頂点シェーダー
//頂点座標、UV座標を渡している
Output PeraVS(float4 pos : POSITION, float2 uv : TEXCOORD)
{
	Output output;			//出力用構造体を宣言
	output.svpos = pos;		//頂点座標を入力
	output.uv = uv;			//UV座標を入力

	return output;			//ピクセルシェーダーに渡す
}
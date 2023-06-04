//FBXモデル用ヘッダーファイル
//シェーダーで用いる各種データをまとめている

Texture2D<float4> tex : register(t0);	//テクスチャ
SamplerState smp : register(s0);		//サンプラー（テクスチャの取得方法を決める）

//ビュープロジェクション用スロット
cbuffer SceneData : register(b0)
{
	matrix view;						//ビュー行列
	matrix proj;						//プロジェクション行列
	float3 eye;							//カメラ座標
};

//座標変換用スロット
cbuffer Transform : register(b1)
{
	matrix world;						//ワールド変換行列
	matrix bone[256];					//ボーン毎の変換行列
}

//パイプラインステートから流れてくるデータを受け止める構造体
struct Output
{
	float4 svpos	: SV_POSITION;		//システム用座標
	float4 pos		: POSITION;			//座標
	float4 normal	: NORMAL;			//法線
	float2 uv		: TEXCOORD;			//テクスチャのUV値
	float3 tangent	: TANGENT;			//正接
	float4 color	: COLOR;			//カラー
	uint4  ids		: IDs;				//頂点に影響を与えるボーンのID（頂点1個当たり4本）
	float4 weight	: WEIGHT;			//各ボーンのウェイト（頂点1個当たり4本）
};
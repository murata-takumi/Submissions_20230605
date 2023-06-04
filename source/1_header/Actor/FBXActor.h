#pragma once
#include "Actor/AssimpLoader.h"

class Dx12Wrapper;
class AssimpLoader;
class FBXActor
{	template<typename T>using ComPtr = ComPtr<T>;

private:
	HRESULT result;															//関数の返り値

	Dx12Wrapper& _dx12;														//Dx12Wrapperインスタンス

	AssimpLoader _loader;													//AssimpLoaderインスタンス

	const aiScene* _scene;													//モデルデータを保持するポインタ

	vector<Mesh> _meshes;													//モデル読み込み用メッシュ配列
	map<string, unsigned int> _boneMapping;									//ボーン名とインデックスの連想配列
	vector<BoneInfo> _boneInfo;												//ボーン情報

	vector<ID3D12Resource*> vertexBuffers;									//メッシュ用頂点バッファー配列
	vector<D3D12_VERTEX_BUFFER_VIEW> VBViews;								//メッシュ用頂点バッファービュー配列

	vector<ID3D12Resource*> indexBuffers;									//メッシュ用インデックスバッファー配列
	vector<D3D12_INDEX_BUFFER_VIEW> IBViews;								//メッシュ用インデックスバッファービュー配列

	ComPtr<ID3D12Resource> transformBuffer;									//ワールド行列用バッファー
	ComPtr<ID3D12DescriptorHeap> transformHeap;								//ワールド行列用ヒープ

	ComPtr<ID3D12DescriptorHeap> texHeap;									//テクスチャ用ヒープ
	vector<D3D12_GPU_DESCRIPTOR_HANDLE> GPUHandles;							//テクスチャバッファービューのGPUハンドル配列

	XMMATRIX* _mappedMatrices = nullptr;									//モデルの座標変換行列を格納する構造体のポインタ

	vector<XMMATRIX> _boneMat;												//ボーン行列のベクトル

	map<string, aiAnimation*> _anims;										//アニメーションとその名前の連想配列
	vector<string> _animStr;												//アニメーション名のベクトル

	string _currentAnimStr;													//現在再生しているアニメーション名

	SIZE _winSize;															//ウィンドウサイズ

	bool _isLoop;															//アニメーションをループするかどうか決める真理値

	LARGE_INTEGER _startTime;												//アニメーション開始時の時間
	LARGE_INTEGER _currentTime;												//現在時間
	LARGE_INTEGER _freq;													//周期数

	void ReadNodeHeirarchy(float animationTime,								//ノード階層を読み込む関数
		const aiNode* pNode, const XMMATRIX& parentTrans);

	const aiNodeAnim* FindNodeAnim											//aiAnimationからノード名が一致したaiNodeAnimを取り出す関数	
		(const aiAnimation* animation, const string nodeName);

	XMMATRIX CalcInterpolatedScaling(										//スケーリングアニメーションを補完する関数
		float animationTime,const aiNodeAnim* nodeAnim);
	XMMATRIX CalcInterpolatedRotation(										//回転アニメーションを補完する関数
		float animationTime, const aiNodeAnim* nodeAnim);
	XMMATRIX CalcInterpolatedPosition(										//座標移動アニメーションを補完する関数
		float animationTime, const aiNodeAnim* nodeAnim);

	int FindScaling(float animationTime, const aiNodeAnim* nodeAnim);		//スケーリングアニメーションのキーを取り出す関数
	int FindRotation(float animationTime,const aiNodeAnim* nodeAnim);		//回転アニメーションのキーを取り出す関数
	int FindPosition(float animationTime,const aiNodeAnim* nodeAnim);		//座標移動アニメーションのキーを取り出す関数

	void InitModel(const wchar_t* filePath);								//モデル関連の初期化を行う関数
	void InitAnimation();													//アニメーション関連の初期化を行う関数

	void BoneTransform(float timeInSeconds);								//モデルのアニメーション用行列を求める関数

	HRESULT CreateVertexBufferView();										//頂点バッファー・ビュー作成関数
	HRESULT CreateIndexBufferView();										//インデックスバッファー・ビュー作成関数
	HRESULT CreateTransformView();											//座標変換用バッファー・ビュー作成関数
	HRESULT CreateShaderResourceView();										//シェーダーリソース・ビュー作成関数

public:	
	FBXActor(Dx12Wrapper& dx12, const wchar_t* filePath);	//コンストラクタ
	~FBXActor();											//デストラクタ

	vector<string> GetAnimstr();							//アニメーション名のベクトルを返す関数
	string GetCurentAnimStr();								//現在実行しているアニメーション名を返す関数

	void SetAnimStr(string animStr);						//実行するアニメーション名を指定する関数
	void SetLoop(bool val);									//アニメーションをループするかどうか決める関数

	void StartAnimation();									//アニメーションを開始する関数

	void Draw();											//毎フレームの描画処理
	void Update();											//毎フレームの座標変換処理
};
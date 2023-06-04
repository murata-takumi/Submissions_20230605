#pragma once
#include "Application.h"

#include <assimp/Importer.hpp>
#include <assimp/matrix4x4.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <d3d12.h>
#include <DirectXMath.h>
#include <string>

struct aiMesh;
struct aiMaterial;

/// <summary>
/// FBXモデルの頂点用構造体
/// </summary>
struct FBXVertex
{
	XMFLOAT3 position;					//座標
	XMFLOAT3 normal;					//法線
	XMFLOAT2 uv;						//UV座標
	XMFLOAT3 tangent;					//正接
	XMFLOAT4 color;						//頂点カラー
	XMUINT4 ids;						//ボーンID
	XMFLOAT4 weights;					//ボーンの影響値
};

/// <summary>
/// FBXモデルを構成するメッシュの構造体
/// </summary>
struct Mesh
{
	vector<FBXVertex> vertices;		//頂点ベクトル
	vector<uint32_t> indices;		//インデックスベクトル
	wstring diffuseMap;				//テクスチャのファイルパス
};

/// <summary>
/// ボーンそのものが持つ情報
/// </summary>
struct BoneInfo
{
	XMMATRIX _boneOffset;	//頂点を回転の中心へ移動させる行列
	XMMATRIX _finalTrans;	//最終的な回転行列
};

/// <summary>
/// モデルデータを受け取る構造体
/// </summary>
struct ImportSettings
{
	vector<Mesh>& meshes;						//出力先のメッシュベクトル
	map<string, unsigned int>& boneMapping;		//ボーン名とインデックスの連想配列
	vector<BoneInfo>& boneInfos;				//ボーン情報
	bool inverseU = false;						//U座標を反転させるか
	bool inverseV = false;						//V座標を反転させるか
};

class AssimpLoader
{
public:
	AssimpLoader();														//コンストラクタ
	~AssimpLoader();													//デストラクタ

	const aiScene* LoadScene(const wchar_t* filePath);					//シーンデータをロードする関数

	void LoadModel(const aiScene* scene, const wchar_t* filePath,		//モデルをロードする関数
		ImportSettings& import);

private:
	unsigned int _boneNum = 0;														//ボーン数

	void LoadMesh(Mesh& dst, const aiMesh* src, bool inverseU, bool inverseV);		//メッシュをロードする関数
	void LoadBone(Mesh& dst, const aiMesh* src,										//メッシュ内のボーンをロードする関数
		map<string, unsigned int>& boneMapping,vector<BoneInfo>& boneInfos);
	void LoadTexture(const wchar_t* filename,Mesh& dst,const aiMaterial* src);		//テクスチャをロードする関数

	void SetIndexAndWeight(FBXVertex& vert,unsigned int id,float weight);			//頂点にボーンIDとウェイトを設定する関数

	Assimp::Importer importer;														//モデルデータインポート用オブジェクト
};
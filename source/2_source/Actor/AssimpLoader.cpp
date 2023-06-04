#include "Functions.h"
#include "Actor/AssimpLoader.h"

#include <filesystem>

namespace fs = std::filesystem;

/// <summary>
/// aiMatrix4x4を受け取りXMMATRIXを返す関数
/// </summary>
/// <param name="mat">aiMatrix4x4</param>
/// <returns>XMMATRIX</returns>
XMMATRIX LoadMatrix4x4(const aiMatrix4x4& mat)
{
	XMFLOAT4X4 ret = XMFLOAT4X4
	(
		mat.a1, mat.b1, mat.c1, mat.d1,
		mat.a2, mat.b2, mat.c2, mat.d2,
		mat.a3, mat.b3, mat.c3, mat.d3,
		mat.a4, mat.b4, mat.c4, mat.d4
	);

	return XMLoadFloat4x4(&ret);
}

/// <summary>
/// コンストラクタ
/// 特に処理はしない
/// </summary>
AssimpLoader::AssimpLoader()
{

}

/// <summary>
/// デストラクタ
/// 特に処理はしない
/// </summary>
AssimpLoader::~AssimpLoader()
{

}

/// <summary>
/// モデル情報を持っているインスタンスを取得する関数
/// </summary>
/// <param name="filePath">モデルのパス</param>
/// <returns>モデル情報を持っているインスタンス</returns>
const aiScene*
AssimpLoader::LoadScene(const wchar_t* filePath)
{
	if (filePath == nullptr)						//ファイル名がおかしかったら処理中断
	{
		assert(0);
		return nullptr;
	}

	auto path = ToString(filePath);					//ファイル名からパスを取得
	
	int flag = 0;									//ビット演算を行い、インポート時の設定フラグを設定する
	flag |= aiProcess_Triangulate;
	flag |= aiProcess_CalcTangentSpace;
	flag |= aiProcess_GenSmoothNormals;
	flag |= aiProcess_GenUVCoords;
	flag |= aiProcess_RemoveRedundantMaterials;
	flag |= aiProcess_OptimizeMeshes;
	flag |= aiProcess_JoinIdenticalVertices;

	auto ret = importer.ReadFile(path, flag);		//インポーターにファイルパスとフラグを読み込ませる
	if (ret == nullptr)
	{
		assert(0);
		return nullptr;
	}

	return ret;
}

/// <summary>
/// モデル用の情報を書き込み、返す関数
/// </summary>
/// <param name="scene">モデル情報を持つインスタンス</param>
/// <param name="filePath">モデルのパス</param>
/// <param name="import">書き込み先の構造体</param>
void
AssimpLoader::LoadModel(const aiScene* scene, const wchar_t* filePath, ImportSettings& import)
{
	auto& meshes = import.meshes;															//メッシュ配列
	auto& mapping = import.boneMapping;														//ボーン
	auto& info = import.boneInfos;															//ボーン情報
	auto inverseU = import.inverseU;														//U座標を反転させるか
	auto inverseV = import.inverseV;														//V座標を反転させるか

	if (0 < scene->mNumMeshes)																//メッシュを持っている場合以下の処理を行う
	{
		meshes.clear();																		//メッシュをリセット
		meshes.resize(scene->mNumMeshes);													//メッシュ数を再設定
		for (unsigned int i = 0; i < meshes.size(); ++i)
		{
			const aiMesh* pMesh = scene->mMeshes[i];										//各メッシュを取得

			LoadMesh(meshes[i], pMesh, inverseU, inverseV);									//メッシュを読み込み
			LoadBone(meshes[i], pMesh,mapping,info);										//ボーンを読み込む

			const aiMaterial* pMaterial = scene->mMaterials[pMesh->mMaterialIndex];			//各マテリアルを取得
			LoadTexture(filePath, meshes[i], pMaterial);									//メッシュにマテリアルを読み込ませる
		}
	}
}

/// <summary>
/// FBXのメッシュを読み込む関数
/// </summary>
/// <param name="dst">メッシュ配列</param>
/// <param name="src">FBXモデル内のメッシュ</param>
/// <param name="inverseU">U座標を反転させるか</param>
/// <param name="inverseV">V座標を反転させるか</param>
void
AssimpLoader::LoadMesh(Mesh& dst, const aiMesh* src, bool inverseU, bool inverseV)
{
	aiVector3D zero3D(0.0f, 0.0f, 0.0f);													//メッシュを構成する頂点座標の初期値
	aiColor4D zeroColor(0.0f, 0.0f, 0.0f, 0.0f);											//頂点カラーの初期値

	dst.vertices.resize(src->mNumVertices);													//頂点数を再設定

	for (auto i = 0u; i < src->mNumVertices; ++i)											//頂点毎に処理する
	{
		auto position = (src->mVertices[i]);												//頂点座標
		auto normal = (src->mNormals[i]);													//法線
		auto uv = (src->HasTextureCoords(0)) ? &(src->mTextureCoords[0][i]) : &zero3D;		//UV座標
		auto tangent = (src->HasTangentsAndBitangents()) ? &(src->mTangents[i]) : &zero3D;	//正接
		auto color = (src->HasVertexColors(0)) ? &(src->mColors[0][i]) : &zeroColor;		//頂点カラー

		if (inverseU)																		//反転オプションがあったらUVを反転させる
		{
			uv->x = 1 - uv->x;
		}
		if (inverseV)
		{
			uv->y = 1 - uv->y;
		}

		FBXVertex vertex = {};
		vertex.position = XMFLOAT3(position.x, position.y, position.z);						//座標
		vertex.normal = XMFLOAT3(normal.x, normal.y, normal.z);								//法線
		vertex.uv = XMFLOAT2(uv->x, uv->y);													//UV座標
		vertex.tangent = XMFLOAT3(tangent->x, tangent->y, tangent->z);						//正接
		vertex.color = XMFLOAT4(color->r, color->g, color->b, color->a);					//頂点カラー

		dst.vertices[i] = vertex;															//ベクトルに格納
	}

	dst.indices.resize(src->mNumFaces * 3);													//インデックス数をメッシュ数×3に調整

	for (auto i = 0u; i < src->mNumFaces; ++i)												//インデックス情報の格納
	{
		const auto& face = src->mFaces[i];

		dst.indices[i * 3 + 0] = face.mIndices[0];
		dst.indices[i * 3 + 1] = face.mIndices[1];
		dst.indices[i * 3 + 2] = face.mIndices[2];
	}
}

/// <summary>
/// 各メッシュに保存されているボーンを読み込む関数
/// </summary>
/// <param name="dst">読み込んだメッシュ</param>
/// <param name="src">書き込み先のメッシュ</param>
/// <param name="boneMapping">ボーンとボーン名の連想配列</param>
/// <param name="boneInfos">ボーン情報</param>
void
AssimpLoader::LoadBone(Mesh& dst, const aiMesh* src, map<string, unsigned int>& boneMapping, vector<BoneInfo>& boneInfos)
{
	for (unsigned int i = 0; i < src->mNumBones; i++)							//メッシュが持つ各ボーンデータに対し以下の処理を行う
	{
		unsigned int boneIndex = 0;												//インデックス

		auto bone = src->mBones[i];												//ボーン

		string boneName = bone->mName.data;										//ボーン名

		//ボーン情報・ボーン名にデータを格納する処理
		if (boneMapping.find(boneName) == boneMapping.end())					//ボーン名が登録されていない場合
		{
			boneIndex = _boneNum;												//インデックスを更新
			_boneNum++;															//インデックスに加算

			BoneInfo bi;														//ボーン情報を登録
			boneInfos.push_back(bi);
		}
		else																	//既にある場合インデックスを既出のボーンのものに更新
		{
			boneIndex = boneMapping[boneName];
		}

		boneInfos[boneIndex]._boneOffset = LoadMatrix4x4(bone->mOffsetMatrix);	//ボーン行列（座標をボーン空間へ移す）を取得

		boneMapping[boneName] = boneIndex;										//連想配列にインデックスを登録

		for (UINT j = 0; j < bone->mNumWeights; j++)							//ボーンに格納されているウェイトに対しループ
		{
			UINT id = bone->mWeights[j].mVertexId;								//ウェイトに対応する頂点のインデックスを取得
			float weight = bone->mWeights[j].mWeight;							//ウェイトの数値を取得

			SetIndexAndWeight(dst.vertices[id], boneIndex, weight);				//頂点にボーン番号・ウェイトを格納
		}
	}
}

/// <summary>
/// FBXファイルからテクスチャを読み込む関数
/// </summary>
/// <param name="filename">FBXファイル名</param>
/// <param name="dst">メッシュ配列</param>
/// <param name="src">マテリアル</param>
void
AssimpLoader::LoadTexture(const wchar_t* filename, Mesh& dst, const aiMaterial* src)
{
	aiString path;

	if (src->Get(AI_MATKEY_TEXTURE_DIFFUSE(0), path) == AI_SUCCESS)	//テクスチャパスを読み込めたら処理を実行
	{
		
		auto dir = GetDirectoryPath(filename);						//テクスチャパスは相対パスで入っているので、まずディレクトリ名を取得

		auto file = GetFilePath(path.C_Str());						//ファイル名を取得

		dst.diffuseMap = dir + L"textures/" + file;					//最後にディレクトリ名・フォルダ名・ファイル名をくっ付ける
	}
	else
	{
		dst.diffuseMap.clear();										//ディフューズマップをクリア
	}
}

/// <summary>
/// 頂点にボーンID、ウェイトを設定する関数
/// </summary>
/// <param name="vert">書き込み先の頂点</param>
/// <param name="id">ボーンID</param>
/// <param name="weight">ウェイト</param>
void
AssimpLoader::SetIndexAndWeight(FBXVertex& vert, unsigned int id, float weight)
{
	if (vert.ids.w == 0 && fabs(vert.weights.w) <= FLT_EPSILON)			//IDの中から空の箇所を探し、そこにIDとウェイトを書き込む
	{
		vert.ids.w = id;
		vert.weights.w = weight;
	}
	else if (vert.ids.x == 0 && fabs(vert.weights.x) <= FLT_EPSILON)
	{
		vert.ids.x = id;
		vert.weights.x = weight;
	}
	else if (vert.ids.y == 0 && fabs(vert.weights.y) <= FLT_EPSILON)
	{
		vert.ids.y = id;
		vert.weights.y = weight;
	}
	else if (vert.ids.z == 0 && fabs(vert.weights.z) <= FLT_EPSILON)
	{
		vert.ids.z = id;
		vert.weights.z = weight;
	}
}
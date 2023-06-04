#include "Functions.h"
#include "Actor/AssimpLoader.h"

#include <filesystem>

namespace fs = std::filesystem;

/// <summary>
/// aiMatrix4x4���󂯎��XMMATRIX��Ԃ��֐�
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
/// �R���X�g���N�^
/// ���ɏ����͂��Ȃ�
/// </summary>
AssimpLoader::AssimpLoader()
{

}

/// <summary>
/// �f�X�g���N�^
/// ���ɏ����͂��Ȃ�
/// </summary>
AssimpLoader::~AssimpLoader()
{

}

/// <summary>
/// ���f�����������Ă���C���X�^���X���擾����֐�
/// </summary>
/// <param name="filePath">���f���̃p�X</param>
/// <returns>���f�����������Ă���C���X�^���X</returns>
const aiScene*
AssimpLoader::LoadScene(const wchar_t* filePath)
{
	if (filePath == nullptr)						//�t�@�C�������������������珈�����f
	{
		assert(0);
		return nullptr;
	}

	auto path = ToString(filePath);					//�t�@�C��������p�X���擾
	
	int flag = 0;									//�r�b�g���Z���s���A�C���|�[�g���̐ݒ�t���O��ݒ肷��
	flag |= aiProcess_Triangulate;
	flag |= aiProcess_CalcTangentSpace;
	flag |= aiProcess_GenSmoothNormals;
	flag |= aiProcess_GenUVCoords;
	flag |= aiProcess_RemoveRedundantMaterials;
	flag |= aiProcess_OptimizeMeshes;
	flag |= aiProcess_JoinIdenticalVertices;

	auto ret = importer.ReadFile(path, flag);		//�C���|�[�^�[�Ƀt�@�C���p�X�ƃt���O��ǂݍ��܂���
	if (ret == nullptr)
	{
		assert(0);
		return nullptr;
	}

	return ret;
}

/// <summary>
/// ���f���p�̏����������݁A�Ԃ��֐�
/// </summary>
/// <param name="scene">���f���������C���X�^���X</param>
/// <param name="filePath">���f���̃p�X</param>
/// <param name="import">�������ݐ�̍\����</param>
void
AssimpLoader::LoadModel(const aiScene* scene, const wchar_t* filePath, ImportSettings& import)
{
	auto& meshes = import.meshes;															//���b�V���z��
	auto& mapping = import.boneMapping;														//�{�[��
	auto& info = import.boneInfos;															//�{�[�����
	auto inverseU = import.inverseU;														//U���W�𔽓]�����邩
	auto inverseV = import.inverseV;														//V���W�𔽓]�����邩

	if (0 < scene->mNumMeshes)																//���b�V���������Ă���ꍇ�ȉ��̏������s��
	{
		meshes.clear();																		//���b�V�������Z�b�g
		meshes.resize(scene->mNumMeshes);													//���b�V�������Đݒ�
		for (unsigned int i = 0; i < meshes.size(); ++i)
		{
			const aiMesh* pMesh = scene->mMeshes[i];										//�e���b�V�����擾

			LoadMesh(meshes[i], pMesh, inverseU, inverseV);									//���b�V����ǂݍ���
			LoadBone(meshes[i], pMesh,mapping,info);										//�{�[����ǂݍ���

			const aiMaterial* pMaterial = scene->mMaterials[pMesh->mMaterialIndex];			//�e�}�e���A�����擾
			LoadTexture(filePath, meshes[i], pMaterial);									//���b�V���Ƀ}�e���A����ǂݍ��܂���
		}
	}
}

/// <summary>
/// FBX�̃��b�V����ǂݍ��ފ֐�
/// </summary>
/// <param name="dst">���b�V���z��</param>
/// <param name="src">FBX���f�����̃��b�V��</param>
/// <param name="inverseU">U���W�𔽓]�����邩</param>
/// <param name="inverseV">V���W�𔽓]�����邩</param>
void
AssimpLoader::LoadMesh(Mesh& dst, const aiMesh* src, bool inverseU, bool inverseV)
{
	aiVector3D zero3D(0.0f, 0.0f, 0.0f);													//���b�V�����\�����钸�_���W�̏����l
	aiColor4D zeroColor(0.0f, 0.0f, 0.0f, 0.0f);											//���_�J���[�̏����l

	dst.vertices.resize(src->mNumVertices);													//���_�����Đݒ�

	for (auto i = 0u; i < src->mNumVertices; ++i)											//���_���ɏ�������
	{
		auto position = (src->mVertices[i]);												//���_���W
		auto normal = (src->mNormals[i]);													//�@��
		auto uv = (src->HasTextureCoords(0)) ? &(src->mTextureCoords[0][i]) : &zero3D;		//UV���W
		auto tangent = (src->HasTangentsAndBitangents()) ? &(src->mTangents[i]) : &zero3D;	//����
		auto color = (src->HasVertexColors(0)) ? &(src->mColors[0][i]) : &zeroColor;		//���_�J���[

		if (inverseU)																		//���]�I�v�V��������������UV�𔽓]������
		{
			uv->x = 1 - uv->x;
		}
		if (inverseV)
		{
			uv->y = 1 - uv->y;
		}

		FBXVertex vertex = {};
		vertex.position = XMFLOAT3(position.x, position.y, position.z);						//���W
		vertex.normal = XMFLOAT3(normal.x, normal.y, normal.z);								//�@��
		vertex.uv = XMFLOAT2(uv->x, uv->y);													//UV���W
		vertex.tangent = XMFLOAT3(tangent->x, tangent->y, tangent->z);						//����
		vertex.color = XMFLOAT4(color->r, color->g, color->b, color->a);					//���_�J���[

		dst.vertices[i] = vertex;															//�x�N�g���Ɋi�[
	}

	dst.indices.resize(src->mNumFaces * 3);													//�C���f�b�N�X�������b�V�����~3�ɒ���

	for (auto i = 0u; i < src->mNumFaces; ++i)												//�C���f�b�N�X���̊i�[
	{
		const auto& face = src->mFaces[i];

		dst.indices[i * 3 + 0] = face.mIndices[0];
		dst.indices[i * 3 + 1] = face.mIndices[1];
		dst.indices[i * 3 + 2] = face.mIndices[2];
	}
}

/// <summary>
/// �e���b�V���ɕۑ�����Ă���{�[����ǂݍ��ފ֐�
/// </summary>
/// <param name="dst">�ǂݍ��񂾃��b�V��</param>
/// <param name="src">�������ݐ�̃��b�V��</param>
/// <param name="boneMapping">�{�[���ƃ{�[�����̘A�z�z��</param>
/// <param name="boneInfos">�{�[�����</param>
void
AssimpLoader::LoadBone(Mesh& dst, const aiMesh* src, map<string, unsigned int>& boneMapping, vector<BoneInfo>& boneInfos)
{
	for (unsigned int i = 0; i < src->mNumBones; i++)							//���b�V�������e�{�[���f�[�^�ɑ΂��ȉ��̏������s��
	{
		unsigned int boneIndex = 0;												//�C���f�b�N�X

		auto bone = src->mBones[i];												//�{�[��

		string boneName = bone->mName.data;										//�{�[����

		//�{�[�����E�{�[�����Ƀf�[�^���i�[���鏈��
		if (boneMapping.find(boneName) == boneMapping.end())					//�{�[�������o�^����Ă��Ȃ��ꍇ
		{
			boneIndex = _boneNum;												//�C���f�b�N�X���X�V
			_boneNum++;															//�C���f�b�N�X�ɉ��Z

			BoneInfo bi;														//�{�[������o�^
			boneInfos.push_back(bi);
		}
		else																	//���ɂ���ꍇ�C���f�b�N�X�����o�̃{�[���̂��̂ɍX�V
		{
			boneIndex = boneMapping[boneName];
		}

		boneInfos[boneIndex]._boneOffset = LoadMatrix4x4(bone->mOffsetMatrix);	//�{�[���s��i���W���{�[����Ԃֈڂ��j���擾

		boneMapping[boneName] = boneIndex;										//�A�z�z��ɃC���f�b�N�X��o�^

		for (UINT j = 0; j < bone->mNumWeights; j++)							//�{�[���Ɋi�[����Ă���E�F�C�g�ɑ΂����[�v
		{
			UINT id = bone->mWeights[j].mVertexId;								//�E�F�C�g�ɑΉ����钸�_�̃C���f�b�N�X���擾
			float weight = bone->mWeights[j].mWeight;							//�E�F�C�g�̐��l���擾

			SetIndexAndWeight(dst.vertices[id], boneIndex, weight);				//���_�Ƀ{�[���ԍ��E�E�F�C�g���i�[
		}
	}
}

/// <summary>
/// FBX�t�@�C������e�N�X�`����ǂݍ��ފ֐�
/// </summary>
/// <param name="filename">FBX�t�@�C����</param>
/// <param name="dst">���b�V���z��</param>
/// <param name="src">�}�e���A��</param>
void
AssimpLoader::LoadTexture(const wchar_t* filename, Mesh& dst, const aiMaterial* src)
{
	aiString path;

	if (src->Get(AI_MATKEY_TEXTURE_DIFFUSE(0), path) == AI_SUCCESS)	//�e�N�X�`���p�X��ǂݍ��߂��珈�������s
	{
		
		auto dir = GetDirectoryPath(filename);						//�e�N�X�`���p�X�͑��΃p�X�œ����Ă���̂ŁA�܂��f�B���N�g�������擾

		auto file = GetFilePath(path.C_Str());						//�t�@�C�������擾

		dst.diffuseMap = dir + L"textures/" + file;					//�Ō�Ƀf�B���N�g�����E�t�H���_���E�t�@�C�����������t����
	}
	else
	{
		dst.diffuseMap.clear();										//�f�B�t���[�Y�}�b�v���N���A
	}
}

/// <summary>
/// ���_�Ƀ{�[��ID�A�E�F�C�g��ݒ肷��֐�
/// </summary>
/// <param name="vert">�������ݐ�̒��_</param>
/// <param name="id">�{�[��ID</param>
/// <param name="weight">�E�F�C�g</param>
void
AssimpLoader::SetIndexAndWeight(FBXVertex& vert, unsigned int id, float weight)
{
	if (vert.ids.w == 0 && fabs(vert.weights.w) <= FLT_EPSILON)			//ID�̒������̉ӏ���T���A������ID�ƃE�F�C�g����������
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
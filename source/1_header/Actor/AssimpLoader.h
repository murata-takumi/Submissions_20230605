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
/// FBX���f���̒��_�p�\����
/// </summary>
struct FBXVertex
{
	XMFLOAT3 position;					//���W
	XMFLOAT3 normal;					//�@��
	XMFLOAT2 uv;						//UV���W
	XMFLOAT3 tangent;					//����
	XMFLOAT4 color;						//���_�J���[
	XMUINT4 ids;						//�{�[��ID
	XMFLOAT4 weights;					//�{�[���̉e���l
};

/// <summary>
/// FBX���f�����\�����郁�b�V���̍\����
/// </summary>
struct Mesh
{
	vector<FBXVertex> vertices;		//���_�x�N�g��
	vector<uint32_t> indices;		//�C���f�b�N�X�x�N�g��
	wstring diffuseMap;				//�e�N�X�`���̃t�@�C���p�X
};

/// <summary>
/// �{�[�����̂��̂������
/// </summary>
struct BoneInfo
{
	XMMATRIX _boneOffset;	//���_����]�̒��S�ֈړ�������s��
	XMMATRIX _finalTrans;	//�ŏI�I�ȉ�]�s��
};

/// <summary>
/// ���f���f�[�^���󂯎��\����
/// </summary>
struct ImportSettings
{
	vector<Mesh>& meshes;						//�o�͐�̃��b�V���x�N�g��
	map<string, unsigned int>& boneMapping;		//�{�[�����ƃC���f�b�N�X�̘A�z�z��
	vector<BoneInfo>& boneInfos;				//�{�[�����
	bool inverseU = false;						//U���W�𔽓]�����邩
	bool inverseV = false;						//V���W�𔽓]�����邩
};

class AssimpLoader
{
public:
	AssimpLoader();														//�R���X�g���N�^
	~AssimpLoader();													//�f�X�g���N�^

	const aiScene* LoadScene(const wchar_t* filePath);					//�V�[���f�[�^�����[�h����֐�

	void LoadModel(const aiScene* scene, const wchar_t* filePath,		//���f�������[�h����֐�
		ImportSettings& import);

private:
	unsigned int _boneNum = 0;														//�{�[����

	void LoadMesh(Mesh& dst, const aiMesh* src, bool inverseU, bool inverseV);		//���b�V�������[�h����֐�
	void LoadBone(Mesh& dst, const aiMesh* src,										//���b�V�����̃{�[�������[�h����֐�
		map<string, unsigned int>& boneMapping,vector<BoneInfo>& boneInfos);
	void LoadTexture(const wchar_t* filename,Mesh& dst,const aiMaterial* src);		//�e�N�X�`�������[�h����֐�

	void SetIndexAndWeight(FBXVertex& vert,unsigned int id,float weight);			//���_�Ƀ{�[��ID�ƃE�F�C�g��ݒ肷��֐�

	Assimp::Importer importer;														//���f���f�[�^�C���|�[�g�p�I�u�W�F�N�g
};
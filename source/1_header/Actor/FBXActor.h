#pragma once
#include "Actor/AssimpLoader.h"

class Dx12Wrapper;
class AssimpLoader;
class FBXActor
{	template<typename T>using ComPtr = ComPtr<T>;

private:
	HRESULT result;															//�֐��̕Ԃ�l

	Dx12Wrapper& _dx12;														//Dx12Wrapper�C���X�^���X

	AssimpLoader _loader;													//AssimpLoader�C���X�^���X

	const aiScene* _scene;													//���f���f�[�^��ێ�����|�C���^

	vector<Mesh> _meshes;													//���f���ǂݍ��ݗp���b�V���z��
	map<string, unsigned int> _boneMapping;									//�{�[�����ƃC���f�b�N�X�̘A�z�z��
	vector<BoneInfo> _boneInfo;												//�{�[�����

	vector<ID3D12Resource*> vertexBuffers;									//���b�V���p���_�o�b�t�@�[�z��
	vector<D3D12_VERTEX_BUFFER_VIEW> VBViews;								//���b�V���p���_�o�b�t�@�[�r���[�z��

	vector<ID3D12Resource*> indexBuffers;									//���b�V���p�C���f�b�N�X�o�b�t�@�[�z��
	vector<D3D12_INDEX_BUFFER_VIEW> IBViews;								//���b�V���p�C���f�b�N�X�o�b�t�@�[�r���[�z��

	ComPtr<ID3D12Resource> transformBuffer;									//���[���h�s��p�o�b�t�@�[
	ComPtr<ID3D12DescriptorHeap> transformHeap;								//���[���h�s��p�q�[�v

	ComPtr<ID3D12DescriptorHeap> texHeap;									//�e�N�X�`���p�q�[�v
	vector<D3D12_GPU_DESCRIPTOR_HANDLE> GPUHandles;							//�e�N�X�`���o�b�t�@�[�r���[��GPU�n���h���z��

	XMMATRIX* _mappedMatrices = nullptr;									//���f���̍��W�ϊ��s����i�[����\���̂̃|�C���^

	vector<XMMATRIX> _boneMat;												//�{�[���s��̃x�N�g��

	map<string, aiAnimation*> _anims;										//�A�j���[�V�����Ƃ��̖��O�̘A�z�z��
	vector<string> _animStr;												//�A�j���[�V�������̃x�N�g��

	string _currentAnimStr;													//���ݍĐ����Ă���A�j���[�V������

	SIZE _winSize;															//�E�B���h�E�T�C�Y

	bool _isLoop;															//�A�j���[�V���������[�v���邩�ǂ������߂�^���l

	LARGE_INTEGER _startTime;												//�A�j���[�V�����J�n���̎���
	LARGE_INTEGER _currentTime;												//���ݎ���
	LARGE_INTEGER _freq;													//������

	void ReadNodeHeirarchy(float animationTime,								//�m�[�h�K�w��ǂݍ��ފ֐�
		const aiNode* pNode, const XMMATRIX& parentTrans);

	const aiNodeAnim* FindNodeAnim											//aiAnimation����m�[�h������v����aiNodeAnim�����o���֐�	
		(const aiAnimation* animation, const string nodeName);

	XMMATRIX CalcInterpolatedScaling(										//�X�P�[�����O�A�j���[�V������⊮����֐�
		float animationTime,const aiNodeAnim* nodeAnim);
	XMMATRIX CalcInterpolatedRotation(										//��]�A�j���[�V������⊮����֐�
		float animationTime, const aiNodeAnim* nodeAnim);
	XMMATRIX CalcInterpolatedPosition(										//���W�ړ��A�j���[�V������⊮����֐�
		float animationTime, const aiNodeAnim* nodeAnim);

	int FindScaling(float animationTime, const aiNodeAnim* nodeAnim);		//�X�P�[�����O�A�j���[�V�����̃L�[�����o���֐�
	int FindRotation(float animationTime,const aiNodeAnim* nodeAnim);		//��]�A�j���[�V�����̃L�[�����o���֐�
	int FindPosition(float animationTime,const aiNodeAnim* nodeAnim);		//���W�ړ��A�j���[�V�����̃L�[�����o���֐�

	void InitModel(const wchar_t* filePath);								//���f���֘A�̏��������s���֐�
	void InitAnimation();													//�A�j���[�V�����֘A�̏��������s���֐�

	void BoneTransform(float timeInSeconds);								//���f���̃A�j���[�V�����p�s������߂�֐�

	HRESULT CreateVertexBufferView();										//���_�o�b�t�@�[�E�r���[�쐬�֐�
	HRESULT CreateIndexBufferView();										//�C���f�b�N�X�o�b�t�@�[�E�r���[�쐬�֐�
	HRESULT CreateTransformView();											//���W�ϊ��p�o�b�t�@�[�E�r���[�쐬�֐�
	HRESULT CreateShaderResourceView();										//�V�F�[�_�[���\�[�X�E�r���[�쐬�֐�

public:	
	FBXActor(Dx12Wrapper& dx12, const wchar_t* filePath);	//�R���X�g���N�^
	~FBXActor();											//�f�X�g���N�^

	vector<string> GetAnimstr();							//�A�j���[�V�������̃x�N�g����Ԃ��֐�
	string GetCurentAnimStr();								//���ݎ��s���Ă���A�j���[�V��������Ԃ��֐�

	void SetAnimStr(string animStr);						//���s����A�j���[�V���������w�肷��֐�
	void SetLoop(bool val);									//�A�j���[�V���������[�v���邩�ǂ������߂�֐�

	void StartAnimation();									//�A�j���[�V�������J�n����֐�

	void Draw();											//���t���[���̕`�揈��
	void Update();											//���t���[���̍��W�ϊ�����
};
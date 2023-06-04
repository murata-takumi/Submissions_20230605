#include "Functions.h"
#include "Actor/FBXActor.h"
#include "Wrapper/Dx12Wrapper.h"

const char* CHARA_REF = "Character1_Reference|";	//�A�j���[�V�������ɕt�^���镶���񃊃e����

/// <summary>
/// �R���X�g���N�^
/// </summary>
/// <param name="dx12">Dx12Wrapper�C���X�^���X</param>
/// <param name="filePath">���f���̃p�X</param>
FBXActor::FBXActor(Dx12Wrapper& dx12, const wchar_t* filePath)
	:_dx12(dx12),_isLoop(false)
{
	InitModel(filePath);					//���f���̏�����

	InitAnimation();						//�A�j���[�V�����̏�����

	CreateVertexBufferView();				//���_�o�b�t�@�[�E�r���[�쐬

	CreateIndexBufferView();				//�C���f�b�N�X�o�b�t�@�[�E�r���[�쐬

	CreateTransformView();					//���W�ϊ��p�o�b�t�@�[�E�r���[�쐬

	CreateShaderResourceView();				//�V�F�[�_�[���\�[�X�E�r���[�쐬
}

/// <summary>
/// �f�X�g���N�^
/// �|�C���^���J������
/// </summary>
FBXActor::~FBXActor()
{
	_scene = nullptr;
}

/// <summary>
/// �A�j���[�V�������J�n����֐�
/// </summary>
void
FBXActor::StartAnimation()
{
	QueryPerformanceCounter(&_startTime);				//�A�j���[�V�����J�n���̎��Ԃ��擾
}

/// <summary>
/// ���f���֘A�̏��������s���֐�
/// </summary>
/// <param name="filePath">���f���̃p�X</param>
void
FBXActor::InitModel(const wchar_t* filePath)
{
	ImportSettings settings =							//���f���ǂݍ��ݗp�ݒ�
	{
		_meshes,										//���b�V�����
		_boneMapping,									//�{�[�����ƃC���f�b�N�X�̘A�z�z��
		_boneInfo,										//�s��Ȃǂ̃{�[�����̃x�N�g��
		false,											//U���W�𔽓]�����邩
		true,											//V���W�𔽓]�����邩
	};

	_scene = _loader.LoadScene(filePath);				//�V�[����ǂݍ���

	_loader.LoadModel(_scene, filePath, settings);		//�V�[�������Ƀ��f����ǂݍ���

	_boneMat.resize(_boneInfo.size());					//�{�[���s��̌������߂�
}

/// <summary>
/// �A�j���[�V�����֘A�̏��������s���֐�
/// </summary>
void
FBXActor::InitAnimation()
{
	for (int i = 0; i < _scene->mNumAnimations; i++)				//���f�������S�A�j���[�V�����ɑ΂����s
	{
		_anims[_scene->mAnimations[i]->mName.C_Str()]				//�A�z�z��ɃA�j���[�V�������ƃA�j���[�V�����f�[�^���i�[
			= _scene->mAnimations[i];

		string name = _scene->mAnimations[i]->mName.C_Str();		//�A�j���[�V���������擾

		name = name.erase(0, 21);									//�]���ȕ�������폜

		_animStr.push_back(name);									//�x�N�g���ɒǉ�
	}

	_currentAnimStr = _animStr[0];									//�ŏ��Ɏ��s����A�j���[�V������ݒ�

	ReadNodeHeirarchy(0, _scene->mRootNode, XMMatrixIdentity());	//�{�[���s���������
}

/// <summary>
/// �{�[���ϊ��s����������ފ֐�
/// </summary>
/// <param name="timeInSeconds">���݂̌o�ߎ���</param>
void
FBXActor::BoneTransform(float timeInSeconds)
{
	float ticksPerSecond =																//1�b������̍X�V�񐔂��擾
		static_cast<float>(_anims[CHARA_REF + _currentAnimStr]->mTicksPerSecond != 0 
			? _anims[CHARA_REF + _currentAnimStr]->mTicksPerSecond : 30.0f);

	float timeInTicks = timeInSeconds * ticksPerSecond;									//�����_�ł̍X�V�񐔂��擾
	float duration = _anims[CHARA_REF + _currentAnimStr]->mDuration;					//�A�j���[�V����������

	auto secondFrame =																	//2�߂̃t���[���̎���
		_anims[CHARA_REF + _currentAnimStr]->mChannels[0]->mPositionKeys[1].mTime;

	float animationTime = _isLoop ? fmod(timeInTicks, duration) : timeInTicks;			//�^���l�̒l�ɉ����Ď��Ԃ̎�����ς���

	if (secondFrame < animationTime && animationTime <= duration)						//�K�w�\������ϊ��s���ǂݍ���
	{
		ReadNodeHeirarchy(animationTime, _scene->mRootNode, XMMatrixIdentity());
	}

	for (UINT i = 0,boneLength = _boneMat.size(); i < boneLength; ++i)					//�ϊ��s����X�V����
	{
		_boneMat[i] = _boneInfo[i]._finalTrans;
	}
}

/// <summary>
/// �ċA�I�Ƀm�[�h�̊K�w��ǂݍ��݁A���W�ϊ����s���s����擾����֐�
/// </summary>
/// <param name="animationTime">���݂̌o�ߎ���</param>
/// <param name="pNode">�e�m�[�h</param>
/// <param name="parentTrans">�e�m�[�h�œK�p����Ă���s��</param>
void
FBXActor::ReadNodeHeirarchy(float animationTime, const aiNode* pNode, const XMMATRIX& parentTrans)
{
	string nodeName(pNode->mName.data);													//�m�[�h�����擾

	const aiAnimation* pAnimation = _anims[CHARA_REF + _currentAnimStr];				//�A�j���[�V�������擾

	aiMatrix4x4 aiTrans = pNode->mTransformation;										//�m�[�h��aiMatrix4x4��XMMATRIX�ɕϊ�
	XMFLOAT4X4 temp = XMFLOAT4X4
	(
		aiTrans.a1, aiTrans.b1, aiTrans.c1, aiTrans.d1,
		aiTrans.a2, aiTrans.b2, aiTrans.c2, aiTrans.d2,
		aiTrans.a3, aiTrans.b3, aiTrans.c3, aiTrans.d3,
		aiTrans.a4, aiTrans.b4, aiTrans.c4, aiTrans.d4
	);
	XMMATRIX nodeTrans = XMLoadFloat4x4(&temp);

	const aiNodeAnim* pNodeAnim = FindNodeAnim(pAnimation, nodeName);					//�m�[�h���Ɠ�����aiNodeAnim�����o��

	if (pNodeAnim != nullptr)															//�A�j���[�V����������ꍇ�e�s����擾����
	{
		XMMATRIX scalingM = CalcInterpolatedScaling(animationTime, pNodeAnim);			//�X�P�[�����O�̍s����擾

		XMMATRIX rotationM = CalcInterpolatedRotation(animationTime, pNodeAnim);		//��]�̍s����擾

		XMMATRIX translationM = CalcInterpolatedPosition(animationTime, pNodeAnim);		//���s�ړ��̍s����擾

		nodeTrans = scalingM * rotationM * translationM;								//�s����܂Ƃ߂ď�Z����
	}

	XMMATRIX globalTrans = nodeTrans * parentTrans;										//�e�̕ϊ��s��Ƀm�[�h����ǂݍ��񂾍s���K�p

	if (_boneMapping.find(nodeName) != _boneMapping.end())
	{
		unsigned int boneIdx = _boneMapping[nodeName];									//�{�[�����ɑΉ�����C���f�b�N�X���擾

		_boneInfo[boneIdx]._finalTrans =												//�ŏI�I�ȕϊ��s����擾
			_boneInfo[boneIdx]._boneOffset * globalTrans;
	}

	auto numChildren = pNode->mNumChildren;												//�q�m�[�h�̌�

	for (UINT i = 0; i < numChildren; i++)												//�q�m�[�h�̕��ɂ��ċA�������s��
	{
		ReadNodeHeirarchy(animationTime, pNode->mChildren[i], globalTrans);
	}
}

/// <summary>
/// ���b�V�����ɒ��_�o�b�t�@�[�E�r���[���쐬����֐�
/// </summary>
/// <returns>�����������������ǂ���</returns>
HRESULT
FBXActor::CreateVertexBufferView()
{
	result = S_OK;																//�Ԃ�l��������

	vertexBuffers.reserve(_meshes.size());										//���_�o�b�t�@�[�E�r���[��p�ӂ���
	VBViews.reserve(_meshes.size());

	for (size_t i = 0; i < _meshes.size(); i++)									//�S���b�V���ɑ΂����������s
	{
		ID3D12Resource* tmpVertexBuffer = nullptr;								//�i�[�p�o�b�t�@�[
		D3D12_VERTEX_BUFFER_VIEW tmpVBView = {};								//�i�[�p�r���[

		auto size = sizeof(FBXVertex) * _meshes[i].vertices.size();				//���_�S�̂̃f�[�^�T�C�Y
		auto stride = sizeof(FBXVertex);										//���_��̃f�[�^�T�C�Y

		auto heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);		//�q�[�v�v���p�e�B
		auto desc = CD3DX12_RESOURCE_DESC::Buffer(size);						//���\�[�X�ݒ�

		result = _dx12.Device()->CreateCommittedResource						//�o�b�t�@�[�쐬
		(
			&heapProp,
			D3D12_HEAP_FLAG_NONE,
			&desc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&tmpVertexBuffer)
		);
		if (FAILED(result))
		{
			assert(0);
			return result;
		}

		vector<FBXVertex> data = _meshes[i].vertices;							//���_�f�[�^�擾

	   	FBXVertex* mappedVertex = nullptr;										//���_�f�[�^���o�b�t�@�[�֏�������
		tmpVertexBuffer->Map(0, nullptr, (void**)&mappedVertex);
		copy(begin(data), end(data), mappedVertex);
		tmpVertexBuffer->Unmap(0, nullptr);

		tmpVBView.BufferLocation = tmpVertexBuffer->GetGPUVirtualAddress();		//�r���[�Ƀo�b�t�@�[������������
		tmpVBView.SizeInBytes = tmpVertexBuffer->GetDesc().Width;
		tmpVBView.StrideInBytes = sizeof(FBXVertex);

		vertexBuffers.push_back(tmpVertexBuffer);								//���_�o�b�t�@�[�E�r���[��z��ɒǉ�
		VBViews.push_back(tmpVBView);
	}

	return result;
}

/// <summary>
/// ���b�V�����ɃC���f�b�N�X�o�b�t�@�[�E�r���[���쐬����֐�
/// </summary>
/// <returns></returns>
HRESULT
FBXActor::CreateIndexBufferView()
{
	result = S_OK;																	//�Ԃ�l��������

	indexBuffers.reserve(_meshes.size());											//�C���f�b�N�X�o�b�t�@�[�E�r���[��p�ӂ���
	IBViews.reserve(_meshes.size());
	for (size_t i = 0; i < _meshes.size(); i++)										//�S���b�V���ɑ΂����������s
	{
		ID3D12Resource* tmpIndexBuffer = nullptr;									//�i�[�p�o�b�t�@�[
		D3D12_INDEX_BUFFER_VIEW tmpIBView = {};										//�i�[�p�r���[

		auto size = sizeof(uint32_t) * _meshes[i].indices.size();					//�C���f�b�N�X�S�̂̃f�[�^�T�C�Y

		auto heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);			//�q�[�v�v���p�e�B
		auto desc = CD3DX12_RESOURCE_DESC::Buffer(size);							//���\�[�X�ݒ�

		result = _dx12.Device()->CreateCommittedResource							//�o�b�t�@�[�쐬
		(
			&heapProp,
			D3D12_HEAP_FLAG_NONE,
			&desc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&tmpIndexBuffer)
		);
		if (FAILED(result))
		{
			assert(0);
			return result;
		}

		vector<uint32_t> data = _meshes[i].indices;									//�C���f�b�N�X�f�[�^���擾

		uint32_t* mappedIndex = nullptr;											//�C���f�b�N�X�f�[�^���o�b�t�@�[�ɏ�������
		tmpIndexBuffer->Map(0, nullptr, (void**)&mappedIndex);
		copy(begin(data), end(data), mappedIndex);
		tmpIndexBuffer->Unmap(0, nullptr);

		tmpIBView.BufferLocation = tmpIndexBuffer->GetGPUVirtualAddress();			//�r���[�Ƀo�b�t�@�[������������
		tmpIBView.Format = DXGI_FORMAT_R32_UINT;
		tmpIBView.SizeInBytes = static_cast<UINT>(size);

		indexBuffers.push_back(tmpIndexBuffer);										//�C���f�b�N�X�o�b�t�@�[�E�r���[��z��ɒǉ�
		IBViews.push_back(tmpIBView);
	}

	return result;
}

/// <summary>
/// �I�u�W�F�N�g�̍��W�ϊ��ɗp������q�[�v�E�r���[���쐬����֐�
/// </summary>
/// <returns>�쐬�ł������ǂ���</returns>
HRESULT
FBXActor::CreateTransformView()
{
	result = S_OK;

	auto buffSize = sizeof(XMMATRIX) * (1 + _boneMat.size());						//���[���h�s��p�o�b�t�@�[�̍쐬
	buffSize = (buffSize + 0xff) & ~0xff;
	auto resDesc = CD3DX12_RESOURCE_DESC::Buffer(buffSize);
	result = _dx12.Device()->CreateCommittedResource
	(
		&_uploadHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(transformBuffer.ReleaseAndGetAddressOf())
	);
	if (FAILED(result)) {
		assert(0);
		return result;
	}

	result = transformBuffer->Map(0, nullptr, (void**)&_mappedMatrices);			//���W�ϊ��p�s��̏�������
	if (FAILED(result))
	{
		assert(0);
		return result;
	}
	_mappedMatrices[0] = XMMatrixIdentity();
	copy(_boneMat.begin(), _boneMat.end(), _mappedMatrices + 1);

																					//�f�B�X�N���v�^�q�[�v�ݒ�p�\���̂̍쐬
	D3D12_DESCRIPTOR_HEAP_DESC transformDescHeapDesc = {};
	transformDescHeapDesc.NumDescriptors = 1;										//�Ƃ肠�������[���h�ЂƂ�
	transformDescHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;		//�V�F�[�_�[���猩����悤�ɂ���
	transformDescHeapDesc.NodeMask = 0;												//�m�[�h�}�X�N��0��
	transformDescHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;			//�f�X�N���v�^�q�[�v���

	result = _dx12.Device()->CreateDescriptorHeap(&transformDescHeapDesc,			//�q�[�v�̍쐬
		IID_PPV_ARGS(transformHeap.ReleaseAndGetAddressOf()));
	if (FAILED(result)) {
		assert(0);
		return result;
	}

	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};									//�r���[�ݒ�p�\���̂̍쐬
	cbvDesc.BufferLocation = transformBuffer->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = transformBuffer->GetDesc().Width;

	_dx12.Device()->CreateConstantBufferView(&cbvDesc,								//�r���[�̍쐬
		transformHeap->GetCPUDescriptorHandleForHeapStart());

	return result;
}

/// <summary>
/// �I�u�W�F�N�g�̃e�N�X�`���ɗp������q�[�v�E�r���[���쐬����֐�
/// </summary>
/// <returns>�쐬�ł������ǂ���</returns>
HRESULT
FBXActor::CreateShaderResourceView()
{
	result = S_OK;

	D3D12_DESCRIPTOR_HEAP_DESC texHeapDesc = {};												//�f�B�X�N���v�^�q�[�v�̍쐬
	texHeapDesc.NodeMask = 1;
	texHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	texHeapDesc.NumDescriptors = _meshes.size();
	texHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	result = _dx12.Device()->CreateDescriptorHeap(&texHeapDesc,
		IID_PPV_ARGS(texHeap.ReleaseAndGetAddressOf()));

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};												//SRV�p�\���̂̍쐬
	srvDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;

	auto CPUHeapHandle = texHeap->GetCPUDescriptorHandleForHeapStart();							//�q�[�v�̐擪�A�h���X(CPU)
	auto GPUHeapHandle = texHeap->GetGPUDescriptorHandleForHeapStart();							//�q�[�v�̐擪�A�h���X(GPU)	
	auto incrementSize =																		//�擪�A�h���X�̂��炷��
		_dx12.Device()->GetDescriptorHandleIncrementSize(texHeapDesc.Type);

	TexMetadata meta = {};																		//�e�N�X�`���ǂݍ��ݗp�f�[�^
	ScratchImage scratch = {};

	DXGI_FORMAT format;																			//�e�N�X�`���̃t�H�[�}�b�g
	size_t width;																				//��
	size_t height;																				//����
	UINT16 arraySize;																			//�e�N�X�`���T�C�Y
	UINT16 mipLevels;
	void* pixels;
	UINT rowPitch;
	UINT slicePitch;

	for (size_t i = 0; i < _meshes.size(); i++)													//�e���b�V���ɑ΂��e�N�X�`���̓ǂݍ��ݏ���
	{
		ID3D12Resource* tmpTexBuffer = nullptr;

		wstring path = _meshes[i].diffuseMap;													//�e�N�X�`���̃p�X

		if (wcscmp(path.c_str(), L"") == 0)														//���e�N�X�`��
		{
			vector<unsigned char> data(4 * 4 * 4);
			fill(data.begin(), data.end(), 0x00);

			format = DXGI_FORMAT_R8G8B8A8_UNORM;
			width = 4;
			height = 4;
			arraySize = 1;
			mipLevels = 1;

			pixels = data.data();
			rowPitch = 4 * 4;
			slicePitch = data.size();
		}
		else																					//�ʏ�e�N�X�`��
		{
			auto ext = FileExtension(path);														//�g���q���擾

			
			if (wcscmp(ext.c_str(), L"psd") == 0)												//�g���q��"psd"�������ꍇ�A"tga"�ɕϊ�����
			{
				path = ReplaceExtension(path, "tga");
				ext = FileExtension(path);
			}

			result = _dx12._loadLambdaTable[ToString(ext)](path, &meta, scratch);				//�g���q�ɉ����ēǂݍ��݊֐���ς���
			if (FAILED(result))
			{
				assert(0);
				return result;
			}

			auto img = scratch.GetImage(0, 0, 0);												//�e�N�X�`���f�[�^�̗p��
			format = meta.format;
			width = meta.width;
			height = meta.height;
			arraySize = static_cast<UINT16>(meta.arraySize);
			mipLevels = static_cast<UINT16>(meta.mipLevels);
			pixels = img->pixels;
			rowPitch = static_cast<UINT>(img->rowPitch);
			slicePitch = static_cast<UINT>(img->slicePitch);
		}

		auto heapProp = 
			CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK, D3D12_MEMORY_POOL_L0);	//�o�b�t�@�[�p�q�[�v�v���p�e�B
		auto desc = CD3DX12_RESOURCE_DESC::Tex2D(
			format,
			width,
			height,
			arraySize,
			mipLevels);

		result = _dx12.Device()->CreateCommittedResource										//�o�b�t�@�[�̍쐬
		(
			&heapProp,
			D3D12_HEAP_FLAG_NONE,
			&desc,
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
			nullptr,
			IID_PPV_ARGS(&tmpTexBuffer)
		);
		if (FAILED(result))
		{
			assert(0);
			return result;
		}

		result = tmpTexBuffer->WriteToSubresource(0,											//�e�N�X�`���̏�������s
			nullptr,
			pixels,
			rowPitch,
			slicePitch
		);
		if (FAILED(result))
		{
			assert(0);
			return result;
		}

		srvDesc.Format = tmpTexBuffer->GetDesc().Format;										//�t�H�[�}�b�g�����킹��
		_dx12.Device()->CreateShaderResourceView(tmpTexBuffer, &srvDesc, CPUHeapHandle);		//�e�N�X�`���o�b�t�@�[�r���[���쐬

		GPUHandles.push_back(GPUHeapHandle);													//GPU�̃A�h���X��ǉ�

		CPUHeapHandle.ptr += incrementSize;														//CPU�̃A�h���X�����炷
		GPUHeapHandle.ptr += incrementSize;														//GPU�̃A�h���X�����炷
	}
}

/// <summary>
/// �A�j���[�V�����z��̒����疼�O����v�����A�j���[�V���������o���֐�
/// </summary>
/// <param name="animation">�A�j���[�V�����z��</param>
/// <param name="nodeName">�A�j���[�V����</param>
/// <returns>�m�[�h������v�����A�j���[�V����</returns>
const aiNodeAnim*
FBXActor::FindNodeAnim(const aiAnimation* animation, const string nodeName)
{
	for (int i = 0; i < animation->mNumChannels; i++)			//aiAnimation�����SaiNodeAnim��ΏۂɃ��[�v
	{
		const aiNodeAnim* nodeAnim = animation->mChannels[i];	//aiNodeAnim�̖��O���擾

		if (string(nodeAnim->mNodeName.data) == nodeName)		//���O����v���Ă����炻���Ԃ�
		{
			return nodeAnim;
		}
	}

	return nullptr;												//�Ȃ�������nullptr
}

/// <summary>
/// �X�P�[�����O�̕�Ԃ����{����֐�
/// </summary>
/// <param name="animationTime">���݂̌o�ߎ���</param>
/// <param name="nodeAnim">�A�j���[�V����</param>
/// <returns>�X�P�[�����O�s��</returns>
XMMATRIX
FBXActor::CalcInterpolatedScaling(float animationTime, const aiNodeAnim* nodeAnim)
{
	aiVector3D tempVec;

	if (nodeAnim->mNumScalingKeys == 1)											//�X�P�[�����O�ŗp����L�[��1�̏ꍇ
	{
		tempVec = nodeAnim->mScalingKeys[0].mValue;								//�L�[���擾
	}
	else																		//�����łȂ��ꍇ
	{
		int scalingIdx = FindScaling(animationTime, nodeAnim);					//�X�P�[�����O�Ŏg���L�[�̃C���f�b�N�X���擾
		int nextScalingIdx = scalingIdx + 1;									//���̃L�[�̃C���f�b�N�X���擾
		assert(nextScalingIdx < nodeAnim->mNumScalingKeys);						//�C���f�b�N�X�𐧌�_�̐��Ɣ�r���傫�������狭���I��

		float deltaTime = static_cast<float>(									//�L�[���m�̌o�ߎ��Ԃ̍������擾
			nodeAnim->mScalingKeys[nextScalingIdx].mTime
			- nodeAnim->mScalingKeys[scalingIdx].mTime);

		float factor =															//���݂̌o�ߎ��ԂƃL�[�̌o�ߎ��Ԃ̍������擾�������Ŋ���
			(animationTime
				- static_cast<float>(nodeAnim->mScalingKeys[scalingIdx].mTime)
			)
			/ deltaTime;
		assert(factor >= 0.0f && factor <= 1.0f);								//������0�ȏ�1�ȉ��ł��邱�Ƃ��m�F

		const aiVector3D& start = nodeAnim->mScalingKeys[scalingIdx].mValue;	//���t���[����aiVector3D
		const aiVector3D& end = nodeAnim->mScalingKeys[nextScalingIdx].mValue;	//���t���[����aiVector3D
		aiVector3D delta = end - start;											//���aiVector3D�̍������擾

		tempVec = start + factor * delta;										//���t���[���̃x�N�g���ɍ����ƈ����̐ς����Z�������̂�Ԃ�
	}

	XMVECTOR scaling(XMVectorSet(tempVec.x, tempVec.y, tempVec.z, 0));			//aiVector3D����XMVector�֕ϊ�

	 XMMATRIX ret = XMMatrixScalingFromVector(scaling);							//�X��XMVector����X�P�[�����O�p��XMMATRIX���擾

	return ret;
}

/// <summary>
/// ��]�̕⊮�����s����֐�
/// </summary>
/// <param name="animationTime">���݂̌o�ߎ���</param>
/// <param name="nodeAnim">�A�j���[�V����</param>
/// <returns>��]�s��</returns>
XMMATRIX
FBXActor::CalcInterpolatedRotation(float animationTime, const aiNodeAnim* nodeAnim)
{
	aiQuaternion tempQuat;

	if (nodeAnim->mNumRotationKeys == 1)														//�L�[��������Ȃ��ꍇ���̂܂ܕԂ�
	{
		tempQuat = nodeAnim->mRotationKeys[0].mValue;
	}
	else
	{
		int rotationIdx = FindRotation(animationTime, nodeAnim);								//�L�[�̃C���f�b�N�X���擾
		int nextRotationIdx = rotationIdx + 1;													//���̃L�[�̃C���f�b�N�X���擾
		assert(nextRotationIdx < nodeAnim->mNumRotationKeys);

		float deltaTime = static_cast<float>													//2�̃L�[�̊Ԃ̎��Ԃ��擾
			(
				nodeAnim->mRotationKeys[nextRotationIdx].mTime -
				nodeAnim->mRotationKeys[rotationIdx].mTime
				);
		float factor =																			//���ݎ��ԂƃL�[�̎��Ԃ̍������擾�������Ŋ���
			(animationTime - static_cast<float>(nodeAnim->mRotationKeys[rotationIdx].mTime))
			/ deltaTime;
		assert(factor >= 0.0f && factor <= 1.0f);

		const aiQuaternion& start = nodeAnim->mRotationKeys[rotationIdx].mValue;				//��]�̏����N�H�[�^�j�I��
		const aiQuaternion& end = nodeAnim->mRotationKeys[nextRotationIdx].mValue;				//��]�̍ŏI�N�H�[�^�j�I��

		aiQuaternion::Interpolate(tempQuat, start, end, factor);								//��Ԃ��Đ��K��
		tempQuat = tempQuat.Normalize();
	}

	aiMatrix3x3 rotMat = tempQuat.GetMatrix();													//�N�H�[�^�j�I������aiMatrix3x3���擾
	auto rotTemp = XMFLOAT3X3
	(
		rotMat.a1, rotMat.b1, rotMat.c1,
		rotMat.a2, rotMat.b2, rotMat.c2,
		rotMat.a3, rotMat.b3, rotMat.c3
	);
	XMMATRIX ret = XMLoadFloat3x3(&rotTemp);													//aiMatrix3x3����XMMATRIX�ɕϊ�

	return ret;
}

/// <summary>
/// ���s�ړ��̕�Ԃ����s����֐�
/// </summary>
/// <param name="animationTime">���݂̌o�ߎ���</param>
/// <param name="nodeAnim">�A�j���[�V����</param>
/// <returns>���s�ړ��s��</returns>
XMMATRIX
FBXActor::CalcInterpolatedPosition(float animationTime, const aiNodeAnim* nodeAnim)
{
	aiVector3D tempPos;

	if (nodeAnim->mNumPositionKeys == 1)											//�L�[��������������炻�̂܂ܕԂ�
	{
		tempPos = nodeAnim->mPositionKeys[0].mValue;
	}
	else
	{
		int positionIdx = FindPosition(animationTime, nodeAnim);					//�L�[�̃C���f�b�N�X���擾
		int nextPositionIdx = positionIdx + 1;										//���̃L�[�̃C���f�b�N�X���擾
		assert(nextPositionIdx < nodeAnim->mNumPositionKeys);

		float deltaTime =															//��̃L�[�̍������擾
			static_cast<float>(
				nodeAnim->mPositionKeys[nextPositionIdx].mTime - 
				nodeAnim->mPositionKeys[positionIdx].mTime
			);
		float factor =																//���ݎ��ԂƃL�[�̎��Ԃ̍������擾���A�����Ŋ���
			(
				animationTime - 
				static_cast<float>(nodeAnim->mPositionKeys[positionIdx].mTime)
			)
			/ deltaTime;
		assert(factor >= 0.0f && factor <= 1.0f);

		const aiVector3D& start = nodeAnim->mPositionKeys[positionIdx].mValue;		//�L�[
		const aiVector3D& end = nodeAnim->mPositionKeys[nextPositionIdx].mValue;	//���̃L�[
		aiVector3D delta = end - start;
		tempPos = start + factor * delta;											//���aiVector3D�̕��
	}
	XMVECTOR translation(XMVectorSet(tempPos.x, tempPos.y, tempPos.z, 0));			//aiVector3D����XMVector�ɕϊ�

	XMMATRIX ret = XMMatrixTranslationFromVector(translation);						//XMVector���畽�s�ړ��p��XMMATRIX�ɕϊ�

	return ret;
}

/// <summary>
/// �X�P�[�����O�A�j���[�V�����̃L�[�̃C���f�b�N�X���擾����֐�
/// </summary>
/// <param name="animationTime">���݂̌o�ߎ���</param>
/// <param name="nodeAnim">�A�j���[�V�����f�[�^</param>
/// <returns>�C���f�b�N�X</returns>
int
FBXActor::FindScaling(float animationTime, const aiNodeAnim* nodeAnim)
{
	auto numSclKeys = nodeAnim->mNumScalingKeys;

	assert(numSclKeys > 0);

	for (int i = 0; i < numSclKeys - 1; ++i)
	{
		if (animationTime < static_cast<float>(nodeAnim->mScalingKeys[i + 1].mTime))
		{
			return i;
		}
	}

	assert(0);
	return -1;
}

/// <summary>
/// ��]�Ɋւ���A�j���[�V�����f�[�^�̃C���f�b�N�X���擾����֐�
/// </summary>
/// <param name="animationTime"></param>
/// <param name="nodeAnim">�A�j���[�V�����f�[�^</param>
/// <returns>�C���f�b�N�X</returns>
int 
FBXActor::FindRotation(float animationTime, const aiNodeAnim* nodeAnim)
{
	auto numRotKeys = nodeAnim->mNumRotationKeys;

	assert(numRotKeys > 0);

	for (int i = 0; i < numRotKeys - 1; ++i)
	{
		if (animationTime < static_cast<float>(nodeAnim->mRotationKeys[i + 1].mTime))
		{
			return i;
		}
	}

	assert(0);
	return -1;
}

/// <summary>
/// ���s�ړ��Ɋւ���A�j���[�V�����f�[�^�̃C���f�b�N�X���擾����֐�
/// </summary>
/// <param name="animationTime"></param>
/// <param name="nodeAnim">�A�j���[�V�����f�[�^</param>
/// <returns>�C���f�b�N�X</returns>
int
FBXActor::FindPosition(float animationTime, const aiNodeAnim* nodeAnim)
{
	auto numPosKeys = nodeAnim->mNumPositionKeys;

	assert(numPosKeys > 0);

	for (int i = 0; i < numPosKeys - 1; ++i)
	{
		if (animationTime < static_cast<float>(nodeAnim->mPositionKeys[i + 1].mTime))
		{
			return i;
		}
	}

	assert(0);
	return -1;
}

/// <summary>
/// �A�N�^�[�������Ă���A�j���[�V�����̖��O�̃x�N�g����Ԃ��֐�
/// </summary>
/// <returns>�A�j���[�V�������̃x�N�g��</returns>
vector<string>
FBXActor::GetAnimstr()
{
	return _animStr;
}

/// <summary>
/// ���ݎ��s���Ă���A�j���[�V��������Ԃ��֐�
/// </summary>
/// <returns>�A�j���[�V������</returns>
string
FBXActor::GetCurentAnimStr()
{
	return _currentAnimStr;
}

/// <summary>
/// ���s����A�j���[�V�������w�肷��֐�
/// </summary>
/// <param name="animStr"></param>
void
FBXActor::SetAnimStr(string animStr)
{
	_currentAnimStr = animStr;

	StartAnimation();
}

/// <summary>
/// �A�j���[�V�����̃��[�v�����肷��֐�
/// </summary>
/// <param name="val">���[�v���邩�ǂ��������߂�^���l</param>
void
FBXActor::SetLoop(bool val)
{
	_isLoop = val;

	StartAnimation();
}

/// <summary>
/// ���t���[���̕`�揈�������s����֐�
/// </summary>
void
FBXActor::Draw()
{
	//���W�ϊ��p�f�B�X�N���v�^�q�[�v���Z�b�g
	ID3D12DescriptorHeap* transformHeaps[] = { transformHeap.Get() };
	_dx12.CommandList()->SetDescriptorHeaps(1, transformHeaps);

	//���[�g�p�����[�^�ƃf�B�X�N���v�^�q�[�v�̃n���h�����֘A�t��
	_dx12.CommandList()->SetGraphicsRootDescriptorTable(1, transformHeap->GetGPUDescriptorHandleForHeapStart());

	//���f�����\�����郁�b�V�����Ɉȉ��̏������s��
	for (size_t i = 0; i < _meshes.size(); i++)
	{
		//���_�E�C���f�b�N�X�o�b�t�@�[�r���[�̃Z�b�g
		_dx12.CommandList()->IASetVertexBuffers(0, 1, &VBViews[i]);
		_dx12.CommandList()->IASetIndexBuffer(&IBViews[i]);

		//�e�N�X�`���o�b�t�@�[�r���[�̃Z�b�g
		ID3D12DescriptorHeap* SetTexHeap[] = { texHeap.Get() };
		_dx12.CommandList()->SetDescriptorHeaps(1, SetTexHeap);
		_dx12.CommandList()->SetGraphicsRootDescriptorTable(2, GPUHandles[i]);

		//���b�V���̕`��
		_dx12.CommandList()->DrawIndexedInstanced(_meshes[i].indices.size(), 1, 0, 0, 0);
	}
}

/// <summary>
/// ���t���[���̍��W�ϊ����������s����֐�
/// </summary>
void
FBXActor::Update()
{
	QueryPerformanceCounter(&_currentTime);										//���ݎ��Ԃ��擾

	auto time = GetLIntDiff(_currentTime, _startTime);							//���ݎ��Ԃ�float�ɕϊ�
	BoneTransform(time);														//���ݎ��Ԃ�n���A�s����擾
	copy(_boneMat.begin(), _boneMat.end(), _mappedMatrices + 1);				//�s����V�F�[�_�[�ɓn���A�A�j���[�V���������s
}
#include "Functions.h"
#include "Manager/SpriteManager.h"
#include "Package/Package.h"
#include "Wrapper/Dx12Wrapper.h"

const int ORIGIN = 0;				//���[or��[
const int START_TOP = 480;			//�X�^�[�g�{�^���̏���W
const int END_TOP = 555;			//�I���{�^���̏���W
const int BUTTON_WIDTH = 200;		//�{�^����
const int BUTTON_HEIGHT = 25;		//�{�^������
const int LOADING_WIDTH = 480;		//���[�h��ʂŕ\������A�C�R���̕�
const int LOADIN_HEIGHT = 270;		//���[�h��ʂŕ\������A�C�R���̍���
const int TITLE_DIFF = 180;			//��ʑS�̂���^�C�g����ʂւ̍���

/// <summary>
/// �R���X�g���N�^
/// </summary>
/// <param name="dx12">Dx12Wrapper�C���X�^���X</param>
/// <param name="width">��ʕ�</param>
/// <param name="height">��ʍ���</param>
SpriteManager::SpriteManager(Dx12Wrapper& dx12, LONG width, LONG height):_dx12(dx12),_width(width),_height(height)
{
	CreateSpriteRS();																				//Sprite�p���[�g�V�O�l�`�����쐬
	InitSpriteDevices();																			//Sprite�p�I�u�W�F�N�g��������

	_buttonLeft = (_width / 2) - (BUTTON_WIDTH / 2);												//�{�^���p��`�̍����W
	_buttonRight = (_width / 2) + (BUTTON_WIDTH / 2);												//�{�^���p��`�̉E���W

	AdjustSpriteRect();																				//�e��`���E�B���h�E�ɍ��킹��

	_titleWidth = _titleRect.right - _titleRect.left;												//�^�C�g����
	_titleHeight = _titleRect.bottom - _titleRect.top;												//�^�C�g������

	_incrementSize =																				//�����̃T�C�Y���擾
		_dx12.Device()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);


	CreateUIBufferView(L"Asset/image/background.png","back");										//�w�i
	CreateUIBufferView(L"Asset/image/cursor.png", "cursor");										//�}�E�X�J�[�\��
	CreateUIBufferView(L"Asset/image/button.png", "button");										//�{�^���i�ʏ펞�j

	CreateUIBufferView(L"Asset/image/loading/1.png", "load_1");										//���[�h��ʂɉ�]����A�C�R��
	CreateUIBufferView(L"Asset/image/loading/2.png", "load_2");
	CreateUIBufferView(L"Asset/image/loading/3.png", "load_3");					
	CreateUIBufferView(L"Asset/image/loading/4.png", "load_4");					
	CreateUIBufferView(L"Asset/image/loading/5.png", "load_5");					
	CreateUIBufferView(L"Asset/image/loading/6.png", "load_6");					
	CreateUIBufferView(L"Asset/image/loading/7.png", "load_7");					
	CreateUIBufferView(L"Asset/image/loading/8.png", "load_8");
}

/// <summary>
/// SpriteBatch�����̃��[�g�V�O�l�`���E�V�F�[�_�[���쐬����֐�
/// </summary>
/// <returns>�����������������ǂ���</returns>
HRESULT
SpriteManager::CreateSpriteRS()
{
	CD3DX12_DESCRIPTOR_RANGE spriteTblRange[2] = {};					//�f�B�X�N���v�^�����W(SRV�p)
	spriteTblRange[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);
	spriteTblRange[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 1);

	
	CD3DX12_ROOT_PARAMETER spriteRootParam[3] = {};						//���[�g�p�����[�^(SRV,CBV�p)
	spriteRootParam[0].InitAsDescriptorTable(
		1, &spriteTblRange[0], D3D12_SHADER_VISIBILITY_PIXEL);
	spriteRootParam[1].InitAsConstantBufferView(						//[1]��ConstantBufferView�Ƃ��ď�����
		0, 0, D3D12_SHADER_VISIBILITY_ALL);
	spriteRootParam[2].InitAsDescriptorTable(
		1, &spriteTblRange[1], D3D12_SHADER_VISIBILITY_ALL);

	CD3DX12_STATIC_SAMPLER_DESC samplerDesc = {};						//�T���v���[
	samplerDesc.Init(0);												//������
	samplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;		//�s�N�Z���V�F�[�_�[���猩����悤�ݒ�

	CD3DX12_ROOT_SIGNATURE_DESC rsDesc = {};							//���[�g�V�O�l�`���쐬�p�\����
	rsDesc.Init(3,spriteRootParam,1,&samplerDesc,
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	ID3DBlob* rsBlob = nullptr;											//���[�g�V�O�l�`���̏�����
	ID3DBlob* errorBlob = nullptr;
	auto result = D3D12SerializeRootSignature(
		&rsDesc,
		D3D_ROOT_SIGNATURE_VERSION_1_0,
		&rsBlob,
		&errorBlob);
	if (FAILED(result))
	{
		assert(0);
		return result;
	}

	rsBlob->Release();													//�g��Ȃ��f�[�^���J��

	result = _dx12.Device()->CreateRootSignature(						//���[�g�V�O�l�`���쐬
		0,
		rsBlob->GetBufferPointer(),
		rsBlob->GetBufferSize(),
		IID_PPV_ARGS(_spriteRS.ReleaseAndGetAddressOf())
	);
	if (FAILED(result))
	{
		assert(0);
		return result;
	}

	return result;
}

/// <summary>
/// Sprite�����I�u�W�F�N�g������������֐�
/// </summary>
void
SpriteManager::InitSpriteDevices()
{
	_gmemory = make_unique<GraphicsMemory>(_dx12.Device());								//�O���t�B�b�N�X�������̏�����

	ResourceUploadBatch resUploadBatch(_dx12.Device());									//�X�v���C�g�\���p�I�u�W�F�N�g�̏�����
	resUploadBatch.Begin();

	RenderTargetState rtState(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_D32_FLOAT);		//�����_�[�^�[�Q�b�g�X�e�[�g

	unique_ptr<CommonStates> _states = make_unique<CommonStates>(_dx12.Device());		//�T���v���[���擾���邽��State�I�u�W�F�N�g��������
	D3D12_GPU_DESCRIPTOR_HANDLE wrap = _states->AnisotropicWrap();						//GPU�n���h��

	SpriteBatchPipelineStateDescription psd(rtState, &CommonStates::NonPremultiplied);

	_spriteBatch = make_unique<SpriteBatch>(_dx12.Device(), resUploadBatch, psd);		//�X�v���C�g�\���p�I�u�W�F�N�g

	//�t�H���g�\���p�I�u�W�F�N�g�̏�����
	_heapForSpriteFont = _dx12.CreateDescriptorHeapForSpriteFont();						//�t�H���g�E�摜�\���p�q�[�v
	_tmpCPUHandle = _heapForSpriteFont->GetCPUDescriptorHandleForHeapStart();			//�q�[�v�n���h��(CPU)
	_tmpGPUHandle = _heapForSpriteFont->GetGPUDescriptorHandleForHeapStart();			//�q�[�v�n���h��(GPU)

	_spriteFont = make_unique<SpriteFont>(												//�t�H���g�\���p�I�u�W�F�N�g
		_dx12.Device(),
		resUploadBatch,
		L"Asset/font/fonttest.spritefont",
		_tmpCPUHandle,
		_tmpGPUHandle
		);
	auto future = resUploadBatch.End(_dx12.CommandQueue());								//GPU���֓]��

	_dx12.WaitForCommandQueue();														//GPU���g�p�\�ɂȂ�܂őҋ@
	future.wait();

	_spriteBatch->SetViewport(*_dx12.ViewPort());										//�X�v���C�g�\���p�I�u�W�F�N�g���r���[�|�[�g�֓o�^

	return;
}

/// <summary>
/// �摜�̃o�b�t�@�[�E�r���[���쐬����֐�
/// </summary>
/// <param name="path">�摜�̃p�X</param>
/// <param name="key">�A�z�z��̃L�[</param>
/// <returns>�����������������ǂ���</returns>
HRESULT
SpriteManager::CreateUIBufferView(const wchar_t* path,string key)
{
	TexMetadata meta = {};															//UI�摜�ǂݍ��ݗp�f�[�^
	ScratchImage scratch = {};

	auto ext = FileExtension(path);													//�g���q���擾

	auto result = _dx12._loadLambdaTable[ToString(ext)](path, &meta, scratch);		//�摜�f�[�^�̓ǂݍ���
	if (FAILED(result))
	{
		assert(0);
		return result;
	}

	auto img = scratch.GetImage(0, 0, 0);											//���f�[�^���擾

	DXGI_FORMAT format = meta.format;												//�t�H�[�}�b�g
	size_t width = meta.width;														//��
	size_t height = meta.height;													//����
	UINT16 arraySize = static_cast<UINT16>(meta.arraySize);							//�e�N�X�`���T�C�Y
	UINT16 mipLevels = static_cast<UINT16>(meta.mipLevels);		
	void* pixels = img->pixels;
	UINT rowPitch = static_cast<UINT>(img->rowPitch);
	UINT slicePitch = static_cast<UINT>(img->slicePitch);

	ID3D12Resource* tmpUIBuff = nullptr;											//�摜�f�[�^�������ݗp�o�b�t�@

	auto uiResDesc = CD3DX12_RESOURCE_DESC::Tex2D(									//���\�[�X�쐬�p�f�[�^
		format,
		(UINT)width,
		(UINT)height,
		arraySize,
		(UINT)mipLevels);

	result = _dx12.Device()->CreateCommittedResource(								//���\�[�X�쐬
		&_writeHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&uiResDesc,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		nullptr,
		IID_PPV_ARGS(&tmpUIBuff));
	if (FAILED(result))
	{
		assert(0);
		return result;;
	}

	result = tmpUIBuff->WriteToSubresource(0,										//�摜������������
		nullptr,
		pixels,
		rowPitch,
		slicePitch);
	if (FAILED(result))
	{
		assert(0);
		return result;
	}

	ShiftHandles();																	//�n���h�������炷

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};									//UI�r���[�p�\���̂̍쐬
	srvDesc.Format = tmpUIBuff->GetDesc().Format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;

	_dx12.Device()->CreateShaderResourceView(tmpUIBuff, &srvDesc, _tmpCPUHandle);	//�r���[�쐬

	_GPUHandles[key] = _tmpGPUHandle;												//GPU�n���h�����x�N�g���Ɋi�[

	return result;
}

/// <summary>
/// ��`�̒����ɕ��͂�z�u�ł���悤�ȍ��W���擾����֐�
/// </summary>
/// <param name="rect">��`</param>
/// <param name="wstr">����</param>
/// <param name="rectWidth">��`�̕�</param>
/// <param name="rectHeight">��`�̍���</param>
/// <returns>���W</returns>
XMFLOAT2
SpriteManager::GetCenterPos(RECT rect, const wchar_t* wstr, float rectWidth, float rectHeight)
{
	auto ret = _spriteFont->MeasureString(wstr);				//������̃t�H���g�T�C�Y���x�N�g���̌`�Ŏ擾
	auto width = XMVectorGetX(ret);								//�����擾
	auto height = XMVectorGetY(ret);							//�������擾

	float xPos = (float)rect.left + ((rectWidth - width) / 2);	//���A���������ɒ��������ƂȂ���W���擾����
	float yPos = (float)rect.top - 5;

	return XMFLOAT2(xPos, yPos);
}

/// <summary>
/// �}�E�X���V�U�[��`�̒��ɓ����Ă��邩�`�F�b�N����֐�
/// </summary>
/// <param name="rect">�`�F�b�N��������`</param>
/// <returns>�����Ă��邩�ǂ���</returns>
bool
SpriteManager::IsInRect(RECT rect)
{
	if ((rect.left <= _x && _x <= rect.right) &&	//�}�E�X���W�Ƌ�`�̍��W���r���A���̒��ɓ����Ă��邩�`�F�b�N
		(rect.top <= _y && _y <= rect.bottom))
	{
		return true;
	}

	return false;
}

/// <summary>
/// CPU��GPU�̃n���h�������炷�֐�
/// </summary>
void
SpriteManager::ShiftHandles()
{
	_tmpCPUHandle.ptr += _incrementSize;	//CPU�n���h�����Y����
	_tmpGPUHandle.ptr += _incrementSize;	//GPU�n���h�����Y����
}

/// <summary>
/// ��ʃT�C�Y�ύX���A��`�𒲐�����֐�
/// </summary>
void
SpriteManager::AdjustSpriteRect()
{
	_backGroundRect = { ORIGIN,ORIGIN,_width,_height };												//�w�i�p�摜�̐ݒ�
	_startButtonRect = { _buttonLeft,START_TOP,_buttonRight,START_TOP + BUTTON_HEIGHT };			//�X�^�[�g�{�^���̐ݒ�
	_endButtonRect = { _buttonLeft,END_TOP,_buttonRight,END_TOP + BUTTON_HEIGHT };					//�I���{�^���̐ݒ�
	_loadingRect = { LOADING_WIDTH,LOADIN_HEIGHT,_width - LOADING_WIDTH,_height - LOADIN_HEIGHT };	//���[�h��ʂ̐ݒ�
	_titleRect = { TITLE_DIFF,TITLE_DIFF,_width - TITLE_DIFF,_height - TITLE_DIFF };				//�^�C�g�����j���[�̐ݒ�

	AdjustWindowRect(&_backGroundRect, WS_OVERLAPPEDWINDOW, false);									//�E�B���h�E�T�C�Y���v�Z
	AdjustWindowRect(&_startButtonRect, WS_OVERLAPPEDWINDOW, false);	
	AdjustWindowRect(&_endButtonRect, WS_OVERLAPPEDWINDOW, false);		
	AdjustWindowRect(&_loadingRect, WS_OVERLAPPEDWINDOW, false);	
	AdjustWindowRect(&_titleRect, WS_OVERLAPPEDWINDOW, false);			
}

/// <summary>
/// �^�C�g����ʂł̉摜�EUI��`�悷��֐�
/// </summary>
void
SpriteManager::TitleDraw()
{
	_dx12.CommandList()->SetDescriptorHeaps(1, &_heapForSpriteFont);									//�q�[�v���Z�b�g

	//�`�揈��
	_spriteBatch->Begin(_dx12.CommandList());															//�o�b�`���Z�b�g

	//�{�^���`�揈��
	if (TitleIsOnStart())																				//�X�^�[�g�{�^��
	{
		_spriteBatch->Draw(
			_GPUHandles["button"], XMUINT2(1, 1), _startButtonRect, Colors::MistyRose);
	}
	else
	{
		_spriteBatch->Draw(
			_GPUHandles["button"], XMUINT2(1, 1), _startButtonRect, Colors::White);
	}
	if (TitleIsOnEnd())																					//�I���{�^��
	{
		_spriteBatch->Draw(
			_GPUHandles["button"], XMUINT2(1, 1), _endButtonRect, Colors::MistyRose);
	}
	else
	{
		_spriteBatch->Draw(
			_GPUHandles["button"], XMUINT2(1, 1), _endButtonRect, Colors::White);
	}

	_spriteFont->DrawString(_spriteBatch.get(), L"Game",												//�^�C�g������\��
		GetCenterPos(_titleRect, L"Game", (float)_titleWidth, (float)_titleHeight), Colors::White);

	_spriteFont->DrawString(_spriteBatch.get(), L"Start",												//�{�^����̃e�L�X�g�`��
		GetCenterPos(_startButtonRect, L"Start", (float)BUTTON_WIDTH, (float)BUTTON_HEIGHT),
		Colors::Black);
	_spriteFont->DrawString(_spriteBatch.get(), L"End",
		GetCenterPos(_endButtonRect, L"End", (float)BUTTON_WIDTH, (float)BUTTON_HEIGHT),
		Colors::Black);

	_spriteBatch->End();																				//�o�b�`������
}

/// <summary>
/// �^�C�g����ʂŃX�^�[�g�{�^���̏�Ƀ}�E�X�����邩�`�F�b�N����֐�
/// </summary>
/// <returns>�}�E�X���W���X�^�[�g�{�^����ɂ��邩�ǂ���</returns>
bool
SpriteManager::TitleIsOnStart()
{
	return IsInRect(_startButtonRect);
}

/// <summary>
/// �^�C�g����ʂŏI���{�^���̏�Ƀ}�E�X�����邩�`�F�b�N����֐�
/// </summary>
/// <returns>�}�E�X���W���I���{�^���̏�ɂ��邩�ǂ���</returns>
bool
SpriteManager::TitleIsOnEnd()
{
	return IsInRect(_endButtonRect);
}

/// <summary>
/// �w�i��`�悷��֐�
/// </summary>
void
SpriteManager::BackGroundDraw()
{
	_dx12.CommandList()->SetDescriptorHeaps(1, &_heapForSpriteFont);							//�q�[�v���Z�b�g

	_spriteBatch->Begin(_dx12.CommandList());													//�o�b�`���Z�b�g

	_spriteBatch->Draw(_GPUHandles["back"], XMUINT2(1, 1), _backGroundRect, Colors::White);		//�`�揈��

	_spriteBatch->End();																		//�o�b�`������
}

/// <summary>
/// ���[�f�B���O��ʂł̉摜��`�悷��֐�
/// </summary>
void
SpriteManager::LoadingDraw()
{
	_dx12.CommandList()->SetDescriptorHeaps(1, &_heapForSpriteFont);				//�q�[�v���Z�b�g

	_spriteBatch->Begin(_dx12.CommandList());										//�o�b�`���Z�b�g

	int count = ((clock() / static_cast<int>(60 * 1.4)) % 8) + 1;					//���݂̎��Ԃɉ����ĕ\������摜��ς��A���[�h�A�C�R���̃A�j���[�V������\��
	_spriteBatch->Draw(_GPUHandles["load_" + to_string(count)], XMUINT2(1, 1),
		_loadingRect, Colors::White);

	_spriteBatch->End();															//�o�b�`������
}

/// <summary>
/// �}�E�X�J�[�\���p�̉摜��`�悷��֐�
/// </summary>
void
SpriteManager::CursorDraw()
{
	_dx12.CommandList()->SetDescriptorHeaps(1, &_heapForSpriteFont);	//�q�[�v���Z�b�g

	//���̑�UI�`�揈��
	_spriteBatch->Begin(_dx12.CommandList());							//�o�b�`���Z�b�g

	_spriteBatch->Draw(_GPUHandles["cursor"], XMUINT2(50, 50),			//�J�[�\�����}�E�X���W�ɕ\��
		XMFLOAT2((float)_x, (float)_y), Colors::White);

	_spriteBatch->End();												//�o�b�`������
}

/// <summary>
/// �O���t�B�b�N�X���������R�}���h�L���[�ɃZ�b�g����֐�
/// </summary>
void
SpriteManager::Commit()
{
	_gmemory->Commit(_dx12.CommandQueue());
}

/// <summary>
/// �}�E�X���W���Z�b�g����֐�
/// </summary>
/// <param name="x">X���W</param>
/// <param name="y">y���W</param>
void
SpriteManager::SetMousePosition(int x, int y)
{
	_x = x;
	_y = y;
}
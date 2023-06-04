#include "Functions.h"
#include "Manager/ImGuiManager.h"
#include "Manager/SpriteManager.h"
#include "Renderer/PeraRenderer.h"
#include "Renderer/Renderer.h"
#include "Wrapper/Dx12Wrapper.h"
#include "Wrapper/SphericalCoordinates.h"

const float FADE_TIME = 1.0f;				//�t�F�[�h�C���E�A�E�g�ɂ����鎞��

const XMVECTOR XZ_PLANE = XMLoadFloat3		//���_�������_�̃x�N�g����XZ���ʂɐ������邽�߂̃x�N�g��
(
	new XMFLOAT3
	(
		1.0f,
		0.0f,
		1.0f
	)
);

/// <summary>
/// �f�o�b�O�p���C���[������������֐�
/// </summary>
void EnableDebugLayer()
{
	ID3D12Debug* debugLayer = nullptr;
	auto result = D3D12GetDebugInterface(
		IID_PPV_ARGS(&debugLayer));
		
	debugLayer->EnableDebugLayer();				//�f�o�b�O���C���[��L��������
	debugLayer->Release();						//�L����������C���^�[�t�F�C�X���J������
}

/// <summary>
/// �f�X�g���N�^
/// ���ɏ����͍s��Ȃ�
/// </summary>
Dx12Wrapper::~Dx12Wrapper()
{

}

/// <summary>
/// �f�o�C�X�֘A������������֐�
/// </summary>
/// <returns>�֐��������������ǂ���</returns>
HRESULT
Dx12Wrapper::InitializeDXGIDevice()
{
	//Debug�������łȂ����Ńt�@�N�g���[�̍쐬�֐���ς���
#ifdef _DEBUG
	auto result = CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(_dxgiFactory.ReleaseAndGetAddressOf()));
#else
	auto result = CreateDXGIFactory1(IID_PPV_ARGS(&_dxgiFactory));
#endif // _DEBUG
	
	D3D_FEATURE_LEVEL levels[] = {												//�t�B�[�`���[���x���̔z���������
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
	};

	vector<IDXGIAdapter*> adapters;												//�񋓂����A�_�v�^�[���i�[
	IDXGIAdapter* tmpAdapter = nullptr;											//�A�_�v�^�[

	for (int i = 0;
		_dxgiFactory->EnumAdapters(i, &tmpAdapter) != DXGI_ERROR_NOT_FOUND;++i	//�t�@�N�g���[���̑S�A�_�v�^�[���x�N�g���Ɋi�[
		)
	{
		adapters.push_back(tmpAdapter);
	}

	for (auto adpt : adapters)													//�i�[�����S�A�_�v�^�[�ɑ΂����[�v
	{
		DXGI_ADAPTER_DESC adesc = {};
		adpt->GetDesc(&adesc);													//�A�_�v�^�[�̐������L�q����\���̂��擾

		wstring strDesc = adesc.Description;									//�A�_�v�^�[�̖��O���擾

		if (strDesc.find(L"NVIDIA") != string::npos)							//�A�_�v�^�[�̖��O������̖��O�ƈ�v�����烋�[�v�I��
		{
			tmpAdapter = adpt;													//�f�o�C�X�쐬�Ɏg�p����A�_�v�^�[������
			break;
		}
	}

	for (auto lv : levels)														//�����������t�B�[�`���[���x���ɑ΂����[�v
	{
		if (D3D12CreateDevice(tmpAdapter, lv, IID_PPV_ARGS(&_dev)) == S_OK)		//�f�o�C�X���쐬�ł���t�B�[�`���[���x����T��
		{
			result = S_OK;
			break;																//�����\�ȃo�[�W���������������烋�[�v�I��
		}
	}
	return result;
}

/// <summary>
/// �R�}���h�֘A������������֐�
/// </summary>
/// <returns>�֐��������������ǂ���</returns>
HRESULT
Dx12Wrapper::InitializeCommand()
{
	auto result = _dev->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,	//�R�}���h�A���P�[�^���쐬
		IID_PPV_ARGS(_cmdAllocator.ReleaseAndGetAddressOf()));

	result = _dev->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT,			//�R�}���h���X�g���쐬
		_cmdAllocator.Get(), nullptr,
		IID_PPV_ARGS(_cmdList.ReleaseAndGetAddressOf()));

	D3D12_COMMAND_QUEUE_DESC cmdQueueDesc = {};									//�R�}���h�L���[�ݒ�\���̂��쐬�E�ݒ�
	cmdQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	cmdQueueDesc.NodeMask = 0;
	cmdQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	cmdQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

	result = _dev->CreateCommandQueue(&cmdQueueDesc,							//�R�}���h�L���[���쐬
		IID_PPV_ARGS(_cmdQueue.ReleaseAndGetAddressOf()));

	return result;
}

/// <summary>
/// �X���b�v�`�F�[�����쐬����֐�
/// </summary>
/// <param name="hwnd">�E�B���h�E���ʗp�f�[�^</param>
/// <returns>�֐��������������ǂ���</returns>
HRESULT
Dx12Wrapper::CreateSwapChain(const HWND& hwnd)
{
	DXGI_SWAP_CHAIN_DESC1 swapchainDesc = {};						//�X���b�v�`�F�[���ݒ�p�\���́E�ݒ�
	swapchainDesc.Width = _winSize.cx;
	swapchainDesc.Height = _winSize.cy;
	swapchainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapchainDesc.Stereo = false;
	swapchainDesc.SampleDesc.Count = 1;
	swapchainDesc.SampleDesc.Quality = 0;
	swapchainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapchainDesc.BufferCount = 2;
	swapchainDesc.Scaling = DXGI_SCALING_STRETCH;				
	swapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;	
	swapchainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
	swapchainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	auto result = _dxgiFactory->CreateSwapChainForHwnd(				//�X���b�v�`�F�[���쐬
		_cmdQueue.Get(),
		hwnd,
		&swapchainDesc,
		nullptr,
		nullptr,
		(IDXGISwapChain1**)_swapchain.ReleaseAndGetAddressOf());

	return result;
}

/// <summary>
/// �����_�[�^�[�Q�b�g���쐬����֐�
/// </summary>
/// <returns>�֐��������������ǂ���</returns>
HRESULT 
Dx12Wrapper::CreateRenderTargetsView()
{
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};							//�����_�[�^�[�Q�b�g�p�f�B�X�N���v�^�q�[�v�ݒ�\���̂̍쐬�E�ݒ�
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	heapDesc.NodeMask = 0;
	heapDesc.NumDescriptors = 2;
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	auto result = _dev->CreateDescriptorHeap(							//�����_�[�^�[�Q�b�g�p�f�B�X�N���v�^�q�[�v�̍쐬
		&heapDesc, IID_PPV_ARGS(_rtvHeap.ReleaseAndGetAddressOf()));
	if (FAILED(result))
	{
		assert(0);
		return result;
	}

	DXGI_SWAP_CHAIN_DESC swcDesc = {};									//�X���b�v�`�F�[���̏��擾
	result = _swapchain->GetDesc(&swcDesc);
	if (FAILED(result))
	{
		assert(0);
		return result;
	}

	_backBuffers.resize(swcDesc.BufferCount);							//�o�b�N�o�b�t�@�[�̐����X���b�v�`�F�[���̃o�b�t�@�[���ɍ��킹��

	
	D3D12_CPU_DESCRIPTOR_HANDLE handle =								//�f�B�X�N���v�^�q�[�v�̐擪�A�h���X(�n���h��)
		_rtvHeap->GetCPUDescriptorHandleForHeapStart();

	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};							//�����_�[�^�[�Q�b�g�r���[�ݒ�p�\���̂̍쐬�E�ݒ�
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

	for (int idx = 0; idx < (int)swcDesc.BufferCount; ++idx)			//�e�o�b�N�o�b�t�@�[�ɑ΂����[�v����
	{
		result = _swapchain->GetBuffer(													//�X���b�v�`�F�[������f�[�^���擾�A�o�b�N�o�b�t�@�[�Ɋ��蓖�Ă�
			idx, IID_PPV_ARGS(&_backBuffers[idx]));	
		if (FAILED(result))
		{
			assert(0);
			return result;
		}

		_dev->CreateRenderTargetView(													//�o�b�N�o�b�t�@�[��Ώۂ�RTV���쐬
			_backBuffers[idx],
			&rtvDesc,
			handle);

		handle.ptr +=																	//�n���h���̈ʒu�����炷
			_dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	}

	DXGI_SWAP_CHAIN_DESC1 desc = {};													//�X���b�v�`�F�[�������擾�i��L��swcDesc�Ƃ͕ʁj
	result = _swapchain->GetDesc1(&desc);
	if (FAILED(result))
	{
		assert(0);
		return result;
	}

	_viewPort.reset(new CD3DX12_VIEWPORT(_backBuffers[0]));								//�X���b�v�`�F�[���̏������Ƀr���[�|�[�g��������
	_rect.reset(new CD3DX12_RECT(0, 0, desc.Width, desc.Height));						//�������V�U�[��`��������

	return result;
}

/// <summary>
/// �r���[�v���W�F�N�V�����p�r���[���쐬
/// </summary>
/// <returns>�֐��������������ǂ���</returns>
HRESULT 
Dx12Wrapper::CreateSceneView()
{
	auto sceneResDesc = 
		CD3DX12_RESOURCE_DESC::Buffer((sizeof(SceneData) + _constSize) & ~_constSize);

	_dev->CreateCommittedResource(														//�萔�o�b�t�@�[(���\�[�X)���쐬
		&_uploadHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&sceneResDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&_sceneConstBuff));

	_mappedScene = nullptr;																//�萔�o�b�t�@�[�ɑ΂��������߂�悤�ɂ���
	auto result = _sceneConstBuff->Map(0,nullptr,(void**)&_mappedScene);

	_mappedScene->view = XMMatrixLookAtLH(												//�r���[�E�v���W�F�N�V�����s�����������
		XMLoadFloat3(&_eye),XMLoadFloat3(&_target),XMLoadFloat3(&_up));
	_mappedScene->proj = XMMatrixPerspectiveFovLH(XM_PIDIV4,
		static_cast<float>(_winSize.cx)/ static_cast<float>(_winSize.cy),
		0.1f,
		1000.0f
	);
	_mappedScene->eye = _eye;															//���_���W����������

	D3D12_DESCRIPTOR_HEAP_DESC sceneDescHeapDesc = {};									//�萔�o�b�t�@�[�p�f�B�X�N���v�^�q�[�v�\���̂̍쐬
	sceneDescHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	sceneDescHeapDesc.NodeMask = 0;
	sceneDescHeapDesc.NumDescriptors = 1;
	sceneDescHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;

	result = _dev->CreateDescriptorHeap(												//�f�B�X�N���v�^�q�[�v�쐬
		&sceneDescHeapDesc,IID_PPV_ARGS(_sceneDescHeap.ReleaseAndGetAddressOf()));

	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};										//�萔�o�b�t�@�[�r���[�ݒ�p�\���̂̍쐬�E�ݒ�
	cbvDesc.BufferLocation = _sceneConstBuff->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = (UINT)_sceneConstBuff->GetDesc().Width;
	
	_dev->CreateConstantBufferView(														//�萔�o�b�t�@�[�r���[�̍쐬
		&cbvDesc,
		_sceneDescHeap->GetCPUDescriptorHandleForHeapStart());

	return result;
}

/// <summary>
/// �[�x�X�e���V���r���[���쐬���쐬����֐�
/// </summary>
/// <returns>�֐��������������ǂ���</returns>
HRESULT
Dx12Wrapper::CreateDepthStencilView()
{
	D3D12_HEAP_PROPERTIES depthHeapProp = {};											//�[�x�X�e���V���o�b�t�@�[�p�q�[�v�v���p�e�B���쐬
	depthHeapProp.Type = D3D12_HEAP_TYPE_DEFAULT;
	depthHeapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	depthHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

	D3D12_RESOURCE_DESC depthResDesc = {};												//�o�b�t�@�[�p���\�[�X�f�B�X�N���쐬�E�ݒ�
	depthResDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	depthResDesc.Width = _winSize.cx;
	depthResDesc.Height = _winSize.cy;
	depthResDesc.DepthOrArraySize = 1;
	depthResDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthResDesc.SampleDesc.Count = 1;
	depthResDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_CLEAR_VALUE depthClearValue = {};												//�N���A�o�����[�̍쐬�E�ݒ�
	depthClearValue.DepthStencil.Depth = 1.0f;
	depthClearValue.Format = DXGI_FORMAT_D32_FLOAT;

	auto result = _dev->CreateCommittedResource(										//�o�b�t�@�[�̍쐬
		&depthHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&depthResDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&depthClearValue,
		IID_PPV_ARGS(_depthBuffer.ReleaseAndGetAddressOf()));

	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};										//�o�b�t�@�[�p�f�B�X�N���v�^�q�[�v�ݒ�p�\���̂̍쐬
	dsvHeapDesc.NumDescriptors = 1;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;

	result = _dev->CreateDescriptorHeap(&dsvHeapDesc,IID_PPV_ARGS(&_dsvHeap));			//�f�B�X�N���v�^�q�[�v�̍쐬

	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};											//�o�b�t�@�[�r���[�ݒ�p�\���̂̍쐬�E�ݒ�
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Flags = D3D12_DSV_FLAG_NONE;

	
	_dev->CreateDepthStencilView(														//�r���[�̍쐬
		_depthBuffer.Get(),&dsvDesc,_dsvHeap->GetCPUDescriptorHandleForHeapStart());

	return result;
}

/// <summary>
/// �G�t�F�N�g�K�p�����߂�f�[�^�p�̃q�[�v�E�o�b�t�@�[���쐬����֐�
/// </summary>
/// <returns>�֐��������������ǂ���</returns>
HRESULT
Dx12Wrapper::CreateFactorBufferAndView()
{
	auto factorResDesc
		= CD3DX12_RESOURCE_DESC::Buffer((sizeof(Factor) + _constSize) & ~_constSize);	//���\�[�X�f�B�X�N�쐬

	auto result = _dev->CreateCommittedResource(										//�o�b�t�@�[�쐬
		&_writeHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&factorResDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(_factorConstBuff.ReleaseAndGetAddressOf()));
	if (FAILED(result))
	{
		assert(0);
		return result;
	}

	result = _factorConstBuff->Map(0, nullptr, (void**)&_mappedFactor);					//�o�b�t�@�[�֏������߂�悤�ɂ���
	if (FAILED(result))
	{
		assert(0);
		return result;
	}
	UpdateFade();

	D3D12_DESCRIPTOR_HEAP_DESC factorHeapDesc = {};										//�f�B�X�N���v�^�q�[�v�ݒ�p�\���̂̍쐬
	factorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	factorHeapDesc.NodeMask = 0;
	factorHeapDesc.NumDescriptors = 1;
	factorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;

	result = _dev->CreateDescriptorHeap(
		&factorHeapDesc, IID_PPV_ARGS(_factorCBVHeap.ReleaseAndGetAddressOf()));		//�f�B�X�N���v�^�q�[�v�쐬
	if (FAILED(result))
	{
		assert(0);
		return result;
	}

	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};										//�萔�o�b�t�@�[�r���[�ݒ�p�\���̂̍쐬�E�ݒ�
	cbvDesc.BufferLocation = _factorConstBuff->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = (UINT)_factorConstBuff->GetDesc().Width;

	_dev->CreateConstantBufferView(														//�萔�o�b�t�@�[�r���[�̍쐬
		&cbvDesc,
		_factorCBVHeap->GetCPUDescriptorHandleForHeapStart());

	return result;
}

/// <summary>
/// �e�N�X�`�����[�h�p�e�[�u�����쐬����֐�
/// �t�@�C�����Ɋ܂܂�Ă���g���q�ɉ����Ď��s����֐���ς���
/// </summary>
void
Dx12Wrapper::CreateTextureLoaderTable()
{
	_loadLambdaTable["sph"]													//sph,spa,png,jpg�g���q��key�ɁALoadFromWICFile�֐���value�ɂ���
		= _loadLambdaTable["spa"]
		= _loadLambdaTable["bmp"]
		= _loadLambdaTable["png"]
		= _loadLambdaTable["jpg"]
		= [](const wstring& path, TexMetadata* meta, ScratchImage& img)
		->HRESULT
	{
		return LoadFromWICFile(path.c_str(), 0, meta, img);
	};

	_loadLambdaTable["tga"]													//tga�g���q��key�ɁALoadFromTGAFile�֐���value�ɂ���
		= [](const wstring& path, TexMetadata* meta, ScratchImage& img)
		->HRESULT
	{
		return LoadFromTGAFile(path.c_str(), meta, img);
	};

	_loadLambdaTable["dds"]													//dds�g���q��key�ɁALoadFromDDSFile�֐���value�ɂ���
		= [](const wstring& path, TexMetadata* meta, ScratchImage& img)
		->HRESULT
	{
		return LoadFromDDSFile(path.c_str(), 0, meta, img);
	};
}

/// <summary>
/// ���_�A�����_�̃x�N�g��������������֐�
/// </summary>
void
Dx12Wrapper::ResetVector()
{
	_eye = _initialEye;
	_target = _initialTarget;
}

/// <summary>
/// �R���X�g���N�^
/// </summary>
/// <param name="hwnd">�E�B���h�E�n���h��</param>
/// <param name="deltaTime">1�t���[��������̕b��</param>
Dx12Wrapper::Dx12Wrapper(HWND hwnd, float deltaTime) :
	_initialEye(0, 50, 100), _initialTarget(0, 50, 0), _up(0, 1, 0), _deltaTime(deltaTime),
	_fade(0.0f)
{
#ifdef _DEBUG
	EnableDebugLayer();													//�f�o�b�O�p���C���[���N��
#endif 
	auto& app = Application::Instance();								//Application�C���X�^���X���擾
	_winSize = app.GetWindowSize();										//�E�B���h�E�̃T�C�Y���擾
	_rate = app.GetRate();												//�t���[�����[�g���擾

	InitializeDXGIDevice();												//�f�o�C�X�֘A��������

	InitializeCommand();												//�R�}���h�֘A��������

	CreateSwapChain(hwnd);												//�X���b�v�`�F�[�����쐬

	CreateRenderTargetsView();											//�����_�[�^�[�Q�b�g���쐬

	ResetVector();														//���_�A�����_�̃x�N�g����������

	CreateSceneView();													//�r���[�v���W�F�N�V�����p�r���[���쐬

	CreateTextureLoaderTable();											//�e�N�X�`�����[�h�p�e�[�u�����쐬

	CreateDepthStencilView();											//�[�x�X�e���V���r���[���쐬

	CreateFactorBufferAndView();										//�t�F�[�h�C���^�A�E�g�p�o�b�t�@�[�E�r���[���쐬

	CreateDescriptorHeapForImgui();										//ImGui�p�q�[�v���쐬

	_dev->CreateFence(													//�t�F���X���쐬
		_fenceVal, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&_fence));

	_sprite.reset(new SpriteManager(*this,_winSize.cx,_winSize.cy));	//SpriteManager�C���X�^���X��������

	_pera.reset(new PeraRenderer(*this));								//PeraRenderer�C���X�^���X��������

	_render.reset(new Renderer(*this));									//Renderer�C���X�^���X��������

	_imgui.reset(new ImGuiManager(*this, hwnd));						//ImGuiManager�C���X�^���X��������
}


/// <summary>
/// ���\�[�X��J�ڂ�����֐�
/// </summary>
/// <param name="resource">�J�ڂ����������\�[�X</param>
/// <param name="before">�J�ڑO�̃X�e�[�g</param>
/// <param name="after">�J�ڌ�̃X�e�[�g</param>
void
Dx12Wrapper::BarrierTransition(
	ID3D12Resource* resource,
	D3D12_RESOURCE_STATES before,
	D3D12_RESOURCE_STATES after)
{
	_barrier = CD3DX12_RESOURCE_BARRIER::Transition(resource, before, after);	//�ϐ���ݒ�

	//���\�[�X��J�ڂ�����
	_cmdList->ResourceBarrier(1, &_barrier);
}

/// <summary>
/// XZ���ʂɐ����������_���W���璍���_���W�ւ̃x�N�g�����擾����֐�
/// </summary>
/// <returns>�����t�����_���W���璍���_���W�ւ̃x�N�g��</returns>
XMVECTOR
Dx12Wrapper::GetXZVecEyeToTarget()const
{
	XMVECTOR vec = XMLoadFloat3					//�܂����_���W���璍���_���W�ւ̃x�N�g�����擾
	(
		new XMFLOAT3
		(
			_target.x-_eye.x,
			_target.y - _eye.y, 
			_target.z - _eye.z
		)
	);

	vec = XMVectorMultiply(vec, XZ_PLANE);		//Y������0�ɂ��A�x�N�g����XZ���ʂɐ���

	XMVECTOR ret = XMVector3Normalize(vec);		//�x�N�g���𐳋K��
	
	return ret;
}

/// <summary>
/// ���_�������_�̃x�N�g���ɑ΂��E�ɐ����ȃx�N�g����Ԃ��֐�
/// </summary>
/// <returns>�����x�N�g��</returns>
XMVECTOR
Dx12Wrapper::GetRightVector()const
{
	XMVECTOR vec = GetXZVecEyeToTarget();								//���_�������_�̃x�N�g�����擾
	float xyz[] = { vec.m128_f32[0],vec.m128_f32[1],vec.m128_f32[2] };	//XYZ�������擾
	
	XMVECTOR cross = XMLoadFloat3(										//�O�ρi2�̃x�N�g�����琬�镽�ʂɑ΂������ȃx�N�g���j���擾
		new XMFLOAT3
		(
			(xyz[1] * _up.z) - xyz[2] * _up.y,
			(xyz[2] * _up.x) - xyz[0] * _up.z,
			(xyz[0] * _up.y) - xyz[1] * _up.x
		)
	);

	return cross;
}

/// <summary>
/// �t���[���Ԃ̌o�ߎ��Ԃ�Ԃ��֐�
/// </summary>
/// <returns>�t���[���Ԃ̌o�ߎ���</returns>
float 
Dx12Wrapper::GetDeltaTime()const
{
	return _deltaTime;
}

/// <summary>
/// �J�������ߕt����E��������֐�
/// </summary>
/// <param name="x">�ړ�����</param>
void
Dx12Wrapper::ScalingSphericalCoordinates(int x)
{
	_eye = XMFLOAT3
	(
		_coordinates->Scaling(x * _deltaTime).ToCartesian().x + _target.x,
		_coordinates->Scaling(x * _deltaTime).ToCartesian().y + _target.y,
		_coordinates->Scaling(x * _deltaTime).ToCartesian().z + _target.z
	);
}

/// <summary>
/// �J�����𕽍s�ړ�������֐�
/// </summary>
/// <param name="dir">���s�ړ����������</param>
/// <param name="value">�ړ�����</param>
void
Dx12Wrapper::TranslateSphericalCoordinates(Direction dir, float value)
{
	switch (dir)
	{
	case Horizontal:
		_target.x += GetRightVector().m128_f32[0] * value * _deltaTime;
		_target.z += GetRightVector().m128_f32[2] * value * _deltaTime;
		break;
	case Vertical:
		_target.y += value * _deltaTime;
		break;
	case Depth:
		_target.x += GetXZVecEyeToTarget().m128_f32[0] * value * _deltaTime;
		_target.z += GetXZVecEyeToTarget().m128_f32[2] * value * _deltaTime;
		break;
	default:
		break;
	}


	//���̌�J�������W���X�V
	_eye =
		XMFLOAT3
		(
			_coordinates->ToCartesian().x + _target.x,
			_coordinates->ToCartesian().y + _target.y,
			_coordinates->ToCartesian().z + _target.z
		);
}

/// <summary>
/// SphericalCoordinates��̃J�������W����]������֐�
/// </summary>
/// <param name="deg">���ʊp���p�����߂�</param>
/// <param name="value">�ォ�����A�E������</param>
void
Dx12Wrapper::RotateSphericalCoordinates(Degree deg, float value)
{
	switch (deg)
	{
	case Degree::Azimth:	//���ʊp�̕����ɉ�]������
		_eye = XMFLOAT3
		(
			_coordinates->Rotate(value * _deltaTime, 0.0f).ToCartesian().x + _target.x,
			_coordinates->Rotate(value * _deltaTime, 0.0f).ToCartesian().y + _target.y,
			_coordinates->Rotate(value * _deltaTime, 0.0f).ToCartesian().z + _target.z
		);
		break;
	case Degree::Elevation:	//�p�̕����ɉ�]������
		_eye = XMFLOAT3
		(
			_coordinates->Rotate(0.0f, value * _deltaTime).ToCartesian().x + _target.x,
			_coordinates->Rotate(0.0f, value * _deltaTime).ToCartesian().y + _target.y,
			_coordinates->Rotate(0.0f, value * _deltaTime).ToCartesian().z + _target.z
		);
		break;
	default:
		break;
	}
}

/// <summary>
/// �J�����̈ʒu������������֐�
/// </summary>
void
Dx12Wrapper::ResetSphericalCoordinates()
{
	ResetVector();									//���_�A�����_�����Z�b�g

	_coordinates.reset(new SphericalCoordinates());	//���ʍ��W������SphericalCoordinates�C���X�^���X���쐬
	_coordinates->SetRadius(300.0f);				//���a��500�ŌŒ�
	_coordinates->SetAzimth(0);						//���ʊp��0�x�ŏ�����
	_coordinates->SetElevation(0);					//�p��0�x�ŏ�����

	_eye = XMFLOAT3									//���_���W��ݒ�
	(
		_coordinates->ToCartesian().x,
		_coordinates->ToCartesian().y,
		_coordinates->ToCartesian().z
	);
}

/// <summary>
/// �r���[�v���W�F�N�V�����p�r���[���Z�b�g����֐�
/// </summary>
void
Dx12Wrapper::SetScene()
{
	_mappedScene->view = XMMatrixLookAtLH(										//�s��E���W����������
		XMLoadFloat3(&_eye), XMLoadFloat3(&_target), XMLoadFloat3(&_up));
	_mappedScene->proj = XMMatrixPerspectiveFovLH(XM_PIDIV4,
		static_cast<float>(_winSize.cx) / static_cast<float>(_winSize.cy),
		0.1f,
		1000.0f
	);
	_mappedScene->eye = _eye;

	ID3D12DescriptorHeap* sceneHeaps[] = { _sceneDescHeap.Get() };				//�f�B�X�N���v�^�q�[�v���R�}���h���X�g�ɃZ�b�g
	_cmdList->SetDescriptorHeaps(1, sceneHeaps);

	_cmdList->SetGraphicsRootDescriptorTable(									//�q�[�v�̃n���h�������[�g�p�����[�^�Ɗ֘A�t��
		0, _sceneDescHeap->GetGPUDescriptorHandleForHeapStart());
}

/// <summary>
/// �Q�[����ʗp���\�[�X�̑J��(PRESENT��RENDER_TARGET)�ERTV�̃Z�b�g�����s����֐�
/// </summary>
void
Dx12Wrapper::BeginGameDraw()
{
	auto bbIdx = _swapchain->GetCurrentBackBufferIndex();									//���݂̃o�b�N�o�b�t�@�[�̃C���f�b�N�X���擾

	BarrierTransition(																		//�Q�[����ʗp���\�[�X��RENDER_TARGET�ɑJ��
		_backBuffers[bbIdx], 
		D3D12_RESOURCE_STATE_PRESENT,
		D3D12_RESOURCE_STATE_RENDER_TARGET);

	auto rtvH = _rtvHeap->GetCPUDescriptorHandleForHeapStart();								//�C���f�b�N�X�ɉ�����RTV�q�[�v�̃n���h�����Y����
	rtvH.ptr += 
		bbIdx * _dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	auto dsvH = _dsvHeap->GetCPUDescriptorHandleForHeapStart();

	_cmdList->OMSetRenderTargets(1, &rtvH, true, &dsvH);									//RTV�EDSV�q�[�v���Z�b�g

	float clearColor[4] = { 1.0f,1.0f,1.0f,1.0f };											//�w�i�F��ݒ�

	_cmdList->ClearRenderTargetView(rtvH, clearColor, 0, nullptr);							//RTV��������
	_cmdList->ClearDepthStencilView(dsvH, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);		//DSV��������
}

/// <summary>
/// �Q�[���p���\�[�X�̑J��(RENDER_TARGET��STATE_PRESENT)�����s����֐�
/// </summary>
void
Dx12Wrapper::EndGameDraw()
{
	auto bbIdx = _swapchain->GetCurrentBackBufferIndex();		//���݂̃o�b�N�o�b�t�@�[�̃C���f�b�N�X���擾

	BarrierTransition(											//�Q�[����ʗp���\�[�X��PRESENT�ɑJ��
		_backBuffers[bbIdx], 
		D3D12_RESOURCE_STATE_RENDER_TARGET, 
		D3D12_RESOURCE_STATE_PRESENT);

	_cmdList->Close();											//�R�}���h���X�g�̃N���[�Y

	ID3D12CommandList* cmdlists[] = { _cmdList.Get() };			//�R�}���h���X�g�̎��s
	_cmdQueue->ExecuteCommandLists(1, cmdlists);

	WaitForCommandQueue();										//�����̓����҂����s��

	_cmdAllocator->Reset();										//�L���[�̃N���A
	_cmdList->Reset(_cmdAllocator.Get(), nullptr);				//�ēx�R�}���h���X�g�𒙂߂鏀��
}
/// <summary>
/// �����̓����҂����s���֐�
/// </summary>
void
Dx12Wrapper::WaitForCommandQueue()
{
	_cmdQueue->Signal(_fence.Get(), ++_fenceVal);					//�t�F���X�l���X�V

	if (_fence->GetCompletedValue() != _fenceVal)					//CPU��GPU�̃t�F���X�l���r���A��v����܂ŏ�����҂����킹��
	{
		auto event = CreateEvent(nullptr, false, false, nullptr);	//��̃C�x���g���擾�H

		_fence->SetEventOnCompletion(_fenceVal, event);				//�t�F���X�l��_fenceVal�ɂȂ������C�x���g��ʒm

		WaitForSingleObject(event, INFINITE);						//�C�x���g����������܂ő҂�������

		CloseHandle(event);											//�C�x���g�����
	}
}

/// <summary>
/// �e�N�X�`����ǂݍ��ފ֐�
/// </summary>
/// <param name="texPath">�e�N�X�`���t�@�C����</param>
/// <returns>�e�N�X�`��(�V�F�[�_�[���\�[�X)�o�b�t�@�[</returns>
ID3D12Resource*
Dx12Wrapper::LoadTextureFromFile(const char* texPath)
{
	auto it = _resourceTable.find(texPath);									//���\�[�X���ǂݍ��܂�Ă��邩�m�F���A����ΕԂ�
	if (it != _resourceTable.end())
	{
		return _resourceTable[texPath];
	}

	string strTexPath = texPath;											//�t�@�C�������R�s�[

	TexMetadata metaData = {};												//���^�f�[�^(�摜�t�@�C���Ɋ֐����)
	ScratchImage scratchImg = {};											//���ۂ̃e�N�X�`���f�[�^������I�u�W�F�N�g

	auto wtexPath = ToWideString(strTexPath);								//�t�@�C������wstring�^�ɕϊ�
	auto ext = FileExtension(strTexPath);									//�t�@�C�����̊g���q���擾

	auto result = _loadLambdaTable[ext](wtexPath,							//�g���q���m�F���A�Ή�����֐���p���ēǂݍ���
		&metaData,
		scratchImg);
	if (FAILED(result))
	{
		assert(0);
		return nullptr;
	}

	auto img = scratchImg.GetImage(0, 0, 0);								//�e�N�X�`���̐��f�[�^�擾

	D3D12_HEAP_PROPERTIES texHeapProp = {};									//�q�[�v�v���p�e�B�쐬�E�ݒ�
	texHeapProp.Type = D3D12_HEAP_TYPE_CUSTOM;
	texHeapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	texHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;
	texHeapProp.CreationNodeMask = 0;
	texHeapProp.VisibleNodeMask = 0;

	D3D12_RESOURCE_DESC texResDesc = {};									//�o�b�t�@�[�p���\�[�X�f�B�X�N�\���̍쐬�E�ݒ�
	texResDesc.Format = metaData.format;
	texResDesc.Width = metaData.width;
	texResDesc.Height = (UINT)metaData.height;
	texResDesc.DepthOrArraySize = (UINT16)metaData.arraySize;
	texResDesc.SampleDesc.Count = 1;
	texResDesc.SampleDesc.Quality = 0;
	texResDesc.MipLevels = (UINT16)metaData.mipLevels;
	texResDesc.Dimension = 
		static_cast<D3D12_RESOURCE_DIMENSION>(metaData.dimension);
	texResDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	texResDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

	ID3D12Resource* texBuff = nullptr;										//���\�[�X�쐬
	result = _dev->CreateCommittedResource(
		&texHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&texResDesc,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		nullptr,
		IID_PPV_ARGS(&texBuff));
	if (FAILED(result))
	{
		return nullptr;
	}

	result = texBuff->WriteToSubresource(									//�쐬�������\�[�X�Ƀe�N�X�`������������
		0,
		nullptr,
		img->pixels,
		(UINT)img->rowPitch,
		(UINT)img->slicePitch
	);
	if (FAILED(result))
	{
		return nullptr;
	}

	_resourceTable[texPath] = texBuff;										//�e�N�X�`���p�A�z�z��Ƀt�@�C�����Ƃ���Ɋ֘A�����e�N�X�`����o�^

	return texBuff;
}

/// <summary>
/// //DXTK12�p�̃q�[�v���쐬����֐�
/// </summary>
/// <returns>�q�[�v</returns>
ID3D12DescriptorHeap*
Dx12Wrapper::CreateDescriptorHeapForSpriteFont()
{
	ID3D12DescriptorHeap* ret = nullptr;
	D3D12_DESCRIPTOR_HEAP_DESC desc = {};
	desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	desc.NodeMask = 1;
	desc.NumDescriptors = 256;
	desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	_dev->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&ret));

	return ret;
}

/// <summary>
/// imgui�p�̃q�[�v���쐬����֐�
/// </summary>
/// <returns>�����������������ǂ���</returns>
HRESULT
Dx12Wrapper::CreateDescriptorHeapForImgui()
{
	D3D12_DESCRIPTOR_HEAP_DESC desc = {};								//�\���̂̐ݒ�
	desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	desc.NodeMask = 0;
	desc.NumDescriptors = 1;
	desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;

	auto result = _dev->CreateDescriptorHeap(							//�q�[�v�̍쐬
		&desc, IID_PPV_ARGS(_heapForImgui.ReleaseAndGetAddressOf()));

	return result;
}

/// <summary>
/// ���[�g�V�O�l�`���E�p�C�v���C���E�`����@���Z�b�g����֐�
/// </summary>
/// <param name="rootSignature">���[�g�V�O�l�`��</param>
/// <param name="pipeline">�p�C�v���C��</param>
/// <param name="topology">�`����@</param>
void
Dx12Wrapper::SetPipelines(ID3D12RootSignature* rootSignature, ID3D12PipelineState* pipeline, D3D12_PRIMITIVE_TOPOLOGY topology)
{
	_cmdList->SetGraphicsRootSignature(rootSignature);	//���[�g�V�O�l�`�����Z�b�g
	_cmdList->SetPipelineState(pipeline);				//�p�C�v���C�����Z�b�g
	_cmdList->IASetPrimitiveTopology(topology);			//�`����@��ݒ�
}

/// <summary>
/// �t�F�[�h�C���^�A�E�g�f�[�^���V�F�[�_�[�ɔ��f����֐�
/// </summary>
void
Dx12Wrapper::UpdateFade()
{
	_mappedFactor->fade = clamp(_fade, 0.0f, 1.0f);		//���l��0�ȏ�1�ȉ��ɐ���
}

/// <summary>
/// �t�F�[�h�C���^�A�E�g�����s����֐�
/// </summary>
/// <param name="start">�t�F�[�h�l�̏����l</param>
/// <param name="end">�t�F�[�h�l�̍ŏI�l</param>
/// <param name="func">�t�F�[�h�������Ɏ��s����������</param>
void
Dx12Wrapper::Fade(float start, float end)
{
	_mtx.lock();

	float t = 0.0f;							//����
	_start = start;							//�t�F�[�h�l�̏����l
	_end = end;								//�t�F�[�h�l�̍ŏI�l

	_rate *= FADE_TIME;

	//���`��ԏ���
	for (int i = 0; i <= _rate; i++)
	{
		_fade = lerp(_start, _end, t);		//�t�F�[�h�l����`���

		t += _deltaTime / FADE_TIME;		//���Z

		Sleep(1);							//������҂�
	}

	_mtx.unlock();
}

/// <summary>
/// PeraRenderer�C���X�^���X��Ԃ��֐�
/// </summary>
/// <returns>�C���X�^���X�̃|�C���^</returns>
PeraRenderer*
Dx12Wrapper::Pera()const
{
	return _pera.get();
}

/// <summary>
/// ImGuiManager�C���X�^���X��Ԃ��֐�
/// </summary>
/// <returns>�C���X�^���X�̃|�C���^</returns>
ImGuiManager*
Dx12Wrapper::ImGui()const
{
	return _imgui.get();
}

/// <summary>
/// imgui�p�q�[�v��Ԃ��֐�
/// </summary>
/// <returns>�q�[�v�̃|�C���^</returns>
ID3D12DescriptorHeap*
Dx12Wrapper::HeapForImgui()const
{
	return _heapForImgui.Get();
}

/// <summary>
/// Renderer�C���X�^���X��Ԃ��֐�
/// </summary>
/// <returns>�C���X�^���X�̃|�C���^</returns>
Renderer*
Dx12Wrapper::Render()
{
	return _render.get();
}

/// <summary>
/// SpriteManager�C���X�^���X��Ԃ��֐�
/// </summary>
/// <returns>�C���X�^���X�̃|�C���^</returns>
SpriteManager*
Dx12Wrapper::Sprite()const
{
	return _sprite.get();
}

/// <summary>
/// �f�o�C�X��Ԃ��֐�
/// </summary>
/// <returns>�f�o�C�X�̃|�C���^</returns>
ID3D12Device*
Dx12Wrapper::Device()const
{
	return _dev.Get();
}

/// <summary>
/// �X���b�v�`�F�[����Ԃ��֐�
/// </summary>
/// <returns>�X���b�v�`�F�[���̃|�C���^</returns>
IDXGISwapChain4*
Dx12Wrapper::Swapchain()const
{
	return _swapchain.Get();
}

/// <summary>
/// �R�}���h���X�g��Ԃ��֐�
/// </summary>
/// <returns>�R�}���h���X�g�̃|�C���^</returns>
ID3D12GraphicsCommandList*
Dx12Wrapper::CommandList()const
{
	return _cmdList.Get();
}

/// <summary>
/// �R�}���h�L���[��Ԃ��֐�
/// </summary>
/// <returns>�R�}���h�L���[�̃|�C���^</returns>
ID3D12CommandQueue*
Dx12Wrapper::CommandQueue()const
{
	return _cmdQueue.Get();
}

/// <summary>
/// �o�b�N�o�b�t�@�[�i1���ځj��Ԃ��֐�
/// </summary>
/// <returns></returns>
ID3D12Resource*
Dx12Wrapper::BackBuffer() const
{
	return _backBuffers[0];
}

/// <summary>
/// RTV�q�[�v��Ԃ��֐�
/// </summary>
/// <returns>RTV�q�[�v</returns>
ID3D12DescriptorHeap*
Dx12Wrapper::RTVHeap() const
{
	return _rtvHeap.Get();
}

/// <summary>
/// �[�x�X�e���V���q�[�v��Ԃ��֐�
/// </summary>
/// <returns>�[�x�X�e���V���q�[�v</returns>
ID3D12DescriptorHeap*
Dx12Wrapper::DSVHeap() const
{
	return _dsvHeap.Get();
}

/// <summary>
/// �����p�q�[�v��Ԃ��֐�
/// </summary>
/// <returns>�����p�q�[�v</returns>
ID3D12DescriptorHeap* 
Dx12Wrapper::FactorCBVHeap() const
{
	return _factorCBVHeap.Get();
}

/// <summary>
/// �r���[�|�[�g��Ԃ��֐�
/// </summary>
/// <returns>�r���[�|�[�g</returns>
D3D12_VIEWPORT*
Dx12Wrapper::ViewPort() const
{
	return _viewPort.get();
}

/// <summary>
/// �V�U�[��`��Ԃ��֐�
/// </summary>
/// <returns>�V�U�[��`</returns>
D3D12_RECT*
Dx12Wrapper::Rect() const
{
	return _rect.get();
}
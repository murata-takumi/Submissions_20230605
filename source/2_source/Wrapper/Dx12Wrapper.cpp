#include "Functions.h"
#include "Manager/ImGuiManager.h"
#include "Manager/SpriteManager.h"
#include "Renderer/PeraRenderer.h"
#include "Renderer/Renderer.h"
#include "Wrapper/Dx12Wrapper.h"
#include "Wrapper/SphericalCoordinates.h"

const float FADE_TIME = 1.0f;				//フェードイン・アウトにかける時間

const XMVECTOR XZ_PLANE = XMLoadFloat3		//視点→注視点のベクトルをXZ平面に制限するためのベクトル
(
	new XMFLOAT3
	(
		1.0f,
		0.0f,
		1.0f
	)
);

/// <summary>
/// デバッグ用レイヤーを初期化する関数
/// </summary>
void EnableDebugLayer()
{
	ID3D12Debug* debugLayer = nullptr;
	auto result = D3D12GetDebugInterface(
		IID_PPV_ARGS(&debugLayer));
		
	debugLayer->EnableDebugLayer();				//デバッグレイヤーを有効化する
	debugLayer->Release();						//有効化したらインターフェイスを開放する
}

/// <summary>
/// デストラクタ
/// 特に処理は行わない
/// </summary>
Dx12Wrapper::~Dx12Wrapper()
{

}

/// <summary>
/// デバイス関連を初期化する関数
/// </summary>
/// <returns>関数が成功したかどうか</returns>
HRESULT
Dx12Wrapper::InitializeDXGIDevice()
{
	//Debugかそうでないかでファクトリーの作成関数を変える
#ifdef _DEBUG
	auto result = CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(_dxgiFactory.ReleaseAndGetAddressOf()));
#else
	auto result = CreateDXGIFactory1(IID_PPV_ARGS(&_dxgiFactory));
#endif // _DEBUG
	
	D3D_FEATURE_LEVEL levels[] = {												//フィーチャーレベルの配列を初期化
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
	};

	vector<IDXGIAdapter*> adapters;												//列挙したアダプターを格納
	IDXGIAdapter* tmpAdapter = nullptr;											//アダプター

	for (int i = 0;
		_dxgiFactory->EnumAdapters(i, &tmpAdapter) != DXGI_ERROR_NOT_FOUND;++i	//ファクトリー内の全アダプターをベクトルに格納
		)
	{
		adapters.push_back(tmpAdapter);
	}

	for (auto adpt : adapters)													//格納した全アダプターに対しループ
	{
		DXGI_ADAPTER_DESC adesc = {};
		adpt->GetDesc(&adesc);													//アダプターの説明を記述する構造体を取得

		wstring strDesc = adesc.Description;									//アダプターの名前を取得

		if (strDesc.find(L"NVIDIA") != string::npos)							//アダプターの名前が特定の名前と一致したらループ終了
		{
			tmpAdapter = adpt;													//デバイス作成に使用するアダプターを決定
			break;
		}
	}

	for (auto lv : levels)														//初期化したフィーチャーレベルに対しループ
	{
		if (D3D12CreateDevice(tmpAdapter, lv, IID_PPV_ARGS(&_dev)) == S_OK)		//デバイスを作成できるフィーチャーレベルを探す
		{
			result = S_OK;
			break;																//生成可能なバージョンが見つかったらループ終了
		}
	}
	return result;
}

/// <summary>
/// コマンド関連を初期化する関数
/// </summary>
/// <returns>関数が成功したかどうか</returns>
HRESULT
Dx12Wrapper::InitializeCommand()
{
	auto result = _dev->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,	//コマンドアロケータを作成
		IID_PPV_ARGS(_cmdAllocator.ReleaseAndGetAddressOf()));

	result = _dev->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT,			//コマンドリストを作成
		_cmdAllocator.Get(), nullptr,
		IID_PPV_ARGS(_cmdList.ReleaseAndGetAddressOf()));

	D3D12_COMMAND_QUEUE_DESC cmdQueueDesc = {};									//コマンドキュー設定構造体を作成・設定
	cmdQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	cmdQueueDesc.NodeMask = 0;
	cmdQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	cmdQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

	result = _dev->CreateCommandQueue(&cmdQueueDesc,							//コマンドキューを作成
		IID_PPV_ARGS(_cmdQueue.ReleaseAndGetAddressOf()));

	return result;
}

/// <summary>
/// スワップチェーンを作成する関数
/// </summary>
/// <param name="hwnd">ウィンドウ識別用データ</param>
/// <returns>関数が成功したかどうか</returns>
HRESULT
Dx12Wrapper::CreateSwapChain(const HWND& hwnd)
{
	DXGI_SWAP_CHAIN_DESC1 swapchainDesc = {};						//スワップチェーン設定用構造体・設定
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

	auto result = _dxgiFactory->CreateSwapChainForHwnd(				//スワップチェーン作成
		_cmdQueue.Get(),
		hwnd,
		&swapchainDesc,
		nullptr,
		nullptr,
		(IDXGISwapChain1**)_swapchain.ReleaseAndGetAddressOf());

	return result;
}

/// <summary>
/// レンダーターゲットを作成する関数
/// </summary>
/// <returns>関数が成功したかどうか</returns>
HRESULT 
Dx12Wrapper::CreateRenderTargetsView()
{
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};							//レンダーターゲット用ディスクリプタヒープ設定構造体の作成・設定
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	heapDesc.NodeMask = 0;
	heapDesc.NumDescriptors = 2;
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	auto result = _dev->CreateDescriptorHeap(							//レンダーターゲット用ディスクリプタヒープの作成
		&heapDesc, IID_PPV_ARGS(_rtvHeap.ReleaseAndGetAddressOf()));
	if (FAILED(result))
	{
		assert(0);
		return result;
	}

	DXGI_SWAP_CHAIN_DESC swcDesc = {};									//スワップチェーンの情報取得
	result = _swapchain->GetDesc(&swcDesc);
	if (FAILED(result))
	{
		assert(0);
		return result;
	}

	_backBuffers.resize(swcDesc.BufferCount);							//バックバッファーの数をスワップチェーンのバッファー数に合わせる

	
	D3D12_CPU_DESCRIPTOR_HANDLE handle =								//ディスクリプタヒープの先頭アドレス(ハンドル)
		_rtvHeap->GetCPUDescriptorHandleForHeapStart();

	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};							//レンダーターゲットビュー設定用構造体の作成・設定
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

	for (int idx = 0; idx < (int)swcDesc.BufferCount; ++idx)			//各バックバッファーに対しループを回す
	{
		result = _swapchain->GetBuffer(													//スワップチェーンからデータを取得、バックバッファーに割り当てる
			idx, IID_PPV_ARGS(&_backBuffers[idx]));	
		if (FAILED(result))
		{
			assert(0);
			return result;
		}

		_dev->CreateRenderTargetView(													//バックバッファーを対象にRTVを作成
			_backBuffers[idx],
			&rtvDesc,
			handle);

		handle.ptr +=																	//ハンドルの位置をずらす
			_dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	}

	DXGI_SWAP_CHAIN_DESC1 desc = {};													//スワップチェーン情報を取得（上記のswcDescとは別）
	result = _swapchain->GetDesc1(&desc);
	if (FAILED(result))
	{
		assert(0);
		return result;
	}

	_viewPort.reset(new CD3DX12_VIEWPORT(_backBuffers[0]));								//スワップチェーンの情報を元にビューポートを初期化
	_rect.reset(new CD3DX12_RECT(0, 0, desc.Width, desc.Height));						//同じくシザー矩形を初期化

	return result;
}

/// <summary>
/// ビュープロジェクション用ビューを作成
/// </summary>
/// <returns>関数が成功したかどうか</returns>
HRESULT 
Dx12Wrapper::CreateSceneView()
{
	auto sceneResDesc = 
		CD3DX12_RESOURCE_DESC::Buffer((sizeof(SceneData) + _constSize) & ~_constSize);

	_dev->CreateCommittedResource(														//定数バッファー(リソース)を作成
		&_uploadHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&sceneResDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&_sceneConstBuff));

	_mappedScene = nullptr;																//定数バッファーに対し書き込めるようにする
	auto result = _sceneConstBuff->Map(0,nullptr,(void**)&_mappedScene);

	_mappedScene->view = XMMatrixLookAtLH(												//ビュー・プロジェクション行列を書き込む
		XMLoadFloat3(&_eye),XMLoadFloat3(&_target),XMLoadFloat3(&_up));
	_mappedScene->proj = XMMatrixPerspectiveFovLH(XM_PIDIV4,
		static_cast<float>(_winSize.cx)/ static_cast<float>(_winSize.cy),
		0.1f,
		1000.0f
	);
	_mappedScene->eye = _eye;															//視点座標も書き込む

	D3D12_DESCRIPTOR_HEAP_DESC sceneDescHeapDesc = {};									//定数バッファー用ディスクリプタヒープ構造体の作成
	sceneDescHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	sceneDescHeapDesc.NodeMask = 0;
	sceneDescHeapDesc.NumDescriptors = 1;
	sceneDescHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;

	result = _dev->CreateDescriptorHeap(												//ディスクリプタヒープ作成
		&sceneDescHeapDesc,IID_PPV_ARGS(_sceneDescHeap.ReleaseAndGetAddressOf()));

	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};										//定数バッファービュー設定用構造体の作成・設定
	cbvDesc.BufferLocation = _sceneConstBuff->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = (UINT)_sceneConstBuff->GetDesc().Width;
	
	_dev->CreateConstantBufferView(														//定数バッファービューの作成
		&cbvDesc,
		_sceneDescHeap->GetCPUDescriptorHandleForHeapStart());

	return result;
}

/// <summary>
/// 深度ステンシルビューを作成を作成する関数
/// </summary>
/// <returns>関数が成功したかどうか</returns>
HRESULT
Dx12Wrapper::CreateDepthStencilView()
{
	D3D12_HEAP_PROPERTIES depthHeapProp = {};											//深度ステンシルバッファー用ヒーププロパティを作成
	depthHeapProp.Type = D3D12_HEAP_TYPE_DEFAULT;
	depthHeapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	depthHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

	D3D12_RESOURCE_DESC depthResDesc = {};												//バッファー用リソースディスクを作成・設定
	depthResDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	depthResDesc.Width = _winSize.cx;
	depthResDesc.Height = _winSize.cy;
	depthResDesc.DepthOrArraySize = 1;
	depthResDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthResDesc.SampleDesc.Count = 1;
	depthResDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_CLEAR_VALUE depthClearValue = {};												//クリアバリューの作成・設定
	depthClearValue.DepthStencil.Depth = 1.0f;
	depthClearValue.Format = DXGI_FORMAT_D32_FLOAT;

	auto result = _dev->CreateCommittedResource(										//バッファーの作成
		&depthHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&depthResDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&depthClearValue,
		IID_PPV_ARGS(_depthBuffer.ReleaseAndGetAddressOf()));

	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};										//バッファー用ディスクリプタヒープ設定用構造体の作成
	dsvHeapDesc.NumDescriptors = 1;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;

	result = _dev->CreateDescriptorHeap(&dsvHeapDesc,IID_PPV_ARGS(&_dsvHeap));			//ディスクリプタヒープの作成

	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};											//バッファービュー設定用構造体の作成・設定
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Flags = D3D12_DSV_FLAG_NONE;

	
	_dev->CreateDepthStencilView(														//ビューの作成
		_depthBuffer.Get(),&dsvDesc,_dsvHeap->GetCPUDescriptorHandleForHeapStart());

	return result;
}

/// <summary>
/// エフェクト適用を決めるデータ用のヒープ・バッファーを作成する関数
/// </summary>
/// <returns>関数が成功したかどうか</returns>
HRESULT
Dx12Wrapper::CreateFactorBufferAndView()
{
	auto factorResDesc
		= CD3DX12_RESOURCE_DESC::Buffer((sizeof(Factor) + _constSize) & ~_constSize);	//リソースディスク作成

	auto result = _dev->CreateCommittedResource(										//バッファー作成
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

	result = _factorConstBuff->Map(0, nullptr, (void**)&_mappedFactor);					//バッファーへ書き込めるようにする
	if (FAILED(result))
	{
		assert(0);
		return result;
	}
	UpdateFade();

	D3D12_DESCRIPTOR_HEAP_DESC factorHeapDesc = {};										//ディスクリプタヒープ設定用構造体の作成
	factorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	factorHeapDesc.NodeMask = 0;
	factorHeapDesc.NumDescriptors = 1;
	factorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;

	result = _dev->CreateDescriptorHeap(
		&factorHeapDesc, IID_PPV_ARGS(_factorCBVHeap.ReleaseAndGetAddressOf()));		//ディスクリプタヒープ作成
	if (FAILED(result))
	{
		assert(0);
		return result;
	}

	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};										//定数バッファービュー設定用構造体の作成・設定
	cbvDesc.BufferLocation = _factorConstBuff->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = (UINT)_factorConstBuff->GetDesc().Width;

	_dev->CreateConstantBufferView(														//定数バッファービューの作成
		&cbvDesc,
		_factorCBVHeap->GetCPUDescriptorHandleForHeapStart());

	return result;
}

/// <summary>
/// テクスチャロード用テーブルを作成する関数
/// ファイル名に含まれている拡張子に応じて実行する関数を変える
/// </summary>
void
Dx12Wrapper::CreateTextureLoaderTable()
{
	_loadLambdaTable["sph"]													//sph,spa,png,jpg拡張子をkeyに、LoadFromWICFile関数をvalueにする
		= _loadLambdaTable["spa"]
		= _loadLambdaTable["bmp"]
		= _loadLambdaTable["png"]
		= _loadLambdaTable["jpg"]
		= [](const wstring& path, TexMetadata* meta, ScratchImage& img)
		->HRESULT
	{
		return LoadFromWICFile(path.c_str(), 0, meta, img);
	};

	_loadLambdaTable["tga"]													//tga拡張子をkeyに、LoadFromTGAFile関数をvalueにする
		= [](const wstring& path, TexMetadata* meta, ScratchImage& img)
		->HRESULT
	{
		return LoadFromTGAFile(path.c_str(), meta, img);
	};

	_loadLambdaTable["dds"]													//dds拡張子をkeyに、LoadFromDDSFile関数をvalueにする
		= [](const wstring& path, TexMetadata* meta, ScratchImage& img)
		->HRESULT
	{
		return LoadFromDDSFile(path.c_str(), 0, meta, img);
	};
}

/// <summary>
/// 視点、注視点のベクトルを初期化する関数
/// </summary>
void
Dx12Wrapper::ResetVector()
{
	_eye = _initialEye;
	_target = _initialTarget;
}

/// <summary>
/// コンストラクタ
/// </summary>
/// <param name="hwnd">ウィンドウハンドル</param>
/// <param name="deltaTime">1フレーム当たりの秒数</param>
Dx12Wrapper::Dx12Wrapper(HWND hwnd, float deltaTime) :
	_initialEye(0, 50, 100), _initialTarget(0, 50, 0), _up(0, 1, 0), _deltaTime(deltaTime),
	_fade(0.0f)
{
#ifdef _DEBUG
	EnableDebugLayer();													//デバッグ用レイヤーを起動
#endif 
	auto& app = Application::Instance();								//Applicationインスタンスを取得
	_winSize = app.GetWindowSize();										//ウィンドウのサイズを取得
	_rate = app.GetRate();												//フレームレートを取得

	InitializeDXGIDevice();												//デバイス関連を初期化

	InitializeCommand();												//コマンド関連を初期化

	CreateSwapChain(hwnd);												//スワップチェーンを作成

	CreateRenderTargetsView();											//レンダーターゲットを作成

	ResetVector();														//視点、注視点のベクトルを初期化

	CreateSceneView();													//ビュープロジェクション用ビューを作成

	CreateTextureLoaderTable();											//テクスチャロード用テーブルを作成

	CreateDepthStencilView();											//深度ステンシルビューを作成

	CreateFactorBufferAndView();										//フェードイン／アウト用バッファー・ビューを作成

	CreateDescriptorHeapForImgui();										//ImGui用ヒープを作成

	_dev->CreateFence(													//フェンスを作成
		_fenceVal, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&_fence));

	_sprite.reset(new SpriteManager(*this,_winSize.cx,_winSize.cy));	//SpriteManagerインスタンスを初期化

	_pera.reset(new PeraRenderer(*this));								//PeraRendererインスタンスを初期化

	_render.reset(new Renderer(*this));									//Rendererインスタンスを初期化

	_imgui.reset(new ImGuiManager(*this, hwnd));						//ImGuiManagerインスタンスを初期化
}


/// <summary>
/// リソースを遷移させる関数
/// </summary>
/// <param name="resource">遷移させたいリソース</param>
/// <param name="before">遷移前のステート</param>
/// <param name="after">遷移後のステート</param>
void
Dx12Wrapper::BarrierTransition(
	ID3D12Resource* resource,
	D3D12_RESOURCE_STATES before,
	D3D12_RESOURCE_STATES after)
{
	_barrier = CD3DX12_RESOURCE_BARRIER::Transition(resource, before, after);	//変数を設定

	//リソースを遷移させる
	_cmdList->ResourceBarrier(1, &_barrier);
}

/// <summary>
/// XZ平面に制限した視点座標から注視点座標へのベクトルを取得する関数
/// </summary>
/// <returns>制限付き視点座標から注視点座標へのベクトル</returns>
XMVECTOR
Dx12Wrapper::GetXZVecEyeToTarget()const
{
	XMVECTOR vec = XMLoadFloat3					//まず視点座標から注視点座標へのベクトルを取得
	(
		new XMFLOAT3
		(
			_target.x-_eye.x,
			_target.y - _eye.y, 
			_target.z - _eye.z
		)
	);

	vec = XMVectorMultiply(vec, XZ_PLANE);		//Y成分を0にし、ベクトルをXZ平面に制限

	XMVECTOR ret = XMVector3Normalize(vec);		//ベクトルを正規化
	
	return ret;
}

/// <summary>
/// 視点→注視点のベクトルに対し右に垂直なベクトルを返す関数
/// </summary>
/// <returns>垂直ベクトル</returns>
XMVECTOR
Dx12Wrapper::GetRightVector()const
{
	XMVECTOR vec = GetXZVecEyeToTarget();								//視点→注視点のベクトルを取得
	float xyz[] = { vec.m128_f32[0],vec.m128_f32[1],vec.m128_f32[2] };	//XYZ成分を取得
	
	XMVECTOR cross = XMLoadFloat3(										//外積（2つのベクトルから成る平面に対し垂直なベクトル）を取得
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
/// フレーム間の経過時間を返す関数
/// </summary>
/// <returns>フレーム間の経過時間</returns>
float 
Dx12Wrapper::GetDeltaTime()const
{
	return _deltaTime;
}

/// <summary>
/// カメラを近付ける・遠ざける関数
/// </summary>
/// <param name="x">移動距離</param>
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
/// カメラを平行移動させる関数
/// </summary>
/// <param name="dir">平行移動させる方向</param>
/// <param name="value">移動距離</param>
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


	//その後カメラ座標を更新
	_eye =
		XMFLOAT3
		(
			_coordinates->ToCartesian().x + _target.x,
			_coordinates->ToCartesian().y + _target.y,
			_coordinates->ToCartesian().z + _target.z
		);
}

/// <summary>
/// SphericalCoordinates上のカメラ座標を回転させる関数
/// </summary>
/// <param name="deg">方位角か仰角か決める</param>
/// <param name="value">上か下か、右か左か</param>
void
Dx12Wrapper::RotateSphericalCoordinates(Degree deg, float value)
{
	switch (deg)
	{
	case Degree::Azimth:	//方位角の方向に回転させる
		_eye = XMFLOAT3
		(
			_coordinates->Rotate(value * _deltaTime, 0.0f).ToCartesian().x + _target.x,
			_coordinates->Rotate(value * _deltaTime, 0.0f).ToCartesian().y + _target.y,
			_coordinates->Rotate(value * _deltaTime, 0.0f).ToCartesian().z + _target.z
		);
		break;
	case Degree::Elevation:	//仰角の方向に回転させる
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
/// カメラの位置を初期化する関数
/// </summary>
void
Dx12Wrapper::ResetSphericalCoordinates()
{
	ResetVector();									//視点、注視点をリセット

	_coordinates.reset(new SphericalCoordinates());	//球面座標を扱うSphericalCoordinatesインスタンスを作成
	_coordinates->SetRadius(300.0f);				//半径は500で固定
	_coordinates->SetAzimth(0);						//方位角は0度で初期化
	_coordinates->SetElevation(0);					//仰角も0度で初期化

	_eye = XMFLOAT3									//視点座標を設定
	(
		_coordinates->ToCartesian().x,
		_coordinates->ToCartesian().y,
		_coordinates->ToCartesian().z
	);
}

/// <summary>
/// ビュープロジェクション用ビューをセットする関数
/// </summary>
void
Dx12Wrapper::SetScene()
{
	_mappedScene->view = XMMatrixLookAtLH(										//行列・座標を書き込む
		XMLoadFloat3(&_eye), XMLoadFloat3(&_target), XMLoadFloat3(&_up));
	_mappedScene->proj = XMMatrixPerspectiveFovLH(XM_PIDIV4,
		static_cast<float>(_winSize.cx) / static_cast<float>(_winSize.cy),
		0.1f,
		1000.0f
	);
	_mappedScene->eye = _eye;

	ID3D12DescriptorHeap* sceneHeaps[] = { _sceneDescHeap.Get() };				//ディスクリプタヒープをコマンドリストにセット
	_cmdList->SetDescriptorHeaps(1, sceneHeaps);

	_cmdList->SetGraphicsRootDescriptorTable(									//ヒープのハンドルをルートパラメータと関連付け
		0, _sceneDescHeap->GetGPUDescriptorHandleForHeapStart());
}

/// <summary>
/// ゲーム画面用リソースの遷移(PRESENT→RENDER_TARGET)・RTVのセットを実行する関数
/// </summary>
void
Dx12Wrapper::BeginGameDraw()
{
	auto bbIdx = _swapchain->GetCurrentBackBufferIndex();									//現在のバックバッファーのインデックスを取得

	BarrierTransition(																		//ゲーム画面用リソースをRENDER_TARGETに遷移
		_backBuffers[bbIdx], 
		D3D12_RESOURCE_STATE_PRESENT,
		D3D12_RESOURCE_STATE_RENDER_TARGET);

	auto rtvH = _rtvHeap->GetCPUDescriptorHandleForHeapStart();								//インデックスに応じてRTVヒープのハンドルをズラす
	rtvH.ptr += 
		bbIdx * _dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	auto dsvH = _dsvHeap->GetCPUDescriptorHandleForHeapStart();

	_cmdList->OMSetRenderTargets(1, &rtvH, true, &dsvH);									//RTV・DSVヒープをセット

	float clearColor[4] = { 1.0f,1.0f,1.0f,1.0f };											//背景色を設定

	_cmdList->ClearRenderTargetView(rtvH, clearColor, 0, nullptr);							//RTVを初期化
	_cmdList->ClearDepthStencilView(dsvH, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);		//DSVも初期化
}

/// <summary>
/// ゲーム用リソースの遷移(RENDER_TARGET→STATE_PRESENT)を実行する関数
/// </summary>
void
Dx12Wrapper::EndGameDraw()
{
	auto bbIdx = _swapchain->GetCurrentBackBufferIndex();		//現在のバックバッファーのインデックスを取得

	BarrierTransition(											//ゲーム画面用リソースをPRESENTに遷移
		_backBuffers[bbIdx], 
		D3D12_RESOURCE_STATE_RENDER_TARGET, 
		D3D12_RESOURCE_STATE_PRESENT);

	_cmdList->Close();											//コマンドリストのクローズ

	ID3D12CommandList* cmdlists[] = { _cmdList.Get() };			//コマンドリストの実行
	_cmdQueue->ExecuteCommandLists(1, cmdlists);

	WaitForCommandQueue();										//処理の同期待ちを行う

	_cmdAllocator->Reset();										//キューのクリア
	_cmdList->Reset(_cmdAllocator.Get(), nullptr);				//再度コマンドリストを貯める準備
}
/// <summary>
/// 処理の同期待ちを行う関数
/// </summary>
void
Dx12Wrapper::WaitForCommandQueue()
{
	_cmdQueue->Signal(_fence.Get(), ++_fenceVal);					//フェンス値を更新

	if (_fence->GetCompletedValue() != _fenceVal)					//CPUとGPUのフェンス値を比較し、一致するまで処理を待ち合わせる
	{
		auto event = CreateEvent(nullptr, false, false, nullptr);	//空のイベントを取得？

		_fence->SetEventOnCompletion(_fenceVal, event);				//フェンス値が_fenceValになった時イベントを通知

		WaitForSingleObject(event, INFINITE);						//イベントが発生するまで待ち続ける

		CloseHandle(event);											//イベントを閉じる
	}
}

/// <summary>
/// テクスチャを読み込む関数
/// </summary>
/// <param name="texPath">テクスチャファイル名</param>
/// <returns>テクスチャ(シェーダーリソース)バッファー</returns>
ID3D12Resource*
Dx12Wrapper::LoadTextureFromFile(const char* texPath)
{
	auto it = _resourceTable.find(texPath);									//リソースが読み込まれているか確認し、あれば返す
	if (it != _resourceTable.end())
	{
		return _resourceTable[texPath];
	}

	string strTexPath = texPath;											//ファイル名をコピー

	TexMetadata metaData = {};												//メタデータ(画像ファイルに関数情報)
	ScratchImage scratchImg = {};											//実際のテクスチャデータを入れるオブジェクト

	auto wtexPath = ToWideString(strTexPath);								//ファイル名をwstring型に変換
	auto ext = FileExtension(strTexPath);									//ファイル名の拡張子を取得

	auto result = _loadLambdaTable[ext](wtexPath,							//拡張子を確認し、対応する関数を用いて読み込み
		&metaData,
		scratchImg);
	if (FAILED(result))
	{
		assert(0);
		return nullptr;
	}

	auto img = scratchImg.GetImage(0, 0, 0);								//テクスチャの生データ取得

	D3D12_HEAP_PROPERTIES texHeapProp = {};									//ヒーププロパティ作成・設定
	texHeapProp.Type = D3D12_HEAP_TYPE_CUSTOM;
	texHeapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	texHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;
	texHeapProp.CreationNodeMask = 0;
	texHeapProp.VisibleNodeMask = 0;

	D3D12_RESOURCE_DESC texResDesc = {};									//バッファー用リソースディスク構造体作成・設定
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

	ID3D12Resource* texBuff = nullptr;										//リソース作成
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

	result = texBuff->WriteToSubresource(									//作成したリソースにテクスチャを書き込む
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

	_resourceTable[texPath] = texBuff;										//テクスチャ用連想配列にファイル名とそれに関連したテクスチャを登録

	return texBuff;
}

/// <summary>
/// //DXTK12用のヒープを作成する関数
/// </summary>
/// <returns>ヒープ</returns>
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
/// imgui用のヒープを作成する関数
/// </summary>
/// <returns>処理が成功したかどうか</returns>
HRESULT
Dx12Wrapper::CreateDescriptorHeapForImgui()
{
	D3D12_DESCRIPTOR_HEAP_DESC desc = {};								//構造体の設定
	desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	desc.NodeMask = 0;
	desc.NumDescriptors = 1;
	desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;

	auto result = _dev->CreateDescriptorHeap(							//ヒープの作成
		&desc, IID_PPV_ARGS(_heapForImgui.ReleaseAndGetAddressOf()));

	return result;
}

/// <summary>
/// ルートシグネチャ・パイプライン・描画方法をセットする関数
/// </summary>
/// <param name="rootSignature">ルートシグネチャ</param>
/// <param name="pipeline">パイプライン</param>
/// <param name="topology">描画方法</param>
void
Dx12Wrapper::SetPipelines(ID3D12RootSignature* rootSignature, ID3D12PipelineState* pipeline, D3D12_PRIMITIVE_TOPOLOGY topology)
{
	_cmdList->SetGraphicsRootSignature(rootSignature);	//ルートシグネチャをセット
	_cmdList->SetPipelineState(pipeline);				//パイプラインをセット
	_cmdList->IASetPrimitiveTopology(topology);			//描画方法を設定
}

/// <summary>
/// フェードイン／アウトデータをシェーダーに反映する関数
/// </summary>
void
Dx12Wrapper::UpdateFade()
{
	_mappedFactor->fade = clamp(_fade, 0.0f, 1.0f);		//α値を0以上1以下に制限
}

/// <summary>
/// フェードイン／アウトを実行する関数
/// </summary>
/// <param name="start">フェード値の初期値</param>
/// <param name="end">フェード値の最終値</param>
/// <param name="func">フェード完了時に実行したい処理</param>
void
Dx12Wrapper::Fade(float start, float end)
{
	_mtx.lock();

	float t = 0.0f;							//時間
	_start = start;							//フェード値の初期値
	_end = end;								//フェード値の最終値

	_rate *= FADE_TIME;

	//線形補間処理
	for (int i = 0; i <= _rate; i++)
	{
		_fade = lerp(_start, _end, t);		//フェード値を線形補間

		t += _deltaTime / FADE_TIME;		//加算

		Sleep(1);							//処理を待つ
	}

	_mtx.unlock();
}

/// <summary>
/// PeraRendererインスタンスを返す関数
/// </summary>
/// <returns>インスタンスのポインタ</returns>
PeraRenderer*
Dx12Wrapper::Pera()const
{
	return _pera.get();
}

/// <summary>
/// ImGuiManagerインスタンスを返す関数
/// </summary>
/// <returns>インスタンスのポインタ</returns>
ImGuiManager*
Dx12Wrapper::ImGui()const
{
	return _imgui.get();
}

/// <summary>
/// imgui用ヒープを返す関数
/// </summary>
/// <returns>ヒープのポインタ</returns>
ID3D12DescriptorHeap*
Dx12Wrapper::HeapForImgui()const
{
	return _heapForImgui.Get();
}

/// <summary>
/// Rendererインスタンスを返す関数
/// </summary>
/// <returns>インスタンスのポインタ</returns>
Renderer*
Dx12Wrapper::Render()
{
	return _render.get();
}

/// <summary>
/// SpriteManagerインスタンスを返す関数
/// </summary>
/// <returns>インスタンスのポインタ</returns>
SpriteManager*
Dx12Wrapper::Sprite()const
{
	return _sprite.get();
}

/// <summary>
/// デバイスを返す関数
/// </summary>
/// <returns>デバイスのポインタ</returns>
ID3D12Device*
Dx12Wrapper::Device()const
{
	return _dev.Get();
}

/// <summary>
/// スワップチェーンを返す関数
/// </summary>
/// <returns>スワップチェーンのポインタ</returns>
IDXGISwapChain4*
Dx12Wrapper::Swapchain()const
{
	return _swapchain.Get();
}

/// <summary>
/// コマンドリストを返す関数
/// </summary>
/// <returns>コマンドリストのポインタ</returns>
ID3D12GraphicsCommandList*
Dx12Wrapper::CommandList()const
{
	return _cmdList.Get();
}

/// <summary>
/// コマンドキューを返す関数
/// </summary>
/// <returns>コマンドキューのポインタ</returns>
ID3D12CommandQueue*
Dx12Wrapper::CommandQueue()const
{
	return _cmdQueue.Get();
}

/// <summary>
/// バックバッファー（1枚目）を返す関数
/// </summary>
/// <returns></returns>
ID3D12Resource*
Dx12Wrapper::BackBuffer() const
{
	return _backBuffers[0];
}

/// <summary>
/// RTVヒープを返す関数
/// </summary>
/// <returns>RTVヒープ</returns>
ID3D12DescriptorHeap*
Dx12Wrapper::RTVHeap() const
{
	return _rtvHeap.Get();
}

/// <summary>
/// 深度ステンシルヒープを返す関数
/// </summary>
/// <returns>深度ステンシルヒープ</returns>
ID3D12DescriptorHeap*
Dx12Wrapper::DSVHeap() const
{
	return _dsvHeap.Get();
}

/// <summary>
/// 因数用ヒープを返す関数
/// </summary>
/// <returns>因数用ヒープ</returns>
ID3D12DescriptorHeap* 
Dx12Wrapper::FactorCBVHeap() const
{
	return _factorCBVHeap.Get();
}

/// <summary>
/// ビューポートを返す関数
/// </summary>
/// <returns>ビューポート</returns>
D3D12_VIEWPORT*
Dx12Wrapper::ViewPort() const
{
	return _viewPort.get();
}

/// <summary>
/// シザー矩形を返す関数
/// </summary>
/// <returns>シザー矩形</returns>
D3D12_RECT*
Dx12Wrapper::Rect() const
{
	return _rect.get();
}
#include "Functions.h"
#include "Renderer/PeraRenderer.h"
#include "Wrapper/Dx12Wrapper.h"

/// <summary>
/// ペラポリゴン用リソース・RTV・SRVを作成する関数
/// </summary>
/// <returns>関数が成功したかどうか</returns>
HRESULT
PeraRenderer::CreatePeraResourcesAndView()
{
	auto resDesc = _dx12.BackBuffer()->GetDesc();							//元々使用していたリソース設定用構造体を利用

	float clsClr[4] = { 1.0f,1.0f,1.0f,1.0f };								//クリア値はレンダリング時と同じ値を設定する必要がある
	D3D12_CLEAR_VALUE clearValue = 
		CD3DX12_CLEAR_VALUE(DXGI_FORMAT_R8G8B8A8_UNORM, clsClr);

	auto result = _dx12.Device()->CreateCommittedResource(					//リソース(レンダーターゲット)の作成
		&_defHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		&clearValue,
		IID_PPV_ARGS(&_peraResource));
	if (FAILED(result))
	{
		assert(0);
		return result;
	}

	auto heapDesc = _dx12.RTVHeap()->GetDesc();								//RTV用ヒープ用構造体の設定
	heapDesc.NumDescriptors = 1;

	result = _dx12.Device()->CreateDescriptorHeap(							//RTV用ヒープの作成
		&heapDesc, IID_PPV_ARGS(&_peraRTVHeap));
	if (FAILED(result))
	{
		assert(0);
		return result;
	}

	D3D12_RENDER_TARGET_VIEW_DESC peraRTVDesc = {};							//RTV用構造体の設定
	peraRTVDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
	peraRTVDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	_dx12.Device()->CreateRenderTargetView(									//RTVの作成
		_peraResource.Get(),
		&peraRTVDesc,
		_peraRTVHeap->GetCPUDescriptorHandleForHeapStart()
	);

	heapDesc.NumDescriptors = 1;											//SRV用ヒープ用構造体の設定
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

	
	result = _dx12.Device()->CreateDescriptorHeap(							//SRV用ヒープの作成
		&heapDesc, IID_PPV_ARGS(_peraSRVHeap.ReleaseAndGetAddressOf()));
	if (result != S_OK)
	{
		assert(0);
		return result;
	}

	D3D12_SHADER_RESOURCE_VIEW_DESC peraSRVDesc = {};						//SRV用構造体の作成
	peraSRVDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	peraSRVDesc.Format = peraRTVDesc.Format;
	peraSRVDesc.Texture2D.MipLevels = 1;
	peraSRVDesc.Shader4ComponentMapping
		= D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

	_dx12.Device()->CreateShaderResourceView(								//SRVの作成
		_peraResource.Get(),
		&peraSRVDesc,
		_peraSRVHeap->GetCPUDescriptorHandleForHeapStart()
	);

	return result;
}

/// <summary>
/// ペラポリゴン用頂点バッファー・VBVを作成する関数
/// </summary>
/// <returns>関数が成功したかどうか</returns>
HRESULT
PeraRenderer::CreatePeraVertex()
{
	struct PeraVertex											//ペラポリゴンを作成するための頂点構造体
	{
		XMFLOAT3 pos;
		XMFLOAT2 uv;
	};

	PeraVertex pv[] =											//ペラポリゴンを構成する4つの頂点
	{
		{{-1.0f,-1.0f,0.1f},{0.0f,1.0f}},
		{{-1.0f,1.0f,0.1f},{0.0f,0.0f}},
		{{1.0f,-1.0f,0.1f},{1.0f,1.0f}},
		{{1.0f,1.0f,0.1f},{1.0f,0.0f}}
	};

	auto pvResDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeof(pv));	//リソースのサイズを設定

	auto result = _dx12.Device()->CreateCommittedResource(		//ペラポリゴン用頂点バッファーの作成
		&_uploadHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&pvResDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&_peraVB));
	if (result != S_OK)
	{
		assert(0);
		return result;
	}

	PeraVertex* mappedPera = nullptr;							//ペラポリゴンをGPU側に転送できるようにする
	_peraVB->Map(0, nullptr, (void**)&mappedPera);
	copy(begin(pv), end(pv), mappedPera);
	_peraVB->Unmap(0, nullptr);

	_peraVBV.BufferLocation = _peraVB->GetGPUVirtualAddress();	//頂点バッファービューの設定
	_peraVBV.SizeInBytes = sizeof(pv);
	_peraVBV.StrideInBytes = sizeof(PeraVertex);

	return result;
}

/// <summary>
/// ペラポリゴン用パイプラインを作成する関数
/// </summary>
/// <returns>関数が成功したかどうか</returns>
HRESULT
PeraRenderer::CreatePeraPipeline()
{
	D3D12_INPUT_ELEMENT_DESC layout[2] =												//ペラポリゴン用頂点レイアウト
	{
		{
			"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,									//頂点座標
			D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0
		},
		{
			"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,									//UV座標
			D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0
		}
	};

	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpsDesc = {};									//ペラポリゴン用パイプライン用構造体の作成
	gpsDesc.InputLayout.NumElements = _countof(layout);									//入力レイアウト数
	gpsDesc.InputLayout.pInputElementDescs = layout;									//入力レイアウト設定

	ID3DBlob* errorBlob = nullptr;														//エラー確認用データ

	auto result = S_OK;																	//関数の返り値をあらかじめ設定

#ifdef _DEBUG																			//Debugビルド時はファイルから直接読み込む

	ID3DBlob* vs = nullptr;																//頂点シェーダー用データ
	ID3DBlob* ps = nullptr;																//ピクセルシェーダー用データ

	result = D3DCompileFromFile(L"3_shader/Pera/PeraVertex.hlsl", nullptr,				//頂点シェーダー読み込み
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"PeraVS", "vs_5_0", 0, 0,
		&vs,
		&errorBlob);
	if (result != S_OK)
	{
		assert(0);
		return result;
	}

	result = D3DCompileFromFile(L"3_shader/Pera/PeraPixel.hlsl", nullptr,				//ピクセルシェーダー読み込み
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"PeraPS", "ps_5_0", 0, 0,
		&ps,
		&errorBlob);
	if (result != S_OK)
	{
		assert(0);
		return result;
	}

	gpsDesc.VS = CD3DX12_SHADER_BYTECODE(vs);											//パイプライン用構造体に頂点シェーダーを設定
	gpsDesc.PS = CD3DX12_SHADER_BYTECODE(ps);											//ピクセルシェーダーも設定

#else																					//Releaseビルド時はコンパイル済みシェーダーを読み込む

	gpsDesc.VS.pShaderBytecode = g_PeraVS;												//頂点シェーダーのバイナリとメモリサイズ
	gpsDesc.VS.BytecodeLength = sizeof(g_PeraVS);
	gpsDesc.PS.pShaderBytecode = g_PeraPS;												//ピクセルシェーダー
	gpsDesc.PS.BytecodeLength = sizeof(g_PeraPS);

#endif

	gpsDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);								//パイプラインステートの設定
	gpsDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	gpsDesc.NumRenderTargets = 1;
	gpsDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	gpsDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	gpsDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	gpsDesc.SampleDesc.Count = 1;
	gpsDesc.SampleDesc.Quality = 0;
	gpsDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

	D3D12_DESCRIPTOR_RANGE range[2] = {};												//ディスクリプタレンジ
	range[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;								//画面テクスチャ[t0]
	range[0].BaseShaderRegister = 0;
	range[0].NumDescriptors = 1;

	range[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;								//定数スロット[b0]
	range[1].BaseShaderRegister = 0;
	range[1].NumDescriptors = 1;

	D3D12_ROOT_PARAMETER rp[2] = {};													//ルートパラメータ
	rp[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rp[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rp[0].DescriptorTable.pDescriptorRanges = &range[0];
	rp[0].DescriptorTable.NumDescriptorRanges = 1;

	rp[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rp[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rp[1].DescriptorTable.pDescriptorRanges = &range[1];
	rp[1].DescriptorTable.NumDescriptorRanges = 1;

	D3D12_STATIC_SAMPLER_DESC sampler = CD3DX12_STATIC_SAMPLER_DESC(0);					//サンプラー

	D3D12_ROOT_SIGNATURE_DESC rsDesc = {};												//ルートシグネチャ設定構造体の作成
	rsDesc.NumParameters = 2;
	rsDesc.pParameters = rp;
	rsDesc.NumStaticSamplers = 1;
	rsDesc.pStaticSamplers = &sampler;
	rsDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	ID3DBlob* rsBlob = nullptr;															//ルートシグネチャ用データ

	result = D3D12SerializeRootSignature(												//ルートシグネチャ用データの初期化
		&rsDesc,
		D3D_ROOT_SIGNATURE_VERSION_1,
		&rsBlob,
		&errorBlob);
	if (result != S_OK)
	{
		assert(0);
		return result;
	}

	result = _dx12.Device()->CreateRootSignature(										//ルートシグネチャの作成
		0,
		rsBlob->GetBufferPointer(),
		rsBlob->GetBufferSize(),
		IID_PPV_ARGS(&_peraRS));
	if (result != S_OK)
	{
		assert(0);
		return result;
	}

	rsBlob->Release();																	//不要になったデータを開放

	gpsDesc.pRootSignature = _peraRS.Get();												//パイプラインへルートシグネチャを登録

	result = _dx12.Device()->CreateGraphicsPipelineState(								//パイプラインの作成
		&gpsDesc, IID_PPV_ARGS(&_peraPipeline));
	if (result != S_OK)
	{
		assert(0);
		return result;
	}

	return result;
}

/// <summary>
/// コンストラクタ
/// </summary>
/// <param name="dx12">Dx12Wrapperインスタンス</param>
PeraRenderer::PeraRenderer(Dx12Wrapper& dx12):_dx12(dx12)
{
	CreatePeraResourcesAndView();							//ペラポリゴン用RT・RTV・SRVを作成
	 
	CreatePeraVertex();										//ペラポリゴン用頂点バッファー・VBVを作成
	
	CreatePeraPipeline();									//ペラポリゴン用パイプラインを作成
}

/// <summary>
/// デストラクタ
/// 特に処理はしない
/// </summary>
PeraRenderer::~PeraRenderer()
{

}

/// <summary>
/// ペラポリゴン用リソースの遷移(SHADER_RESOURCE→RENDER_TARGET)・RTVのセットを実行する関数
/// </summary>
void
PeraRenderer::BeginPeraDraw()
{
	_dx12.BarrierTransition(																//ペラポリゴン用リソースをRENDER_TARGETに遷移
		_peraResource.Get(),
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET);

	auto rtvH = _peraRTVHeap->GetCPUDescriptorHandleForHeapStart();							//ペラポリゴン用RTV・SRVヒープのハンドルを取得
	auto dsvH = _dx12.DSVHeap()->GetCPUDescriptorHandleForHeapStart();						//DSVヒープのハンドルを取得

	_dx12.CommandList()->OMSetRenderTargets(1, &rtvH, false, &dsvH);						//RTV・DSVヒープのハンドルをコマンドリストに設定

	float clearColor[4] = { 1.0f,1.0f,1.0f,1.0f };											//背景色を設定

	_dx12.CommandList()->ClearRenderTargetView(												//RTVをクリア
		rtvH, clearColor, 0, nullptr);				
	_dx12.CommandList()->ClearDepthStencilView(												//DSVをクリア
		dsvH, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

	_dx12.CommandList()->RSSetViewports(1, _dx12.ViewPort());								//ビューポートをコマンドリストに設定
	_dx12.CommandList()->RSSetScissorRects(1, _dx12.Rect());								//シザー矩形をコマンドリストに設定
}

/// <summary>
/// ペラポリゴン用ルートシグネチャ・パイプラインをセットする関数
/// </summary>
void
PeraRenderer::SetPeraPipelines()
{
	_dx12.SetPipelines(																//パイプラインステート、ルートシグネチャ、プリミティブトポロジーをセット
		_peraRS.Get(), _peraPipeline.Get(), D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	ID3D12DescriptorHeap* peraSRVHeaps[] = { _peraSRVHeap.Get() };					//ペラポリゴン用SRV、及びハンドルを設定
	_dx12.CommandList()->SetDescriptorHeaps(1, peraSRVHeaps);
	auto handle = _peraSRVHeap->GetGPUDescriptorHandleForHeapStart();
	_dx12.CommandList()->SetGraphicsRootDescriptorTable(0, handle);

	ID3D12DescriptorHeap* factorCBVHeap[] = { _dx12.FactorCBVHeap() };				//エフェクト適用データ用CBV、及びハンドルを設定
	_dx12.CommandList()->SetDescriptorHeaps(1, factorCBVHeap);
	handle = _dx12.FactorCBVHeap()->GetGPUDescriptorHandleForHeapStart();
	_dx12.CommandList()->SetGraphicsRootDescriptorTable(1, handle);

	_dx12.UpdateFade();																//データを更新
}

/// <summary>
/// ペラポリゴン用リソースの遷移(RENDER_TARGET→SHADER_RESOURCE)を実行する関数
/// </summary>
void
PeraRenderer::EndPeraDraw()
{
	_dx12.BarrierTransition(							//ペラポリゴン用リソースをSHADER_RESOURCEに遷移
		_peraResource.Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
	);
}

/// <summary>
/// ペラポリゴン用頂点バッファービューを返す関数
/// </summary>
/// <returns>ビュー</returns>
D3D12_VERTEX_BUFFER_VIEW*
PeraRenderer::PeraVBView()
{
	return &_peraVBV;
}

/// <summary>
/// ペラポリゴン用パイプラインを返す関数
/// </summary>
/// <returns>パイプラインのポインタ</returns>
ID3D12PipelineState*
PeraRenderer::PeraPipeline()
{
	return _peraPipeline.Get();
}

/// <summary>
/// ペラポリゴン用ルートシグネチャを返す関数
/// </summary>
/// <returns>ルートシグネチャのポインタ</returns>
ID3D12RootSignature*
PeraRenderer::PeraRootSignature()
{
	return _peraRS.Get();
}
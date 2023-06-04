#include "Application.h"

/// <summary>
/// カメラをどちらの方に回転させるか決定する列挙体
/// </summary>
enum Degree
{
	Azimth,			//方位角
	Elevation,		//仰角
};

/// <summary>
/// カメラをどちらの方向へ平行移動させるか決定する列挙体
/// </summary>
enum Direction
{
	Horizontal,	//水平方向
	Vertical,	//垂直方向
	Depth,		//奥行方向
};

/// <summary>
/// 描画命令を実行するためのオブジェクト等を管理するクラス
/// </summary>
class SphericalCoordinates;
class SpriteManager;
class PeraRenderer;
class Renderer;
class ImGuiManager;
class Dx12Wrapper
{
	template<typename T>
	using ComPtr = ComPtr<T>;

private:
	SIZE _winSize;												//ウィンドウサイズ

	XMFLOAT3 _initialEye;										//視点の初期座標
	XMFLOAT3 _initialTarget;									//注視点の初期座標

	shared_ptr<SpriteManager> _sprite;							//スプライト管理インスタンス
	shared_ptr<PeraRenderer> _pera;								//UI管理インスタンス
	shared_ptr<Renderer> _render;								//パイプライン・ルートシグネチャ管理インスタンス
	shared_ptr<ImGuiManager> _imgui;							//デバッグ用ウィンドウ管理インスタンス
	shared_ptr<SphericalCoordinates> _coordinates;				//カメラ座標用インスタンス

	//DXGI周り
	ComPtr<IDXGIFactory6> _dxgiFactory;							//ファクトリー
	ComPtr<ID3D12Device> _dev;									//デバイス

	//DX12(コマンド)周り
	ComPtr<ID3D12CommandAllocator> _cmdAllocator;				//コマンドアロケータ
	ComPtr<ID3D12GraphicsCommandList> _cmdList;					//コマンドリスト
	ComPtr<ID3D12CommandQueue> _cmdQueue;						//コマンドキュー
	ComPtr<IDXGISwapChain4> _swapchain;							//スワップチェーン

	//表示関連のバッファ回り
	vector<ID3D12Resource*> _backBuffers;						//レンダーターゲット用バッファー
	ComPtr<ID3D12DescriptorHeap> _rtvHeap;						//レンダーターゲットビュー用ヒープ
	ComPtr<ID3D12Resource> _depthBuffer;						//深度ステンシル用バッファー
	ComPtr<ID3D12DescriptorHeap> _dsvHeap;						//深度ステンシルビュー用ヒープ

	unique_ptr<D3D12_VIEWPORT> _viewPort = {};					//ビューポート
	unique_ptr<D3D12_RECT> _rect = {};							//シザー矩形

	//ビュープロジェクション用定数バッファ周り
	ComPtr<ID3D12Resource> _sceneConstBuff;						//ビュー・プロジェクション用バッファー
	ComPtr<ID3D12DescriptorHeap> _sceneDescHeap;				//ビュー・プロジェクション用ディスクリプタヒープ

	ComPtr<ID3D12DescriptorHeap> _factorCBVHeap;				//歪みデータ用ヒープ
	ComPtr<ID3D12Resource> _factorConstBuff;					//歪みデータ用バッファー

	ComPtr<ID3D12DescriptorHeap> _heapForImgui;					//imguiで使用するヒープ

	/// <summary>
	/// シェーダーにフェードイン／アウトデータを渡すための構造体及びインスタンス
	/// </summary>
	struct Factor
	{
		float fade;												//フェードイン／アウトデータ
	};
	Factor* _mappedFactor = nullptr;							//マップ用ポインタ

	/// <summary>
	/// スロットに流し込むデータの構造体及びインスタンス
	/// 各行列を別個に流し込むのではなく、構造体としてまとめる
	/// </summary>
	struct SceneData
	{
		XMMATRIX view;
		XMMATRIX proj;
		XMFLOAT3 eye;
	};
	SceneData* _mappedScene;									//マップ用ポインタ

	ComPtr<ID3D12Fence> _fence;									//フェンス
	UINT64 _fenceVal;											//初期化値

	CD3DX12_RESOURCE_BARRIER _barrier;							//リソースバリア用変数

	XMFLOAT3 _eye;												//視点座標
	XMFLOAT3 _target;											//注視点座標
	XMFLOAT3 _up;												//上座標

	mutex _mtx;													//排他制御用オブジェクト

	float _deltaTime;											//フレーム間の時間

	float _fade;												//フェードイン／アウトするかどうかを決めるデータ

	float _start, _end;											//フェードイン／アウトで線形補完する際の初期値、最終値

	int _rate;													//フレームレート
		
	using LoadLambda_t =										//テクスチャロード用ラムダ式を格納する型の定義
		function<HRESULT(
			const wstring& path, TexMetadata*, ScratchImage&
		)>;

	map<string, ID3D12Resource*> _resourceTable;				//stringをkey,リソースをvalueとした連想配列

	HRESULT InitializeDXGIDevice();								//デバイス関連を初期化する関数

	HRESULT InitializeCommand();								//コマンド関連を初期化する関数

	HRESULT CreateSwapChain(const HWND& hwnd);					//スワップチェーンを作成する関数

	HRESULT CreateRenderTargetsView();							//レンダーターゲットを作成する関数

	HRESULT CreateSceneView();									//ビュープロジェクション用ビューを作成する関数

	HRESULT CreateDepthStencilView();							//深度ステンシルビューを作成する関数

	HRESULT CreateFactorBufferAndView();						//エフェクト適用を決めるデータ用のヒープ・バッファーを作成する関数

	HRESULT CreateDescriptorHeapForImgui();						//imgui用のディスクリプタヒープを作成する関数

	void CreateTextureLoaderTable();							//テクスチャロート用テーブルを作成する関数

	void ResetVector();											//視点、注視点のベクトルを初期化する関数

public:
	map<string, LoadLambda_t> _loadLambdaTable;							//stringをkey,LoadLambda_tをvalueとした連想配列

	Dx12Wrapper(HWND hwnd, float deltatime);							//コンストラクタ
	~Dx12Wrapper();														//デストラクタ

	void BarrierTransition(												//リソースを遷移させる関数
		ID3D12Resource* resource,
		D3D12_RESOURCE_STATES before, 
		D3D12_RESOURCE_STATES after);

	XMVECTOR GetXZVecEyeToTarget()const;								//視点座標（カメラ座標）から注視点座標へのベクトルを取得する関数
	XMVECTOR GetRightVector()const;										//注視点へのベクトルの右方向のベクトルを取得する関数

	float GetDeltaTime()const;

	void ScalingSphericalCoordinates(int x);							//カメラを近付ける・遠ざける関数
	void TranslateSphericalCoordinates(Direction dir, float value);		//カメラを移動させる関数
	void RotateSphericalCoordinates(Degree deg, float value);			//カメラを回転させる関数
	void ResetSphericalCoordinates();									//カメラの位置を初期化する関数

	void SetScene();													//ビュープロジェクション用ビューをコマンドリストにセットする関数

	void BeginGameDraw();												//リソースの遷移(STATE_PRESENT→RENDER_TARGET)・RTVのセットを実行する関数
	void EndGameDraw();													//リソースの遷移(RENDER_TARGET→STATE_PRESENT)を実行する関数

	void WaitForCommandQueue();											//処理の同期待ちを行う関数

	ID3D12Resource* LoadTextureFromFile(const char* texPath);			//テクスチャを読み込む関数

	ID3D12DescriptorHeap* CreateDescriptorHeapForSpriteFont();			//DXTK12用のヒープを作成する関数

	void SetPipelines(ID3D12RootSignature* rootSignature,				//ルートシグネチャ・パイプライン・描画方法をセットする関数
		ID3D12PipelineState* pipeline, 
		D3D12_PRIMITIVE_TOPOLOGY topology);

	void UpdateFade();													//フェードイン／アウトデータをシェーダーに反映する関数

	void Fade(float start,float end);									//フェードイン／アウトを実行する関数

	Renderer* Render();													//Rendererインスタンスを返す関数
	SpriteManager* Sprite() const;										//SpriteManagerインスタンスを返す関数
	PeraRenderer* Pera() const;											//PeraRendererインスタンスを返す関数
	ImGuiManager* ImGui() const;										//ImGuiManagerインスタンスを返す関数

	ID3D12Device* Device() const;										//デバイスを返す関数
	IDXGISwapChain4* Swapchain() const;									//スワップチェーンを返す関数
	ID3D12GraphicsCommandList* CommandList() const;						//コマンドリストを返す関数
	ID3D12CommandQueue* CommandQueue() const;							//コマンドキューを返す関数
	ID3D12Resource* BackBuffer() const;									//バックバッファー（1枚目）を返す関数
	ID3D12DescriptorHeap* RTVHeap() const;								//RTVヒープを返す関数
	ID3D12DescriptorHeap* DSVHeap() const;								//深度ステンシルヒープを返す関数
	ID3D12DescriptorHeap* FactorCBVHeap() const;						//因数用ヒープを返す関数
	ID3D12DescriptorHeap* HeapForImgui() const;							//imgui用ヒープを返す関数
	D3D12_VIEWPORT* ViewPort() const;									//ビューポートを返す関数
	D3D12_RECT* Rect() const;											//シザー矩形を返す関数
};


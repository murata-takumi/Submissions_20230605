#include "Application.h"
#include "Package/Package.h"
#include "Scene/PlayScene.h"
#include "Scene/TitleScene.h"

const unsigned int DISPLAY_WIDTH = GetSystemMetrics(SM_CXSCREEN);		//ディスプレイ幅
const unsigned int WINDOW_WIDTH = 1280;									//ウィンドウ幅
const unsigned int DISPLAY_HEIGHT = GetSystemMetrics(SM_CYSCREEN);		//ディスプレイ高さ
const unsigned int WINDOW_HEIGHT = 720;									//ウィンドウ高さ

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND,UINT,WPARAM,LPARAM);

/// <summary>
/// OSから送られてくるデータ(メッセージ)を処理する関数
/// </summary>
/// <param name="hwnd">ウィンドウを識別するデータ</param>
/// <param name="msg">OSから送られてくるデータ</param>
/// <param name="wpraram">1つ目のメッセージの変数</param>
/// <param name="lparam" >2つ目のメッセージの変数</param>
/// <returns></returns>
LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);											//OSにアプリの終了を伝える
		return 0;

	case WM_ACTIVATEAPP:
		Keyboard::ProcessMessage(msg, wparam, lparam);
		Mouse::ProcessMessage(msg, wparam, lparam);
		break;

	case WM_INPUT:													//マウスの入力を伝える
	case WM_MOUSEMOVE:
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_MOUSEWHEEL:
	case WM_XBUTTONDOWN:
	case WM_XBUTTONUP:
	case WM_MOUSEHOVER:
		Mouse::ProcessMessage(msg, wparam, lparam);
		break;

	case WM_SYSKEYDOWN:												//キーボードの入力を伝える
	case WM_KEYDOWN:
	case WM_KEYUP:
	case WM_SYSKEYUP:
		Keyboard::ProcessMessage(msg, wparam, lparam);
		break;

	case WM_MENUCHAR:
		return MAKELRESULT(0, MNC_CLOSE);
	}

	ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam);		//imguiのウィンドウを動かすために必要
	return DefWindowProc(hwnd, msg, wparam, lparam);				//既定の処理を行う
}

/// <summary>
/// フォルダ内のデータをすべて削除する関数
/// </summary>
void
OnExit()
{
	Package package;
	package.RemoveFileInDiretory(_T("Asset/font"));
	package.RemoveFileInDiretory(_T("Asset/image"));
	package.RemoveFileInDiretory(_T("Asset/model"));
	package.RemoveFileInDiretory(_T("Asset/sound"));

	return;
}

/// <summary>
/// ゲーム用ウィンドウを作成する関数
/// </summary>
/// <param name="hwnd">ウィンドウを識別するデータ</param>
/// <param name="windowClass">ウィンドウ作成用データを格納する構造体</param>
void 
Application::CreateGameWindow(HWND& hwnd, WNDCLASSEX& windowClass)
{
	windowClass.cbSize = sizeof(WNDCLASSEX);							//構造体のメモリサイズ指定
	windowClass.lpfnWndProc = (WNDPROC)WindowProcedure;					//コールバック関数の指定
	windowClass.lpszClassName = _T("DX12Application");					//アプリケーションクラス名
	windowClass.hInstance = GetModuleHandle(nullptr);					//インスタンス(アプリケーション)ハンドルの取得

	RegisterClassEx(&windowClass);										//アプリケーションクラスの指定をOSに伝える

	RECT wrc = { 0,0,(LONG)WINDOW_WIDTH,(LONG)WINDOW_HEIGHT };			//ウィンドウサイズの決定
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);					//ウィンドウサイズを計算

	hwnd = CreateWindow(windowClass.lpszClassName,						//ウィンドウハンドルの登録
		_T("Program"),
		WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX ,
		(DISPLAY_WIDTH/2) - (WINDOW_WIDTH/2),
		(DISPLAY_HEIGHT / 2) - (WINDOW_HEIGHT / 2),
		wrc.right - wrc.left,
		wrc.bottom - wrc.top,
		nullptr,
		nullptr,
		windowClass.hInstance,
		nullptr
	);

	auto hdc = GetDC(_hwnd);											//リフレッシュレートの取得
	_rate = GetDeviceCaps(hdc, VREFRESH);

	if (_rate <= 60) { _interval = 1; }									//レートに応じてレンダーターゲットのフリップ間隔を設定
	else if (_rate >= 120) { _interval = 2; }

	_deltaTime = (float)1 / (float)_rate;								//1フレーム毎の経過秒数を計算
}

/// <summary>
/// コンストラクタ
/// 特に処理は行わない
/// </summary>
Application::Application()
{

}

/// <summary>
/// 静的インスタンスを返す関数
/// </summary>
/// <returns>インスタンスの参照</returns>
Application&
Application::Instance()
{
	static Application instance;
	return instance;
}

/// <summary>
/// 現在のシーンを返す関数
/// </summary>
/// <returns>シーンのポインタ</returns>
shared_ptr<SceneBase>
Application::GetCurrentScene()
{
	return _Scene;
}

/// <summary>
/// アプリケーション初期化関数
/// </summary>
/// <returns>初期化が成功したかどうか</returns>
bool 
Application::Init()
{
	auto result = CoInitializeEx(0, COINIT_MULTITHREADED);						//スレッド処理を行うためCOMライブラリを初期化
	CreateGameWindow(_hwnd, _windowClass);										//ゲーム用ウィンドウ作成

	ShowCursor(true);															//マウスカーソルを表示する

	atexit(OnExit);																//終了ハンドラーに関数を登録

	_package.reset(new Package());												//Packageインスタンスを初期化

	_package->CreateDirectoryAndFile(_T("Asset/font.pak"));						//.pakファイルからデータを読み込む
	_package->CreateDirectoryAndFile(_T("Asset/image.pak"));
	_package->CreateDirectoryAndFile(_T("Asset/model.pak"));
	_package->CreateDirectoryAndFile(_T("Asset/sound.pak"));

	_dx12.reset(new Dx12Wrapper(_hwnd,_deltaTime));								//Dx12Wrapperインスタンスを初期化
	
	_input.reset(new InputManager());											//InputManagerインスタンスを初期化

	_sound.reset(new SoundManager());											//SoundManagerインスタンスを初期化

	_title.reset(new TitleScene(*_dx12, *_input, *_sound));						//タイトルシーンの初期化
	_play.reset(new PlayScene(*_dx12, *_input, *_sound));						//ゲームシーンの初期化

	ShowWindow(_hwnd, SW_SHOW);													//ウィンドウハンドルに対応するウィンドウを表示

	ChangeScene(SceneNames::Title);												//タイトルシーンへ遷移

	return true;
}

/// <summary>
/// ゲーム画面を描画する関数
/// </summary>
void
Application::Run()
{
	//ゲームループ
	while (true)
	{
 		//スレッド関係の処理
		if (PeekMessage(&_msg, nullptr, 0, 0, PM_REMOVE))		//メッセージを調べる
		{
			TranslateMessage(&_msg);							//メッセージを翻訳
			DispatchMessage(&_msg);								//ウィンドウプロシージャへメッセージを送る

			if (_msg.message == WM_QUIT)						//アプリが終了したらループを終了させる
			{
				break;
			}
		}
		else
		{
			_Scene->Update();									//各シーンの更新処理
		}
	}
}

/// <summary>
/// アプリ終了時、後始末を行う関数
/// </summary>
void 
Application::Terminate()
{
	UnregisterClass(_windowClass.lpszClassName, _windowClass.hInstance);	//構造体の設定を解除
}

/// <summary>
/// ウィンドウサイズを返す関数
/// </summary>
/// <returns>ウィンドウサイズを示す構造体</returns>
SIZE
Application::GetWindowSize()const
{
	SIZE ret;					//構造体を宣言し、幅と高さを設定
	ret.cx = WINDOW_WIDTH;
	ret.cy = WINDOW_HEIGHT;

	return ret;					//構造体を返す
}

/// <summary>
/// シーンを切り替える関数
/// </summary>
/// <param name="name">切り替えたいシーン</param>
void
Application::ChangeScene(SceneNames name)
{
	//引数に応じて遷移先のシーンを決める
	switch (name)
	{
	case SceneNames::Title:
		SetScene(_title);		//タイトルシーンへ遷移
		break;
	case SceneNames::Play:
		SetScene(_play);		//ゲームシーンへ遷移
		break;
	default:					//シーン名が上記3つのどれでもない場合異常終了
		assert(0);
		break;
	}
}

/// <summary>
/// シーンの設定・終了時の処理・開始時の処理を実行する関数
/// </summary>
/// <param name="scene">切り替えたいシーンオブジェクト</param>
void
Application::SetScene(shared_ptr<SceneBase> scene)
{
	if (_Scene != nullptr)_Scene->SceneEnd();	//シーン終了時の処理

	if (scene != nullptr)_Scene = scene;		//シーン切り替え

	_Scene->SceneStart();						//シーン開始時の処理
}

/// <summary>
/// ゲームを終了させる
/// </summary>
void
Application::ExitApp()
{
	SendMessage(_hwnd,WM_DESTROY,0,0);	//終了用メッセージを送る
}

/// <summary>
/// レンダーターゲットのフリップ間隔を返す関数
/// </summary>
/// <returns>フリップ間隔</returns>
int
Application::GetInterval()const
{
	return _interval;
}

/// <summary>
/// 1秒あたりのフレームレートを返す関数
/// </summary>
/// <returns>フレームレート</returns>
int
Application::GetRate()const
{
	return _rate;
}

/// <summary>
/// デストラクタ
/// ポインタを開放する
/// </summary>
Application::~Application()
{	

}
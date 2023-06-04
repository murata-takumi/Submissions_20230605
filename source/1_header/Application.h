#pragma once
#include <DirectXTex-master/DirectXTex/d3dx12.h>
#include <DirectXTex-master/DirectXTex/DirectXTex.h>

#include <d3dcompiler.h>
#include <array>
#include <cstdlib>
#include <map>
#include <tchar.h>
#include <dxgi1_6.h>
#include <vector>
#include <thread>
#include <mutex>
#include <functional>
#include <chrono>
#include <iostream>
#include <fstream>
#include <filesystem>

using namespace std;
using namespace DirectX;
using namespace Microsoft::WRL;

#if _DEBUG
#pragma comment(lib,"assimp-vc143-mtd.lib")
#else
#pragma comment(lib,"assimp-vc143-mt.lib")
#endif // DEBUG


#pragma comment(lib,"DirectXTK12.lib")
#pragma comment(lib,"DirectXTex.lib")
#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib,"winmm.lib")

const float PI = 3.141592653f;					//円周率

/// <summary>
/// 読み込むSceneクラスを識別するための列挙型
/// </summary>
enum SceneNames
{
	Title,		//タイトルシーン
	Play,		//ゲームシーン
};

class Dx12Wrapper;
class InputManager;
class Package;
class PlayScene;
class SceneBase;
class SoundManager;
class TitleScene;
/// <summary>
/// ゲームの初期化・更新・終了を管理するクラス
/// </summary>
class Application
{
private:
	WNDCLASSEX _windowClass;										//ウィンドウ作成時に必要な情報を格納
	HWND _hwnd;														//ウィンドウの識別に必要な値

	shared_ptr<Dx12Wrapper> _dx12;									//Dx12Wrapperインスタンス
	shared_ptr<InputManager> _input;								//InputManagerインスタンス	
	shared_ptr<SoundManager> _sound;								//SoundManagerインスタンス

	shared_ptr<TitleScene> _title;									//TitleSceneインスタンス
	shared_ptr<PlayScene> _play;									//PlaySceneインスタンス

	shared_ptr<SceneBase> _Scene;									//更新処理を行うインスタンス

	shared_ptr<Package> _package;									

	float _deltaTime;												//フレーム間の経過時間

	int _rate;														//1秒あたりのフレーム数
	int _interval;													//フリップ間隔

	void CreateGameWindow(HWND& hwnd, WNDCLASSEX& windowClass);		//ゲーム用ウィンドウを作成する関数

	Application();													//コンストラクタ
	Application(const Application&) = delete;						//コンストラクタを外部から呼び出されないよう設定

public:

	MSG _msg = {};									//メッセージ用構造体

	static Application& Instance();					//インスタンスの参照を返す？

	shared_ptr<SceneBase> GetCurrentScene();		//現在のシーンを返す関数

	bool Init();									//初期化

	void Run();										//ゲーム画面の描画

	void Terminate();								//ゲーム終了時の後始末

	SIZE GetWindowSize()const;						//ウィンドウサイズを返す

	void SetScene(shared_ptr<SceneBase> scene);		//シーンを切り替える
	void ChangeScene(SceneNames name);				//シーンの設定・終了時の処理・開始時の処理を実行
	void ExitApp();									//アプリケーションを終了する

	int GetInterval()const;							//レンダーターゲットのフリップ間隔を返す関数
	int GetRate()const;								//1秒間のフレームレートを返す関数

	~Application();									//デストラクタ
};

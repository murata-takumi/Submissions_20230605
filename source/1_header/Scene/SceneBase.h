#pragma once
#include "Application.h"

#include "Manager/InputManager.h"
#include "Manager/SoundManager.h"
#include "Manager/SpriteManager.h"
#include "Renderer/PeraRenderer.h"
#include "Renderer/Renderer.h"
#include "Wrapper/Dx12Wrapper.h"

#include <chrono>

/// <summary>
/// 各シーンオブジェクトの基幹クラス
/// </summary>
class Dx12Wrapper;
class FBXActor;
class SpriteManager;
class InputManager;
class SoundManager;
class ImGuiManager;
class SceneBase
{
	friend ImGuiManager;

public:

	SceneBase(																					//コンストラクタ
		Dx12Wrapper& dx12,
		InputManager& input,
		SoundManager& sound
	) :_dx12(dx12), _input(input), _sound(sound), _app(Application::Instance()),_fps(0.0f)
	{
		_interval = _app.GetInterval();															//フリップ間隔を取得
	}
	virtual ~SceneBase()																		//デストラクタ
	{

	}

	virtual void Update() = 0;																	//更新処理
	virtual void SceneStart();																	//シーン開始時の処理
	virtual void SceneEnd();																	//シーン終了時の処理
protected:
	Application& _app;										//Applicationインスタンス
	Dx12Wrapper& _dx12;										//Dx12Wrapperインスタンス
	InputManager& _input;									//InputManagerインスタンス
	SoundManager& _sound;									//SoundManagerインスタンス

	static bool _isLoading;									//ロード中であることを示す真理値
	static bool _isPlaying;									//操作可能であることを示す真理値

	static LARGE_INTEGER _currentTime;						//ゲームの現在時間を格納する値
	static LARGE_INTEGER _updatedTime;						//ゲームの直前の時間を格納する値
	static LARGE_INTEGER _beforeTime;						//ゲームの時間を一時保存しておくための値

	double _fps;											//1秒当たりに画面が切り替わる回数

	int _interval;											//フリップ間隔

	void ChangeScene(SceneNames name);						//シーン遷移する為の関数

	virtual void BackGroundDraw();							//背景を描画する関数

	virtual void ModelDraw();								//モデルを描画する関数

	virtual void EffectAndUIDraw();							//エフェクト・UIを描画する関数

	void PeraDraw();										//ペラポリゴンの描画処理をまとめた関数

	void GameDraw();										//ゲーム画面ポリゴンの描画処理をまとめた関数

	void DrawUpdate();										//描画関数

	void InputUpdate();										//入力更新関数

	void UpdateFPS();										//FPSを更新する関数

	void ParallelProcess(function<void(void)> func);		//ラムダ式を受け取り並列処理を行う関数
};
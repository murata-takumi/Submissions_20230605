#include "Functions.h"

#include "Scene/SceneBase.h"

const float FRAME_TIME = 1.0f / 60.0f;	//更新回数を60FPSに固定する際の1フレームの秒数

bool SceneBase::_isLoading = false;		//各シーンでロード中かどうかを識別する真理値
bool SceneBase::_isPlaying = false;		//各シーンで操作可能かどうかを決める真理値

LARGE_INTEGER SceneBase::_currentTime;	//現フレームの時間
LARGE_INTEGER SceneBase::_updatedTime;	//FPS更新時の時間
LARGE_INTEGER SceneBase::_beforeTime;	//前フレームの時間

/// <summary>
/// シーン開始時に実行する処理
/// </summary>
void
SceneBase::SceneStart()
{
	auto startFunc = [&]()
	{
		_dx12.Fade(1.0f, 0.0f);		//フェードイン処理

		_isPlaying = true;			//操作可能にする
	};
	ParallelProcess(startFunc);		//上記の処理を並列処理する
}

/// <summary>
/// シーン終了時に実行する処理
/// </summary>
void
SceneBase::SceneEnd()
{

}

/// <summary>
/// シーンを変更する関数
/// </summary>
/// <param name="name">変更先のシーンの名前</param>
void
SceneBase::ChangeScene(SceneNames name)
{
	auto changeFunc = [&,name]()
	{
		_isPlaying = false;			//操作不可にする

		_dx12.Fade(0.0f, 1.0f);		//フェードアウト処理

		_app.ChangeScene(name);		//シーンを遷移させる
	};
	ParallelProcess(changeFunc);	//並列処理
}

/// <summary>
/// ペラポリゴンの描画処理をまとめた関数
/// </summary>
void
SceneBase::PeraDraw()
{
	_dx12.Pera()->BeginPeraDraw();				//ペラポリゴン描画準備
		
	BackGroundDraw();							//背景描画処理

	_dx12.SetPipelines(							//ペラポリゴン用パイプラインをセット
		_dx12.Render()->GetRootSignature(),
		_dx12.Render()->GetPipelineState(),
		D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST
	);

	_dx12.SetScene();							//ビュー・プロジェクション行列を適用

	ModelDraw();								//モデル描画処理

	_dx12.Pera()->EndPeraDraw();				//ペラポリゴン描画後始末
}

/// <summary>
/// 背景の描画を行う関数
/// </summary>
void
SceneBase::BackGroundDraw()
{
	_dx12.Sprite()->BackGroundDraw();
}

/// <summary>
/// モデルの描画を行う関数
/// </summary>
void
SceneBase::ModelDraw()
{

}

/// <summary>
/// 画面にかかるエフェクト・UIを描画する関数
/// </summary>
void
SceneBase::EffectAndUIDraw()
{
	_dx12.Sprite()->CursorDraw();	//マウスカーソルを描画
}

/// <summary>
/// ゲーム画面描画処理
/// </summary>
void
SceneBase::GameDraw()
{
	_dx12.BeginGameDraw();															//ゲーム画面描画準備

	_dx12.Pera()->SetPeraPipelines();												//以下ペラポリゴン用コマンドリスト処理

	_dx12.CommandList()->IASetVertexBuffers(0, 1, _dx12.Pera()->PeraVBView());		//ペラポリゴン用VBVをセット
	_dx12.CommandList()->DrawInstanced(4, 1, 0, 0);									//ペラポリゴンを構成する頂点を描画

	EffectAndUIDraw();																//ここにエフェクト上の描画処理

	_dx12.EndGameDraw();															//ゲーム画面描画後始末

	_dx12.Swapchain()->Present(_interval, 0);										//スワップチェーンのフリップ処理

	_dx12.Sprite()->Commit();														//グラフィックスメモリを設定
}

/// <summary>
/// 各描画の更新をまとめて行う関数
/// </summary>
void
SceneBase::DrawUpdate()
{
	UpdateFPS();			//FPSを更新

	PeraDraw();				//ペラポリゴン描画処理

	GameDraw();				//ゲーム画面描画処理
}

/// <summary>
/// 入力を更新する関数
/// </summary>
void
SceneBase::InputUpdate()
{
	_input.UpdateInput();																	//入力を更新

	_dx12.Sprite()->SetMousePosition(_input.MouseState().x, _input.MouseState().y);			//マウス座標を格納
}

/// <summary>
/// 現在のフレームレートを更新する関数
/// </summary>
void
SceneBase::UpdateFPS()
{
	QueryPerformanceCounter(&_currentTime);							//現在フレームの時間を取得

	auto diff = GetLIntDiff(_currentTime,_updatedTime);				//前回FPS更新時との差分
	auto frameTime = GetLIntDiff(_currentTime, _beforeTime);		//前フレームとの差分

	if (frameTime < FRAME_TIME)										//処理時間に余裕がある場合、待ち合わせを行う
	{
		DWORD sleepTime =
			static_cast<DWORD>((FRAME_TIME - frameTime) * 1000);
		timeBeginPeriod(1);

		Sleep(sleepTime);

		timeEndPeriod(1);
	}

	if (diff >= 1)													//差分が1秒以上だった場合
	{
		_fps = 1 / frameTime;										//1秒を差分で割り、FPSを取得

		_updatedTime = _currentTime;								//FPS更新時間を更新
	}

	_beforeTime = _currentTime;										//前フレームの時間を更新
}

/// <summary>
/// ラムダ式を受け取り並列処理を行う関数
/// </summary>
/// <param name="func">並列に処理したいラムダ式</param>
void
SceneBase::ParallelProcess(function<void(void)> func)
{
	thread th(func);	//ラムダ式を処理するスレッドを作成
	th.detach();		//スレッドの管理を手放す
}
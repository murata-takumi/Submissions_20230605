#include "Scene/TitleScene.h"

/// <summary>
/// コンストラクタ
/// </summary>
/// <param name="dx12">Dx12Wrapperインスタンス</param>
/// <param name="renderer">Rendererインスタンス</param>
/// <param name="sprite">SpriteManagerインスタンス</param>
/// <param name="input">InputManagerインスタンス</param>
/// <param name="sound">SoundManagerインスタンス</param>
TitleScene::TitleScene
(
	Dx12Wrapper& dx12,
	InputManager& input,
	SoundManager& sound
)
	:SceneBase
	(
		dx12,
		input,
		sound
	)
{

}

/// <summary>
/// デストラクタ
/// </summary>
TitleScene::~TitleScene()
{

}

/// <summary>
/// 更新処理
/// </summary>
void
TitleScene::Update()
{
	InputUpdate();											//入力を更新

	if (_input.MouseTracker().leftButton == 
		Mouse::ButtonStateTracker::PRESSED && _isPlaying)
	{
		if (_dx12.Sprite()->TitleIsOnStart())				//開始ボタンの上で左クリック
		{
			_sound.CallButton();							//効果音

			ChangeScene(SceneNames::Play);					//ゲームシーンへ遷移

			return;
		}

		if (_dx12.Sprite()->TitleIsOnEnd())					//終了ボタンの上で左クリック
		{
			_sound.CallButton();							//効果音

			_isPlaying = false;								//操作不可にする

			auto exitFunc = ([&]()
				{
					_dx12.Fade(0.0f, 1.0f);					//フェードアウト

					_app.ExitApp();							//ゲームを終了

					return;
				}
			);
			ParallelProcess(exitFunc);

			return;
		}
	}

	DrawUpdate();	//描画処理
}

/// <summary>
/// シーン開始時の処理
/// </summary>
void
TitleScene::SceneStart()
{
	SceneBase::SceneStart();
}

/// <summary>
/// シーン終了時の処理
/// </summary>
void
TitleScene::SceneEnd()
{

}

/// <summary>
/// 背景を描画する関数
/// </summary>
void
TitleScene::BackGroundDraw()
{
	SceneBase::BackGroundDraw();		//背景を描画
	_dx12.Sprite()->TitleDraw();		//タイトル画面でのボタン描画
}

/// <summary>
/// エフェクト・UIを描画する関数
/// </summary>
void
TitleScene::EffectAndUIDraw()
{
	SceneBase::EffectAndUIDraw();	//マウスカーソルを描画
}
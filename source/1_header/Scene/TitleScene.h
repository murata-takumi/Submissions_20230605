#pragma once
#include "Scene/SceneBase.h"

/// <summary>
/// タイトルシーンを管理するクラス
/// </summary>
class TitleScene : public SceneBase
{
public:
	TitleScene(						//コンストラクタ
		Dx12Wrapper& dx12,
		InputManager& input,
		SoundManager& sound
	);
	~TitleScene();					//デストラクタ

	void Update() override;			//更新処理

	void SceneStart() override;		//シーン開始時の処理
	void SceneEnd() override;		//シーン終了時の処理

protected:
	void BackGroundDraw() override;		//背景を描画する関数
	void EffectAndUIDraw() override;	//画面に対するエフェクト・UIを描画する関数
};
#pragma once
#include "Scene/SceneBase.h"

/// <summary>
/// ゲームシーンを管理するクラス
/// </summary>
class PlayScene :public SceneBase
{
public:
	PlayScene(							//コンストラクタ
		Dx12Wrapper& dx12,
		InputManager& input,
		SoundManager& sound
	);
	~PlayScene();						//デストラクタ

	void Update() override;				//更新処理

	void SceneStart() override;			//シーン開始時の処理
	void SceneEnd() override;			//シーン終了時の処理

protected:
	void BackGroundDraw() override;			//UI描画処理
	void ModelDraw() override;				//モデル描画処理
	void EffectAndUIDraw() override;		//画面エフェクト描画処理

	shared_ptr<FBXActor> _actor;			//Unitychanインスタンス

	XMVECTOR _direction;					//プレイヤー、カメラの進行ベクトル
};
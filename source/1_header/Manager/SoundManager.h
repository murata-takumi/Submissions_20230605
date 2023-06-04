#pragma once
#include "Application.h"

#include <DirectXTK12-master/Inc/Audio.h>

/// <summary>
/// 音声データを管理するクラス
/// </summary>
class SoundManager
{
private:

	unique_ptr<DirectX::AudioEngine> _audioEngine;		//サウンド全体を管理

	unique_ptr<DirectX::SoundEffect> _buttonEffect;		//ボタン押下時の効果音を読み込む

public:
	
	SoundManager();		//コンストラクタ
	~SoundManager();	//デストラクタ

	void CallButton();	//ボタン押下時の効果音
};
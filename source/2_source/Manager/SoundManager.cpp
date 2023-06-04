#include "Manager/SoundManager.h"

/// <summary>
/// コンストラクタ
/// 音声データの読み込みを行う
/// </summary>
SoundManager::SoundManager()
{
	AUDIO_ENGINE_FLAGS eflags = AudioEngine_Default;												//フラグを設定

#ifdef _DEBUG
	eflags |= AudioEngine_Debug;
#endif // DEBUG
	_audioEngine = make_unique<AudioEngine>(eflags);												//AudioEngineインスタンスを初期化

	_buttonEffect = make_unique<SoundEffect>(_audioEngine.get(), L"Asset/sound/button.wav");		//効果音読み込み
}

/// <summary>
/// デストラクタ
/// AudioEngineインスタンスのリセットを行う
/// </summary>
SoundManager::~SoundManager()
{
	_audioEngine->Reset();
}

/// <summary>
/// ボタン押下時の効果音の再生
/// </summary>
void
SoundManager::CallButton()
{
	_buttonEffect->Play();
}
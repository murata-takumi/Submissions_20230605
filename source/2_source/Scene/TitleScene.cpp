#include "Scene/TitleScene.h"

/// <summary>
/// �R���X�g���N�^
/// </summary>
/// <param name="dx12">Dx12Wrapper�C���X�^���X</param>
/// <param name="renderer">Renderer�C���X�^���X</param>
/// <param name="sprite">SpriteManager�C���X�^���X</param>
/// <param name="input">InputManager�C���X�^���X</param>
/// <param name="sound">SoundManager�C���X�^���X</param>
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
/// �f�X�g���N�^
/// </summary>
TitleScene::~TitleScene()
{

}

/// <summary>
/// �X�V����
/// </summary>
void
TitleScene::Update()
{
	InputUpdate();											//���͂��X�V

	if (_input.MouseTracker().leftButton == 
		Mouse::ButtonStateTracker::PRESSED && _isPlaying)
	{
		if (_dx12.Sprite()->TitleIsOnStart())				//�J�n�{�^���̏�ō��N���b�N
		{
			_sound.CallButton();							//���ʉ�

			ChangeScene(SceneNames::Play);					//�Q�[���V�[���֑J��

			return;
		}

		if (_dx12.Sprite()->TitleIsOnEnd())					//�I���{�^���̏�ō��N���b�N
		{
			_sound.CallButton();							//���ʉ�

			_isPlaying = false;								//����s�ɂ���

			auto exitFunc = ([&]()
				{
					_dx12.Fade(0.0f, 1.0f);					//�t�F�[�h�A�E�g

					_app.ExitApp();							//�Q�[�����I��

					return;
				}
			);
			ParallelProcess(exitFunc);

			return;
		}
	}

	DrawUpdate();	//�`�揈��
}

/// <summary>
/// �V�[���J�n���̏���
/// </summary>
void
TitleScene::SceneStart()
{
	SceneBase::SceneStart();
}

/// <summary>
/// �V�[���I�����̏���
/// </summary>
void
TitleScene::SceneEnd()
{

}

/// <summary>
/// �w�i��`�悷��֐�
/// </summary>
void
TitleScene::BackGroundDraw()
{
	SceneBase::BackGroundDraw();		//�w�i��`��
	_dx12.Sprite()->TitleDraw();		//�^�C�g����ʂł̃{�^���`��
}

/// <summary>
/// �G�t�F�N�g�EUI��`�悷��֐�
/// </summary>
void
TitleScene::EffectAndUIDraw()
{
	SceneBase::EffectAndUIDraw();	//�}�E�X�J�[�\����`��
}
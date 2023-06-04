#pragma once
#include "Scene/SceneBase.h"

/// <summary>
/// �^�C�g���V�[�����Ǘ�����N���X
/// </summary>
class TitleScene : public SceneBase
{
public:
	TitleScene(						//�R���X�g���N�^
		Dx12Wrapper& dx12,
		InputManager& input,
		SoundManager& sound
	);
	~TitleScene();					//�f�X�g���N�^

	void Update() override;			//�X�V����

	void SceneStart() override;		//�V�[���J�n���̏���
	void SceneEnd() override;		//�V�[���I�����̏���

protected:
	void BackGroundDraw() override;		//�w�i��`�悷��֐�
	void EffectAndUIDraw() override;	//��ʂɑ΂���G�t�F�N�g�EUI��`�悷��֐�
};
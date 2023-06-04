#pragma once
#include "Scene/SceneBase.h"

/// <summary>
/// �Q�[���V�[�����Ǘ�����N���X
/// </summary>
class PlayScene :public SceneBase
{
public:
	PlayScene(							//�R���X�g���N�^
		Dx12Wrapper& dx12,
		InputManager& input,
		SoundManager& sound
	);
	~PlayScene();						//�f�X�g���N�^

	void Update() override;				//�X�V����

	void SceneStart() override;			//�V�[���J�n���̏���
	void SceneEnd() override;			//�V�[���I�����̏���

protected:
	void BackGroundDraw() override;			//UI�`�揈��
	void ModelDraw() override;				//���f���`�揈��
	void EffectAndUIDraw() override;		//��ʃG�t�F�N�g�`�揈��

	shared_ptr<FBXActor> _actor;			//Unitychan�C���X�^���X

	XMVECTOR _direction;					//�v���C���[�A�J�����̐i�s�x�N�g��
};
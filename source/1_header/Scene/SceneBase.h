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
/// �e�V�[���I�u�W�F�N�g�̊�N���X
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

	SceneBase(																					//�R���X�g���N�^
		Dx12Wrapper& dx12,
		InputManager& input,
		SoundManager& sound
	) :_dx12(dx12), _input(input), _sound(sound), _app(Application::Instance()),_fps(0.0f)
	{
		_interval = _app.GetInterval();															//�t���b�v�Ԋu���擾
	}
	virtual ~SceneBase()																		//�f�X�g���N�^
	{

	}

	virtual void Update() = 0;																	//�X�V����
	virtual void SceneStart();																	//�V�[���J�n���̏���
	virtual void SceneEnd();																	//�V�[���I�����̏���
protected:
	Application& _app;										//Application�C���X�^���X
	Dx12Wrapper& _dx12;										//Dx12Wrapper�C���X�^���X
	InputManager& _input;									//InputManager�C���X�^���X
	SoundManager& _sound;									//SoundManager�C���X�^���X

	static bool _isLoading;									//���[�h���ł��邱�Ƃ������^���l
	static bool _isPlaying;									//����\�ł��邱�Ƃ������^���l

	static LARGE_INTEGER _currentTime;						//�Q�[���̌��ݎ��Ԃ��i�[����l
	static LARGE_INTEGER _updatedTime;						//�Q�[���̒��O�̎��Ԃ��i�[����l
	static LARGE_INTEGER _beforeTime;						//�Q�[���̎��Ԃ��ꎞ�ۑ����Ă������߂̒l

	double _fps;											//1�b������ɉ�ʂ��؂�ւ���

	int _interval;											//�t���b�v�Ԋu

	void ChangeScene(SceneNames name);						//�V�[���J�ڂ���ׂ̊֐�

	virtual void BackGroundDraw();							//�w�i��`�悷��֐�

	virtual void ModelDraw();								//���f����`�悷��֐�

	virtual void EffectAndUIDraw();							//�G�t�F�N�g�EUI��`�悷��֐�

	void PeraDraw();										//�y���|���S���̕`�揈�����܂Ƃ߂��֐�

	void GameDraw();										//�Q�[����ʃ|���S���̕`�揈�����܂Ƃ߂��֐�

	void DrawUpdate();										//�`��֐�

	void InputUpdate();										//���͍X�V�֐�

	void UpdateFPS();										//FPS���X�V����֐�

	void ParallelProcess(function<void(void)> func);		//�����_�����󂯎����񏈗����s���֐�
};
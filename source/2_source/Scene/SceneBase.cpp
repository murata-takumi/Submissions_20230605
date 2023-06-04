#include "Functions.h"

#include "Scene/SceneBase.h"

const float FRAME_TIME = 1.0f / 60.0f;	//�X�V�񐔂�60FPS�ɌŒ肷��ۂ�1�t���[���̕b��

bool SceneBase::_isLoading = false;		//�e�V�[���Ń��[�h�����ǂ��������ʂ���^���l
bool SceneBase::_isPlaying = false;		//�e�V�[���ő���\���ǂ��������߂�^���l

LARGE_INTEGER SceneBase::_currentTime;	//���t���[���̎���
LARGE_INTEGER SceneBase::_updatedTime;	//FPS�X�V���̎���
LARGE_INTEGER SceneBase::_beforeTime;	//�O�t���[���̎���

/// <summary>
/// �V�[���J�n���Ɏ��s���鏈��
/// </summary>
void
SceneBase::SceneStart()
{
	auto startFunc = [&]()
	{
		_dx12.Fade(1.0f, 0.0f);		//�t�F�[�h�C������

		_isPlaying = true;			//����\�ɂ���
	};
	ParallelProcess(startFunc);		//��L�̏�������񏈗�����
}

/// <summary>
/// �V�[���I�����Ɏ��s���鏈��
/// </summary>
void
SceneBase::SceneEnd()
{

}

/// <summary>
/// �V�[����ύX����֐�
/// </summary>
/// <param name="name">�ύX��̃V�[���̖��O</param>
void
SceneBase::ChangeScene(SceneNames name)
{
	auto changeFunc = [&,name]()
	{
		_isPlaying = false;			//����s�ɂ���

		_dx12.Fade(0.0f, 1.0f);		//�t�F�[�h�A�E�g����

		_app.ChangeScene(name);		//�V�[����J�ڂ�����
	};
	ParallelProcess(changeFunc);	//���񏈗�
}

/// <summary>
/// �y���|���S���̕`�揈�����܂Ƃ߂��֐�
/// </summary>
void
SceneBase::PeraDraw()
{
	_dx12.Pera()->BeginPeraDraw();				//�y���|���S���`�揀��
		
	BackGroundDraw();							//�w�i�`�揈��

	_dx12.SetPipelines(							//�y���|���S���p�p�C�v���C�����Z�b�g
		_dx12.Render()->GetRootSignature(),
		_dx12.Render()->GetPipelineState(),
		D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST
	);

	_dx12.SetScene();							//�r���[�E�v���W�F�N�V�����s���K�p

	ModelDraw();								//���f���`�揈��

	_dx12.Pera()->EndPeraDraw();				//�y���|���S���`���n��
}

/// <summary>
/// �w�i�̕`����s���֐�
/// </summary>
void
SceneBase::BackGroundDraw()
{
	_dx12.Sprite()->BackGroundDraw();
}

/// <summary>
/// ���f���̕`����s���֐�
/// </summary>
void
SceneBase::ModelDraw()
{

}

/// <summary>
/// ��ʂɂ�����G�t�F�N�g�EUI��`�悷��֐�
/// </summary>
void
SceneBase::EffectAndUIDraw()
{
	_dx12.Sprite()->CursorDraw();	//�}�E�X�J�[�\����`��
}

/// <summary>
/// �Q�[����ʕ`�揈��
/// </summary>
void
SceneBase::GameDraw()
{
	_dx12.BeginGameDraw();															//�Q�[����ʕ`�揀��

	_dx12.Pera()->SetPeraPipelines();												//�ȉ��y���|���S���p�R�}���h���X�g����

	_dx12.CommandList()->IASetVertexBuffers(0, 1, _dx12.Pera()->PeraVBView());		//�y���|���S���pVBV���Z�b�g
	_dx12.CommandList()->DrawInstanced(4, 1, 0, 0);									//�y���|���S�����\�����钸�_��`��

	EffectAndUIDraw();																//�����ɃG�t�F�N�g��̕`�揈��

	_dx12.EndGameDraw();															//�Q�[����ʕ`���n��

	_dx12.Swapchain()->Present(_interval, 0);										//�X���b�v�`�F�[���̃t���b�v����

	_dx12.Sprite()->Commit();														//�O���t�B�b�N�X��������ݒ�
}

/// <summary>
/// �e�`��̍X�V���܂Ƃ߂čs���֐�
/// </summary>
void
SceneBase::DrawUpdate()
{
	UpdateFPS();			//FPS���X�V

	PeraDraw();				//�y���|���S���`�揈��

	GameDraw();				//�Q�[����ʕ`�揈��
}

/// <summary>
/// ���͂��X�V����֐�
/// </summary>
void
SceneBase::InputUpdate()
{
	_input.UpdateInput();																	//���͂��X�V

	_dx12.Sprite()->SetMousePosition(_input.MouseState().x, _input.MouseState().y);			//�}�E�X���W���i�[
}

/// <summary>
/// ���݂̃t���[�����[�g���X�V����֐�
/// </summary>
void
SceneBase::UpdateFPS()
{
	QueryPerformanceCounter(&_currentTime);							//���݃t���[���̎��Ԃ��擾

	auto diff = GetLIntDiff(_currentTime,_updatedTime);				//�O��FPS�X�V���Ƃ̍���
	auto frameTime = GetLIntDiff(_currentTime, _beforeTime);		//�O�t���[���Ƃ̍���

	if (frameTime < FRAME_TIME)										//�������Ԃɗ]�T������ꍇ�A�҂����킹���s��
	{
		DWORD sleepTime =
			static_cast<DWORD>((FRAME_TIME - frameTime) * 1000);
		timeBeginPeriod(1);

		Sleep(sleepTime);

		timeEndPeriod(1);
	}

	if (diff >= 1)													//������1�b�ȏゾ�����ꍇ
	{
		_fps = 1 / frameTime;										//1�b�������Ŋ���AFPS���擾

		_updatedTime = _currentTime;								//FPS�X�V���Ԃ��X�V
	}

	_beforeTime = _currentTime;										//�O�t���[���̎��Ԃ��X�V
}

/// <summary>
/// �����_�����󂯎����񏈗����s���֐�
/// </summary>
/// <param name="func">����ɏ��������������_��</param>
void
SceneBase::ParallelProcess(function<void(void)> func)
{
	thread th(func);	//�����_������������X���b�h���쐬
	th.detach();		//�X���b�h�̊Ǘ��������
}
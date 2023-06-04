#include "Actor/AssimpLoader.h"
#include "Actor/FBXActor.h"
#include "Manager/ImGuiManager.h"
#include "Scene/PlayScene.h"

const wchar_t* UNITY_PATH = L"Asset/model/Unitychan.fbx";		//���f���f�[�^���i�[����Ă���t�@�C���p�X

/// <summary>
/// �R���X�g���N�^
/// </summary>
/// <param name="dx12">Dx12Wrapper�C���X�^���X</param>
/// <param name="input">InputManager�C���X�^���X</param>
/// <param name="sound">SoundManager�C���X�^���X</param>
PlayScene::PlayScene(
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
PlayScene::~PlayScene()
{
	
}

/// <summary>
/// �X�V����
/// </summary>
void
PlayScene::Update()
{
	InputUpdate();																//���͂��X�V

	if (_isPlaying)																//����\�ȏꍇ���͂𔽉f����
	{
		//WASD�L�[�̓��͂ɉ����Ď��_���ړ�������
		//�����_�𒆐S�Ɏ��_����]
		if (_input.KeyState().A)
		{
			_dx12.RotateSphericalCoordinates(Degree::Azimth, 90);
		}
		if (_input.KeyState().D)
		{
			_dx12.RotateSphericalCoordinates(Degree::Azimth, -90);
		}
		if (_input.KeyState().W)
		{
			_dx12.RotateSphericalCoordinates(Degree::Elevation, 45);
		}
		if (_input.KeyState().S)
		{
			_dx12.RotateSphericalCoordinates(Degree::Elevation, -45);
		}

		//Q�E�L�[�����������͊g��E�k��
		//���V�t�g�L�[�������Ă���ꍇ�̓J�������㉺�ɕ��s�ړ�������
		if (_input.KeyState().Q)
		{
			if (_input.KeyState().LeftShift)
			{
				_dx12.TranslateSphericalCoordinates(Direction::Vertical, -90);
			}
			else
			{
				_dx12.ScalingSphericalCoordinates(-90);
			}
		}
		if (_input.KeyState().E)
		{
			if (_input.KeyState().LeftShift)
			{
				_dx12.TranslateSphericalCoordinates(Direction::Vertical, 90);
			}
			else
			{
				_dx12.ScalingSphericalCoordinates(90);
			}
		}
	}

	DrawUpdate();																//�`�揈��
}

/// <summary>
/// �V�[���J�n���Ɏ��s����֐�
/// </summary>
void
PlayScene::SceneStart()
{
	_dx12.ResetSphericalCoordinates();									//�J�����ʒu�����Z�b�g

	_direction = XMLoadFloat3(new XMFLOAT3(0.0f, 0.0f, 0.0f));			//�ړ��x�N�g����������

	//���f���ǂݍ��ݏ���
	auto f = [&]()
	{
		_isLoading = true;												//���[�h�����Ǝ���

		vector<thread> ths;												//���񏈗��������X���b�h�̃x�N�g��

		ths.emplace_back												//�X���b�h�̒ǉ�
		(	
			[&](){
				_actor.reset(new FBXActor(_dx12, UNITY_PATH));			//Unitychan�C���X�^���X�̒ǉ�
			}
		);

		for (auto& th : ths)											//�X���b�h�̎��s�����҂�
		{
			th.join();
		}

		_dx12.ImGui()->SetActor(_actor);								//�A�N�^�[��ImGuiManager���ɓn��
		_dx12.ImGui()->SetCurrentAnimStr(_actor->GetCurentAnimStr());	//�A�N�^�[�����s����A�j���[�V��������ImGuiManager���ɓn��

		_isLoading = false;												//���[�h����

		SceneBase::SceneStart();										//�t�F�[�h����
	};
	ParallelProcess(f);													//���f���ǂݍ��ݏ�����ʏ�̕`�揈���ƕ��s���čs��
}

/// <summary>
/// �V�[���J�ڎ��Ɏ��s����֐�
/// </summary>
void
PlayScene::SceneEnd()
{
	_actor = nullptr;					//�A�N�^�[���J��

	_dx12.ImGui()->ResetAnimStr();		//ImGui�ŕ\������Ă���A�j���[�V��������������
}

/// <summary>
/// UI��`�悷��֐�
/// </summary>
void
PlayScene::BackGroundDraw()
{
	SceneBase::BackGroundDraw();		//�w�i��`��
}

/// <summary>
/// ���f���`����s���֐�
/// </summary>
void
PlayScene::ModelDraw()
{
	if (!_isLoading)			//�v���C���[���ǂݍ��݊������Ă���ꍇ�`�揈��
	{
		if (_actor != nullptr)
		{
			_actor->Draw();		//�`�揈��
			_actor->Update();	//�X�V����
		}
	}
}

/// <summary>
/// �G�t�F�N�g��̕`����s���֐�
/// </summary>
void
PlayScene::EffectAndUIDraw()
{
	SceneBase::EffectAndUIDraw();		//�}�E�X�J�[�\����`��

	if (_isLoading)						//���[�h���̏ꍇ
	{
		_dx12.Sprite()->LoadingDraw();	//���[�h��ʂ�`��
	}
	else if(_isPlaying)					//����\�ȏꍇ
	{
		_dx12.ImGui()->SetFPS(_fps);	//Scene����ImGui�Ƀf�[�^��n��
		_dx12.ImGui()->ImGuiDraw();		//ImGui�̃E�B���h�E��`��
	}
}
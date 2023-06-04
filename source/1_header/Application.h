#pragma once
#include <DirectXTex-master/DirectXTex/d3dx12.h>
#include <DirectXTex-master/DirectXTex/DirectXTex.h>

#include <d3dcompiler.h>
#include <array>
#include <cstdlib>
#include <map>
#include <tchar.h>
#include <dxgi1_6.h>
#include <vector>
#include <thread>
#include <mutex>
#include <functional>
#include <chrono>
#include <iostream>
#include <fstream>
#include <filesystem>

using namespace std;
using namespace DirectX;
using namespace Microsoft::WRL;

#if _DEBUG
#pragma comment(lib,"assimp-vc143-mtd.lib")
#else
#pragma comment(lib,"assimp-vc143-mt.lib")
#endif // DEBUG


#pragma comment(lib,"DirectXTK12.lib")
#pragma comment(lib,"DirectXTex.lib")
#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib,"winmm.lib")

const float PI = 3.141592653f;					//�~����

/// <summary>
/// �ǂݍ���Scene�N���X�����ʂ��邽�߂̗񋓌^
/// </summary>
enum SceneNames
{
	Title,		//�^�C�g���V�[��
	Play,		//�Q�[���V�[��
};

class Dx12Wrapper;
class InputManager;
class Package;
class PlayScene;
class SceneBase;
class SoundManager;
class TitleScene;
/// <summary>
/// �Q�[���̏������E�X�V�E�I�����Ǘ�����N���X
/// </summary>
class Application
{
private:
	WNDCLASSEX _windowClass;										//�E�B���h�E�쐬���ɕK�v�ȏ����i�[
	HWND _hwnd;														//�E�B���h�E�̎��ʂɕK�v�Ȓl

	shared_ptr<Dx12Wrapper> _dx12;									//Dx12Wrapper�C���X�^���X
	shared_ptr<InputManager> _input;								//InputManager�C���X�^���X	
	shared_ptr<SoundManager> _sound;								//SoundManager�C���X�^���X

	shared_ptr<TitleScene> _title;									//TitleScene�C���X�^���X
	shared_ptr<PlayScene> _play;									//PlayScene�C���X�^���X

	shared_ptr<SceneBase> _Scene;									//�X�V�������s���C���X�^���X

	shared_ptr<Package> _package;									

	float _deltaTime;												//�t���[���Ԃ̌o�ߎ���

	int _rate;														//1�b������̃t���[����
	int _interval;													//�t���b�v�Ԋu

	void CreateGameWindow(HWND& hwnd, WNDCLASSEX& windowClass);		//�Q�[���p�E�B���h�E���쐬����֐�

	Application();													//�R���X�g���N�^
	Application(const Application&) = delete;						//�R���X�g���N�^���O������Ăяo����Ȃ��悤�ݒ�

public:

	MSG _msg = {};									//���b�Z�[�W�p�\����

	static Application& Instance();					//�C���X�^���X�̎Q�Ƃ�Ԃ��H

	shared_ptr<SceneBase> GetCurrentScene();		//���݂̃V�[����Ԃ��֐�

	bool Init();									//������

	void Run();										//�Q�[����ʂ̕`��

	void Terminate();								//�Q�[���I�����̌�n��

	SIZE GetWindowSize()const;						//�E�B���h�E�T�C�Y��Ԃ�

	void SetScene(shared_ptr<SceneBase> scene);		//�V�[����؂�ւ���
	void ChangeScene(SceneNames name);				//�V�[���̐ݒ�E�I�����̏����E�J�n���̏��������s
	void ExitApp();									//�A�v���P�[�V�������I������

	int GetInterval()const;							//�����_�[�^�[�Q�b�g�̃t���b�v�Ԋu��Ԃ��֐�
	int GetRate()const;								//1�b�Ԃ̃t���[�����[�g��Ԃ��֐�

	~Application();									//�f�X�g���N�^
};

#pragma once
#include "Application.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx12.h"
#include "imgui/imgui_impl_win32.h"

class Dx12Wrapper;
class FBXActor;
class ImGuiManager
{
public:
	ImGuiManager(Dx12Wrapper& dx12,HWND hwnd);	//�R���X�g���N�^

	void ImGuiDraw();							//�e�E�B���h�E�̕`�揈��

	void SetFPS(float fps);						//PlayScene����󂯎�����f�[�^�𔽉f����֐�
	void SetActor(shared_ptr<FBXActor> actor);	//�A�N�^�[���󂯎��֐�
	void SetCurrentAnimStr(string str);			//�A�N�^�[�����ݎ��s���Ă���A�j���[�V���������󂯎��֐�
	void ResetAnimStr();						//�A�j���[�V�������̔z�������������֐�

private:
	Application& _app;							//Application�C���X�^���X
	Dx12Wrapper& _dx12;							//Dx12Wrapper�C���X�^���X

	shared_ptr<FBXActor> _actor;				//�A�N�^�[

	string _currentAnimStr;						//�A�N�^�[�����ݎ��s���Ă���A�j���[�V������
	vector<string> _animStr;					//�S�A�j���[�V������

	ImGuiWindowFlags _fpsFlag = 0;				//FPS�\���p�E�B���h�E�̐ݒ�t���O
	ImGuiWindowFlags _animFlag = 0;				//�A�j���[�V�����\���p�E�B���h�E�̐ݒ�t���O

	bool _isLoop = false;						//�A�N�^�[�̃A�j���[�V���������[�v���邩�ǂ������߂�^���l

	float _windowWidth;							//�E�B���h�E��
	float _windowHeight;							//�E�B���h�E����
	float _size;								//�t�H���g�T�C�Y
	float _fps;									//1�b������̃t���[�����[�g
};
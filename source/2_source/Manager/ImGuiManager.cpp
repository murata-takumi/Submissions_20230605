#include "Actor/FBXActor.h"
#include "Manager/ImGuiManager.h"
#include "Scene/SceneBase.h"

const int DIFF = 15;				//��ʒ[�Ɗe�E�B���h�E�̊Ԃ̋���
const int BUTTON_WIDTH = 300;		//�{�^���p�E�B���h�E�̕�
const int BUTTON_HEIGHT = 200;		//�{�^���p�E�B���h�E�̍���
const int ANIMATION_WIDTH = 350;	//�A�j���[�V�����p�E�B���h�E�̕�

/// <summary>
/// �R���X�g���N�^
/// ImGui�֘A�̏��������s��
/// </summary>
/// <param name="dx12">Dx12Wrapper�C���X�^���X</param>
/// <param name="hwnd">�E�B���h�E�n���h��</param>
ImGuiManager::ImGuiManager(Dx12Wrapper& dx12, HWND hwnd)
	:_dx12(dx12),_app(Application::Instance()),_windowWidth(_app.GetWindowSize().cx),_windowHeight(_app.GetWindowSize().cy),
	_size(18.0f), _fps(0.0f)
{
	_fpsFlag |= ImGuiWindowFlags_NoMove;								//FPS��\������E�B���h�E�������Ȃ��悤�ݒ�
	_fpsFlag |= ImGuiWindowFlags_NoTitleBar;							//�^�C�g���o�[��\�����Ȃ�
	_fpsFlag |= ImGuiWindowFlags_NoResize;								//�E�B���h�E�T�C�Y��ς��Ȃ�
	_fpsFlag |= ImGuiWindowFlags_NoScrollbar;							//�X�N���[���o�[��\�����Ȃ�

	_animFlag |= ImGuiWindowFlags_NoMove;								//�A�j���[�V�����\���E�B���h�E�����l�ɐݒ�
	_animFlag |= ImGuiWindowFlags_NoResize;								//�E�B���h�E�T�C�Y��ς��Ȃ�

	if (ImGui::CreateContext() == nullptr)								//imgui�̏�����
	{
		assert(0);
		return;
	}
	bool bInResult = ImGui_ImplWin32_Init(hwnd);					
	if (!bInResult)
	{
		assert(0);
		return;
	}
	bInResult = ImGui_ImplDX12_Init(
		_dx12.Device(),
		3,
		DXGI_FORMAT_R8G8B8A8_UNORM,
		_dx12.HeapForImgui(),
		_dx12.HeapForImgui()->GetCPUDescriptorHandleForHeapStart(),
		_dx12.HeapForImgui()->GetGPUDescriptorHandleForHeapStart());
	if (!bInResult)
	{
		assert(0);
		return;
	}

	ImGuiIO& io = ImGui::GetIO();										//�t�H���g��ݒ�
	io.Fonts->AddFontFromFileTTF(
		"Asset/font/BIZUDPGothic-Bold.ttf",
		_size,NULL,io.Fonts->GetGlyphRangesJapanese());
}

/// <summary>
/// �e��E�B���h�E�̕`�����������֐�
/// </summary>
void
ImGuiManager::ImGuiDraw()
{
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	//FPS�E�B���h�E�̕\������
	{
		ImGui::Begin("FPS", 0, _fpsFlag);

		ImGui::SetWindowPos(ImVec2(DIFF,DIFF));												//�E�B���h�E�ʒu������
		ImGui::SetWindowSize(ImVec2(_size * 8, _size));										//�E�B���h�E�T�C�Y��ݒ�

		if (_fps <= 30.0f)ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));	//FPS��30�ȉ��̏ꍇ�����̐F��Ԃ�����
		ImGui::Text("FPS:%.1f", _fps);														//FPS��\��
		if (_fps <= 30.0f)ImGui::PopStyleColor();

		ImGui::End();
	}

	//�{�^�����܂Ƃ߂��E�B���h�E�̕\������
	{
		ImGui::Begin("Controll",0,_animFlag);
		ImGui::SetWindowPos(ImVec2(DIFF, _windowHeight - (BUTTON_HEIGHT + DIFF)));
		ImGui::SetWindowSize(ImVec2(BUTTON_WIDTH, BUTTON_HEIGHT));

		if (ImGui::Button("CameraReset"))													//�{�^���������ꂽ��J���������Z�b�g
		{
			_dx12.ResetSphericalCoordinates();										
		}

		if (ImGui::Button("Exit"))															//�{�^���������ꂽ��ʃV�[���֑J��
		{
			_app.GetCurrentScene()->ChangeScene(SceneNames::Title);
		}

		ImGui::End();
	}

	//�A�j���[�V����������ׂ��E�B���h�E�̕\������
	{
		ImGui::Begin("Animation",0,_animFlag);

		ImGui::SetWindowPos(ImVec2(_windowWidth - (ANIMATION_WIDTH + DIFF), DIFF));			//�E�B���h�E���W��ݒ�
		ImGui::SetWindowSize(ImVec2(ANIMATION_WIDTH, _windowHeight - (DIFF * 2)));			//�E�B���h�E�T�C�Y��ݒ�

		ImGui::Text(_currentAnimStr.c_str());												//���ݎ��s���Ă���A�j���[�V��������\��

		int count = 0;

		for (auto str : _animStr)															//�A�j���[�V����������ׂ�
		{
			if (ImGui::Button(str.c_str(),ImVec2(160,0)))									//�{�^������������
			{
				_actor->SetAnimStr(str);													//���s����A�j���[�V�������w��

				SetCurrentAnimStr(str);														//���ݎ��s���Ă���A�j���[�V���������X�V
			}

			if (count++ % 2 == 0)
			{
				ImGui::SameLine();
			}
		}

		if (ImGui::Checkbox("Loop",&_isLoop))												//�A�j���[�V�����̃��[�v���w�肷��
		{
			_actor->SetLoop(_isLoop);
		}
		
		ImGui::End();
	}

	ImGui::Render();

	ID3D12DescriptorHeap* heap[] = { _dx12.HeapForImgui() };								//ImGui�p�q�[�v���Z�b�g
	_dx12.CommandList()->SetDescriptorHeaps(1, heap);
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), _dx12.CommandList());
}

/// <summary>
/// �V�[������󂯎�����f�[�^�𔽉f����֐�
/// </summary>
/// <param name="data"FPS</param>
void
ImGuiManager::SetFPS(float fps)
{
	_fps = fps;
}

/// <summary>
/// �A�N�^�[���󂯎��֐�
/// </summary>
/// <param name="actor">�A�N�^�[</param>
void
ImGuiManager::SetActor(shared_ptr<FBXActor> actor)
{
	_actor = actor;							//�A�N�^�[���i�[

	for (auto str : _actor->GetAnimstr())	//�A�N�^�[�̃A�j���[�V�������������瑤�Ɋi�[����
	{
		_animStr.push_back(str);
	}
}

/// <summary>
/// �A�N�^�[�����ݎ��s���Ă���A�j���[�V���������󂯎��֐�
/// </summary>
/// <param name="str">�A�j���[�V������</param>
void
ImGuiManager::SetCurrentAnimStr(string str)
{
	_currentAnimStr = "Current Animation : " + str;
}

/// <summary>
/// �A�j���[�V�������̔z�������������֐�
/// </summary>
void
ImGuiManager::ResetAnimStr()
{
	_animStr.clear();
}
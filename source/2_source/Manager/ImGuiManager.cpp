#include "Actor/FBXActor.h"
#include "Manager/ImGuiManager.h"
#include "Scene/SceneBase.h"

const int DIFF = 15;				//画面端と各ウィンドウの間の距離
const int BUTTON_WIDTH = 300;		//ボタン用ウィンドウの幅
const int BUTTON_HEIGHT = 200;		//ボタン用ウィンドウの高さ
const int ANIMATION_WIDTH = 350;	//アニメーション用ウィンドウの幅

/// <summary>
/// コンストラクタ
/// ImGui関連の初期化を行う
/// </summary>
/// <param name="dx12">Dx12Wrapperインスタンス</param>
/// <param name="hwnd">ウィンドウハンドル</param>
ImGuiManager::ImGuiManager(Dx12Wrapper& dx12, HWND hwnd)
	:_dx12(dx12),_app(Application::Instance()),_windowWidth(_app.GetWindowSize().cx),_windowHeight(_app.GetWindowSize().cy),
	_size(18.0f), _fps(0.0f)
{
	_fpsFlag |= ImGuiWindowFlags_NoMove;								//FPSを表示するウィンドウが動かないよう設定
	_fpsFlag |= ImGuiWindowFlags_NoTitleBar;							//タイトルバーを表示しない
	_fpsFlag |= ImGuiWindowFlags_NoResize;								//ウィンドウサイズを変えない
	_fpsFlag |= ImGuiWindowFlags_NoScrollbar;							//スクロールバーを表示しない

	_animFlag |= ImGuiWindowFlags_NoMove;								//アニメーション表示ウィンドウも同様に設定
	_animFlag |= ImGuiWindowFlags_NoResize;								//ウィンドウサイズを変えない

	if (ImGui::CreateContext() == nullptr)								//imguiの初期化
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

	ImGuiIO& io = ImGui::GetIO();										//フォントを設定
	io.Fonts->AddFontFromFileTTF(
		"Asset/font/BIZUDPGothic-Bold.ttf",
		_size,NULL,io.Fonts->GetGlyphRangesJapanese());
}

/// <summary>
/// 各種ウィンドウの描画を実装する関数
/// </summary>
void
ImGuiManager::ImGuiDraw()
{
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	//FPSウィンドウの表示処理
	{
		ImGui::Begin("FPS", 0, _fpsFlag);

		ImGui::SetWindowPos(ImVec2(DIFF,DIFF));												//ウィンドウ位置を決定
		ImGui::SetWindowSize(ImVec2(_size * 8, _size));										//ウィンドウサイズを設定

		if (_fps <= 30.0f)ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));	//FPSが30以下の場合文字の色を赤くする
		ImGui::Text("FPS:%.1f", _fps);														//FPSを表示
		if (_fps <= 30.0f)ImGui::PopStyleColor();

		ImGui::End();
	}

	//ボタンをまとめたウィンドウの表示処理
	{
		ImGui::Begin("Controll",0,_animFlag);
		ImGui::SetWindowPos(ImVec2(DIFF, _windowHeight - (BUTTON_HEIGHT + DIFF)));
		ImGui::SetWindowSize(ImVec2(BUTTON_WIDTH, BUTTON_HEIGHT));

		if (ImGui::Button("CameraReset"))													//ボタンが押されたらカメラをリセット
		{
			_dx12.ResetSphericalCoordinates();										
		}

		if (ImGui::Button("Exit"))															//ボタンが押されたら別シーンへ遷移
		{
			_app.GetCurrentScene()->ChangeScene(SceneNames::Title);
		}

		ImGui::End();
	}

	//アニメーション名を並べたウィンドウの表示処理
	{
		ImGui::Begin("Animation",0,_animFlag);

		ImGui::SetWindowPos(ImVec2(_windowWidth - (ANIMATION_WIDTH + DIFF), DIFF));			//ウィンドウ座標を設定
		ImGui::SetWindowSize(ImVec2(ANIMATION_WIDTH, _windowHeight - (DIFF * 2)));			//ウィンドウサイズを設定

		ImGui::Text(_currentAnimStr.c_str());												//現在実行しているアニメーション名を表示

		int count = 0;

		for (auto str : _animStr)															//アニメーション名を並べる
		{
			if (ImGui::Button(str.c_str(),ImVec2(160,0)))									//ボタンを押したら
			{
				_actor->SetAnimStr(str);													//実行するアニメーションを指定

				SetCurrentAnimStr(str);														//現在実行しているアニメーション名を更新
			}

			if (count++ % 2 == 0)
			{
				ImGui::SameLine();
			}
		}

		if (ImGui::Checkbox("Loop",&_isLoop))												//アニメーションのループを指定する
		{
			_actor->SetLoop(_isLoop);
		}
		
		ImGui::End();
	}

	ImGui::Render();

	ID3D12DescriptorHeap* heap[] = { _dx12.HeapForImgui() };								//ImGui用ヒープをセット
	_dx12.CommandList()->SetDescriptorHeaps(1, heap);
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), _dx12.CommandList());
}

/// <summary>
/// シーンから受け取ったデータを反映する関数
/// </summary>
/// <param name="data"FPS</param>
void
ImGuiManager::SetFPS(float fps)
{
	_fps = fps;
}

/// <summary>
/// アクターを受け取る関数
/// </summary>
/// <param name="actor">アクター</param>
void
ImGuiManager::SetActor(shared_ptr<FBXActor> actor)
{
	_actor = actor;							//アクターを格納

	for (auto str : _actor->GetAnimstr())	//アクターのアニメーション名をこちら側に格納する
	{
		_animStr.push_back(str);
	}
}

/// <summary>
/// アクターが現在実行しているアニメーション名を受け取る関数
/// </summary>
/// <param name="str">アニメーション名</param>
void
ImGuiManager::SetCurrentAnimStr(string str)
{
	_currentAnimStr = "Current Animation : " + str;
}

/// <summary>
/// アニメーション名の配列を初期化する関数
/// </summary>
void
ImGuiManager::ResetAnimStr()
{
	_animStr.clear();
}
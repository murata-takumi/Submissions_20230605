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
	ImGuiManager(Dx12Wrapper& dx12,HWND hwnd);	//コンストラクタ

	void ImGuiDraw();							//各ウィンドウの描画処理

	void SetFPS(float fps);						//PlaySceneから受け取ったデータを反映する関数
	void SetActor(shared_ptr<FBXActor> actor);	//アクターを受け取る関数
	void SetCurrentAnimStr(string str);			//アクターが現在実行しているアニメーション名を受け取る関数
	void ResetAnimStr();						//アニメーション名の配列を初期化する関数

private:
	Application& _app;							//Applicationインスタンス
	Dx12Wrapper& _dx12;							//Dx12Wrapperインスタンス

	shared_ptr<FBXActor> _actor;				//アクター

	string _currentAnimStr;						//アクターが現在実行しているアニメーション名
	vector<string> _animStr;					//全アニメーション名

	ImGuiWindowFlags _fpsFlag = 0;				//FPS表示用ウィンドウの設定フラグ
	ImGuiWindowFlags _animFlag = 0;				//アニメーション表示用ウィンドウの設定フラグ

	bool _isLoop = false;						//アクターのアニメーションがループするかどうか決める真理値

	float _windowWidth;							//ウィンドウ幅
	float _windowHeight;							//ウィンドウ高さ
	float _size;								//フォントサイズ
	float _fps;									//1秒当たりのフレームレート
};
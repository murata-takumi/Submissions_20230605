#pragma once
#include "Application.h"

#include <DirectXTK12-master/Inc/CommonStates.h>
#include <DirectXTK12-master/Inc/ResourceUploadBatch.h>
#include <DirectXTK12-master/Inc/SpriteBatch.h>
#include <DirectXTK12-master/Inc/SpriteFont.h>

#include <sstream>
#include <time.h>

/// <summary>
/// 画像や文字フォントを管理するクラス
/// </summary>
class Dx12Wrapper;
class SpriteManager
{
	template<typename T>
	using ComPtr = ComPtr<T>;

private:
	unique_ptr<GraphicsMemory> _gmemory;									//グラフィックスメモリ
	unique_ptr<SpriteFont> _spriteFont;										//フォント表示用オブジェクト
	unique_ptr<SpriteBatch> _spriteBatch;									//スプライト（画像）表示用オブジェクト

	Dx12Wrapper& _dx12;														//Dx12Wrapperインスタンス

	ID3DBlob* _psBlob = nullptr;											//ピクセルシェーダー用データ

	SIZE _winSize;															//ウィンドウサイズ
	LONG _width;															//画面の幅
	LONG _height;															//画面の高さ

	ID3D12DescriptorHeap* _heapForSpriteFont = nullptr;						//フォント・画像用ヒープ
	D3D12_CPU_DESCRIPTOR_HANDLE _tmpCPUHandle;								//ヒープハンドル(CPU)
	D3D12_GPU_DESCRIPTOR_HANDLE _tmpGPUHandle;								//ヒープハンドル(GPU)
	UINT _incrementSize;													//ハンドルのアドレスの差分

	map<string,D3D12_GPU_DESCRIPTOR_HANDLE> _GPUHandles;					//ハンドル(GPU)のベクトル

	RECT _backGroundRect;													//背景用矩形
	RECT _startButtonRect;													//タイトル画面でのスタートボタン用矩形
	RECT _endButtonRect;													//タイトル画面での終了ボタン用矩形
	RECT _loadingRect;														//ロード中アイコン用矩形
	RECT _titleRect;														//タイトル画面の文字列用矩形

	LONG _buttonLeft,_buttonRight,_titleWidth,_titleHeight;

	ComPtr<ID3D12RootSignature> _spriteRS;									//スプライト描画用ルートシグネチャ

	int _x;																	//マウスのX座標
	int _y;																	//マウスのY座標

	HRESULT CreateSpriteRS();												//SpriteBatch用ルートシグネチャを作成する関数
	HRESULT CreateUIBufferView(const wchar_t* path, string key);			//UI用の画像のバッファー・ビューを作成する関数

	XMFLOAT2 GetCenterPos(
		RECT rect, const wchar_t* wstr, float rectWidth, float rectHeight);	//矩形から文章を中央揃えに出来る座標を取得する

	bool IsInRect(RECT rect);												//マウスカーソルが矩形内に存在するか確認する関数

	void InitSpriteDevices();												//スプライト・文字列表示用オブジェクトを初期化する関数
	void ShiftHandles();													//CPU,GPU用ハンドルをずらす関数
public:
	SpriteManager(Dx12Wrapper& dx12,LONG width,LONG height);	//コンストラクタ

	bool TitleIsOnStart();										//タイトル画面でのスタートボタンの上にマウスがあるかチェック
	bool TitleIsOnEnd();										//タイトル画面での終了ボタンの上にマウスがあるかチェック

	void AdjustSpriteRect();									//画面サイズの変更を感知して矩形を調整する

	void TitleDraw();											//タイトル画面でのUIを描画

	void BackGroundDraw();										//背景を描画
	void LoadingDraw();											//ロード画面での描画
	void CursorDraw();											//マウスカーソルを描画

	void Commit();												//グラフィックスメモリをコマンドリストにセット
	void SetMousePosition(int x,int y);							//マウス座標を更新
};
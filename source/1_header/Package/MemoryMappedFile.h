#pragma once
#include "Application.h"

/// <summary>
/// パッケージ化するファイルのデータを保存するクラス
/// </summary>
class MemoryMappedFile
{
protected:
	HANDLE _hFile;		//ファイルのハンドル
	HANDLE _hMap;		//メモリマップのハンドル
	void* _pPointer;	//ファイルデータの先頭ポインタ
	DWORD _fileSize;	//ファイルサイズ

public:
	MemoryMappedFile();											//コンストラクタ
	~MemoryMappedFile();										//デストラクタ

	void Release();												//データを全て解放・初期化する関数

	bool Open(													//ファイルを開く関数
		const wchar_t* fileName,
		DWORD readWriteFlag = GENERIC_READ | GENERIC_WRITE,
		DWORD openFlag = OPEN_EXISTING
	);

	void* GetPtr();												//ファイルデータの先頭ポインタを返す関数

	DWORD GetSize();											//ファイルサイズを返す関数
};
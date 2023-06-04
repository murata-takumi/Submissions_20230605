#include "Package/MemoryMappedFile.h"

/// <summary>
/// コンストラクタ
/// 各変数を初期化する
/// </summary>
MemoryMappedFile::MemoryMappedFile()
	:_hFile(NULL), _hMap(NULL), _pPointer(nullptr), _fileSize(0)
{

}

/// <summary>
/// デストラクタ
/// このインスタンスを開放する
/// </summary>
MemoryMappedFile::~MemoryMappedFile()
{
	this->Release();
}

/// <summary>
/// データを全て解放・初期化する関数
/// </summary>
void
MemoryMappedFile::Release()
{
	if (_pPointer != nullptr)
	{
		UnmapViewOfFile(_pPointer);
		_pPointer = nullptr;
	}
	if (_hMap != NULL)
	{
		CloseHandle(_hMap);
		_hMap = NULL;
	}
	if (_hFile != NULL)
	{
		CloseHandle(_hFile);
		_hFile = NULL;
	}
	_fileSize = 0;
}

/// <summary>
/// ファイルを開く関数
/// </summary>
/// <param name="fileName">ファイル名</param>
/// <param name="readWriteFlag">読み書きフラグ</param>
/// <param name="openFlag">ファイルオープンフラグ</param>
/// <returns>処理が成功したかどうか</returns>
bool
MemoryMappedFile::Open(
	const wchar_t* fileName,DWORD readWriteFlag, DWORD openFlag
)
{
	_hFile = CreateFile(fileName, readWriteFlag, 0, 0, openFlag,		//ファイルを開く
		FILE_ATTRIBUTE_NORMAL, NULL);
	if (_hFile == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	DWORD mapFlag = PAGE_READWRITE;										//ファイルマッピングオブジェクトを作成
	if (readWriteFlag == GENERIC_READ)
	{
		mapFlag = PAGE_READONLY;
	}
	_hMap = CreateFileMapping(_hFile, 0, mapFlag, 0, 0, fileName);
	if (_hMap == INVALID_HANDLE_VALUE)
	{
		CloseHandle(_hFile);
		_hFile = NULL;
		return false;
	}

	DWORD mapViewFlag = FILE_MAP_WRITE;									//ポインタを取得
	if (mapFlag == PAGE_READONLY)
	{
		mapViewFlag = FILE_MAP_READ;
	}
	_pPointer = MapViewOfFile(_hMap, mapViewFlag, 0, 0, 0);
	if (_pPointer == nullptr)
	{
		CloseHandle(_hMap);
		CloseHandle(_hFile);
		_hMap = NULL;
		_hFile = NULL;
		return false;
	}

	DWORD fileSize;														//ファイルサイズを取得
	_fileSize = ::GetFileSize(_hFile, &fileSize);

	return true;
}

/// <summary>
/// ファイルデータの先頭ポインタを返す関数
/// </summary>
/// <returns>ポインタ</returns>
void*
MemoryMappedFile::GetPtr()
{
	return _pPointer;
}

/// <summary>
/// ファイルサイズを返す関数
/// </summary>
/// <returns>ファイルサイズ</returns>
DWORD
MemoryMappedFile::GetSize()
{
	return _fileSize;
}
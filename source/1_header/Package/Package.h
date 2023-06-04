#pragma once
#include "Package/MemoryMappedFile.h"

const int PACKAGE_FORMAT_MAX = 32;					//フォーマット名の最大長さ

/// <summary>
/// ヘッダー
/// </summary>
struct PackageHeader
{
	wchar_t formatName[PACKAGE_FORMAT_MAX];
	UINT fileNum;
};

/// <summary>
/// ファイルデータ
/// </summary>
struct FileData
{
	wchar_t fileName[MAX_PATH];	//ファイル名
	UINT size;						//ファイルサイズ
	BYTE* ptr;						//ファイルの先頭ポインタ
	BYTE tail;						//ファイルの末尾
};

/// <summary>
/// ファイルのパッケージ化、読み込みを実装するクラス
/// </summary>
class Package
{
private:
	wchar_t _formatName[PACKAGE_FORMAT_MAX];						//フォーマット名
	list<basic_string<wchar_t>> _fileList;							//ファイル名のリスト
	map<basic_string<wchar_t>, FileData> _fileDataMap;				//ファイル名とファイルデータの連想配列
	MemoryMappedFile _packageFile;									//パッケージファイル

	void EnumFiles(const wchar_t* dir, const wchar_t* wildCard);	//ファイルを列挙する関数
public:
	Package();																	//コンストラクタ
	~Package();																	//デストラクタ

	bool SetFormatName(wchar_t formatname[PACKAGE_FORMAT_MAX]);					//フォーマット名を登録する関数
	bool CreatePackage(const wchar_t* dirName, const wchar_t* outFileName);		//パッケージを作成する関数
	bool OpenPackage(const wchar_t* fileName);									//パッケージを開く関数
	bool LoadPackage(BYTE* ptr);												//パッケージ内のデータを読み込む関数

	bool GetFileData(const wchar_t* fileName, FileData* pOutData);				//ファイルのデータを読み込む関数
	
	int GetFileNum();															//ファイル数を取得する関数

	bool CreateDirectoryAndFile(const basic_string<wchar_t> fileName);			//ディレクトリとその中のファイルを作成する関数
	bool RemoveFileInDiretory(const basic_string<wchar_t> dirName);				//ディレクトリ内のファイルをすべて削除する関数
};
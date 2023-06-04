#include "Package/Package.h"

#include <fstream>
#include <filesystem>
#include <fileapi.h>

/// <summary>
/// 文字列を置換する関数
/// </summary>
/// <param name="source">置換対象の文字列</param>
/// <param name="pattern">置換対象のパターン</param>
/// <param name="placement">置換する文字列</param>
/// <returns>置換後の文字列</returns>
basic_string<wchar_t> _treplace(
	const basic_string<wchar_t>& source,
	const basic_string<wchar_t>& pattern,
	const basic_string<wchar_t>& placement
)
{
	basic_string<wchar_t> result;													//返り値
	basic_string<wchar_t>::size_type pos_before = 0;								//検索位置
	basic_string<wchar_t>::size_type pos = 0;										//文字列の初期位置
	basic_string<wchar_t>::size_type len = pattern.size();							//パターンの長さ

	while ((pos = source.find(pattern, pos)) != basic_string<wchar_t>::npos){		//文字列内に置換対象が存在する場合
		result.append(source, pos_before, pos - pos_before);						//返り値に置換対象手前までの文字列を追加
		result.append(placement);													//末尾に置換文字列を追加
		pos += len;																	//初期位置を更新
		pos_before = pos;															//検索位置を更新
	}

	result.append(source, pos_before, source.size() - pos_before);					//置換対象以降の文字列を追加
	return result;
}

/// <summary>
/// ファイルを列挙する関数
/// </summary>
/// <param name="dir">ディレクトリパス</param>
/// <param name="wildCard">ワイルドカード</param>
void
Package::EnumFiles(const wchar_t* dir, const wchar_t* wildCard)
{
	basic_string<wchar_t> currentDir = dir;
	basic_string<wchar_t> relatedDir = dir;
	currentDir += _T("/");
	currentDir += wildCard;
	WIN32_FIND_DATA fd = {};
	HANDLE hFind = FindFirstFile(currentDir.c_str(), &fd);										//ディレクトリを検索
	if (hFind == INVALID_HANDLE_VALUE)
	{
		return;
	}

	do {																						//ディレクトリ内の全データに対し実行

		if (_tcscmp(fd.cFileName, _T(".")) != 0 && _tcscmp(fd.cFileName, _T("..")) != 0)
		{
			if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)									//データがディレクトリだった場合			
			{
				basic_string<wchar_t> childDir = dir;											//子ディレクトリに対し再帰処理
				childDir += _T("/");
				childDir += fd.cFileName;
				this->EnumFiles(childDir.c_str(), wildCard);
			}
			else																				//データがファイルだった場合
			{
				basic_string<wchar_t> tmp = relatedDir;											//連想配列に追加
				tmp += _T("/");
				tmp += fd.cFileName;

				basic_string<wchar_t> correct = _treplace(tmp,_T("\\"),_T("/"));
				_fileList.push_back(correct.c_str());
			}
		}

	} while (FindNextFile(hFind, &fd) == TRUE);
}

/// <summary>
/// コンストラクタ
/// フォーマット名を初期化する
/// </summary>
Package::Package()
{
	memset(_formatName, 0, sizeof(_formatName));
}

/// <summary>
/// デストラクタ
/// </summary>
Package::~Package()
{

}

/// <summary>
/// フォーマット名を登録する関数
/// </summary>
/// <param name="formatname">登録したいフォーマット名</param>
/// <returns>処理が成功したかどうか</returns>
bool 
Package::SetFormatName(wchar_t formatname[PACKAGE_FORMAT_MAX])
{
	wcscpy_s(_formatName, formatname);
	return true;
}

/// <summary>
/// 指定したディレクトリの全ファイルをパッケージ化する関数
/// </summary>
/// <param name="dirName">ディレクトリ名</param>
/// <param name="outFileName">出力ファイル名</param>
/// <returns>処理が成功したかどうか</returns>
bool
Package::CreatePackage(const wchar_t* dirName, const wchar_t* outFileName)
{
	basic_string<wchar_t> dir = dirName;												//ディレクトリ名をコピー

	this->EnumFiles(dir.c_str(), _T("*.*"));											//ファイル列挙

	HANDLE hOutFile = CreateFile(outFileName, GENERIC_WRITE, 0, 0, CREATE_ALWAYS,		//ハンドル取得
		FILE_ATTRIBUTE_NORMAL, NULL);
	if (hOutFile == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	DWORD writeByte = 0;																//書き込みバイト数

	PackageHeader header = {};															//ヘッダーへの書き込み
	wcscpy_s(header.formatName, _formatName);											//フォーマット名
	header.fileNum = (UINT)_fileList.size();											//ファイル数
	WriteFile(hOutFile, &header, sizeof(header), &writeByte, NULL);						//ヘッダー書き込み

	list<basic_string<wchar_t>>::iterator it;											//ファイルデータ書き込み
	for (it = _fileList.begin(); it != _fileList.end(); it++)							//ディレクトリ内の全ファイルに対し処理を行う
	{
		MemoryMappedFile file;															//ファイルを初期化
		if (!file.Open(it->c_str(), GENERIC_READ))
		{
			CloseHandle(hOutFile);
			return false;
		}

		FileData data = {};																//ファイルデータを初期化
		wcscpy_s(data.fileName, it->c_str());
		data.size = file.GetSize();
		data.tail = '\0';

		WriteFile(hOutFile, data.fileName, sizeof(data.fileName), &writeByte, NULL);	//ファイル名書き込み
		WriteFile(hOutFile, &data.size, sizeof(data.size), &writeByte, NULL);			//ファイルサイズ書き込み
		WriteFile(hOutFile, file.GetPtr(), file.GetSize(), &writeByte, NULL);			//ファイルデータ書き込み
		WriteFile(hOutFile, &data.tail, sizeof(BYTE), &writeByte, NULL);				//データ末尾書き込み
	}

	CloseHandle(hOutFile);																//ファイルハンドルをクローズ

	return true;
}

/// <summary>
/// パッケージファイルを開く関数
/// </summary>
/// <param name="fileName">ファイル名</param>
/// <returns>処理が成功したかどうか</returns>
bool
Package::OpenPackage(const wchar_t* fileName)
{
	if(!_packageFile.Open(fileName))								//パッケージファイルを開く
	{
		_packageFile.Release();
		return false;
	}

	return this->LoadPackage((BYTE*)_packageFile.GetPtr());			//パッケージファイルのポインタを取得できたかどうか
}

/// <summary>
/// パッケージデータを読み取る関数
/// </summary>
/// <param name="ptr">パッケージデータの先頭ポインタ</param>
/// <returns>処理が成功したかどうか</returns>
bool
Package::LoadPackage(BYTE* ptr)
{
	BYTE* tempPtr = ptr;													//ヘッダーを読み込み、そのサイズ分ポインタをズラす
	PackageHeader header = {};
	memcpy(&header, tempPtr, sizeof(header));	
	tempPtr += sizeof(header);

	if (_tcscmp(header.formatName, _formatName) != 0)						//パッケージのフォーマット名のチェック
	{
		_packageFile.Release();
		return false;
	}

	for (UINT i = 0; i < header.fileNum; ++i)								//ヘッダーに登録されている全ファイルに対し実行
	{
		FileData data = {};
		memcpy(data.fileName, tempPtr, sizeof(data.fileName));				//ファイル名読み込み
		tempPtr += sizeof(data.fileName);
		memcpy(&data.size, tempPtr, sizeof(data.size));						//ファイルサイズ読み込み
		tempPtr += sizeof(data.size);
		data.ptr = tempPtr;													//ファイルの先頭ポインタ取得
		tempPtr += data.size;
		memcpy(&data.tail, tempPtr, sizeof(data.tail));						//データ末尾読み込み
		tempPtr += sizeof(data.tail);

		_fileDataMap.insert(												//データを挿入
			pair<basic_string<wchar_t>, FileData>(data.fileName, data)
		);
	}

	return true;
}

/// <summary>
/// 指定したファイルデータを取得する関数
/// </summary>
/// <param name="fileName">指定するファイル名</param>
/// <param name="pOutData">ファイルデータ</param>
/// <returns>処理が成功したかどうか</returns>
bool
Package::GetFileData(const wchar_t* fileName, FileData* pOutData)
{
	if (_fileDataMap.find(fileName) != _fileDataMap.end())				//ファイル名を元に検索し、対応するデータをコピーする
	{
		wcscpy_s(pOutData->fileName, _fileDataMap[fileName].fileName);
		pOutData->ptr = _fileDataMap[fileName].ptr;
		pOutData->size = _fileDataMap[fileName].size;
		pOutData->tail = _fileDataMap[fileName].tail;

		return true;
	}

	return false;
}

/// <summary>
/// パッケージに格納されているファイル数を返す関数
/// </summary>
/// <returns></returns>
int
Package::GetFileNum()
{
	return (int)_fileDataMap.size();
}

/// <summary>
/// .pakファイルを基にディレクトリにデータファイルを作成する関数
/// </summary>
/// <param name="fileName">.pakファイル名</param>
/// <returns>処理が成功したかどうか</returns>
bool
Package::CreateDirectoryAndFile(const basic_string<wchar_t> fileName)
{
	OpenPackage(fileName.c_str());

	basic_string<wchar_t> dirName;										//.pakファイルの拡張子を削除し、ディレクトリパスにする
	dirName.append(fileName,0,fileName.size() - 4);

	map<basic_string<wchar_t>, FileData>::iterator it;
	for (it = _fileDataMap.begin(); it != _fileDataMap.end(); it++)		//.pak内に格納されていたデータを全てファイルに書き出す
	{
		FileData fd;
		GetFileData(it->first.c_str(), &fd);
		void* data = fd.ptr;
		size_t size = fd.size;

		ofstream ofs(it->first.c_str(),
			std::ios::out | std::ios::binary);
		if (!ofs)
		{
			assert(0);
		}
		ofs.write(static_cast<const char*>(data), size);
		ofs.close();
	}

	return true;
}

/// <summary>
/// ディレクトリ内のファイルをすべて削除する関数
/// </summary>
/// <param name="dirName">ディレクトリパス</param>
/// <returns>処理が成功したかどうか</returns>
bool
Package::RemoveFileInDiretory(const basic_string<wchar_t> dirName)
{
	basic_string<wchar_t> currentDir = dirName;
	basic_string<wchar_t> wildCard = _T("*.*");

	currentDir += _T("/");
	currentDir += wildCard;
	WIN32_FIND_DATA fd = {};
	HANDLE hFind = FindFirstFile(currentDir.c_str(), &fd);										//ディレクトリが存在するかチェック
	if (hFind == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	do {																						//ディレクトリが持つ全データに対し実行

		if (_tcscmp(fd.cFileName, _T(".")) != 0 && _tcscmp(fd.cFileName, _T("..")) != 0)
		{
			if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)									//ディレクトリだった場合
			{
				basic_string<wchar_t> childDir = dirName;										//子ディレクトリに対し再帰呼び出し
				childDir += _T("/");
				childDir += fd.cFileName;
				this->RemoveFileInDiretory(childDir);
			}
			else																				//ファイルだった場合
			{
				basic_string<wchar_t> fileName = dirName + _T("/") + fd.cFileName;				//ファイル名を取得し、削除
				filesystem::remove(fileName.c_str());
			}
		}

	} while (FindNextFile(hFind, &fd) == TRUE);

	return true;
}
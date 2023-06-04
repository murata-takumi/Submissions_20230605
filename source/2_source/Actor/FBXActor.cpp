#include "Functions.h"
#include "Actor/FBXActor.h"
#include "Wrapper/Dx12Wrapper.h"

const char* CHARA_REF = "Character1_Reference|";	//アニメーション名に付与する文字列リテラル

/// <summary>
/// コンストラクタ
/// </summary>
/// <param name="dx12">Dx12Wrapperインスタンス</param>
/// <param name="filePath">モデルのパス</param>
FBXActor::FBXActor(Dx12Wrapper& dx12, const wchar_t* filePath)
	:_dx12(dx12),_isLoop(false)
{
	InitModel(filePath);					//モデルの初期化

	InitAnimation();						//アニメーションの初期化

	CreateVertexBufferView();				//頂点バッファー・ビュー作成

	CreateIndexBufferView();				//インデックスバッファー・ビュー作成

	CreateTransformView();					//座標変換用バッファー・ビュー作成

	CreateShaderResourceView();				//シェーダーリソース・ビュー作成
}

/// <summary>
/// デストラクタ
/// ポインタを開放する
/// </summary>
FBXActor::~FBXActor()
{
	_scene = nullptr;
}

/// <summary>
/// アニメーションを開始する関数
/// </summary>
void
FBXActor::StartAnimation()
{
	QueryPerformanceCounter(&_startTime);				//アニメーション開始時の時間を取得
}

/// <summary>
/// モデル関連の初期化を行う関数
/// </summary>
/// <param name="filePath">モデルのパス</param>
void
FBXActor::InitModel(const wchar_t* filePath)
{
	ImportSettings settings =							//モデル読み込み用設定
	{
		_meshes,										//メッシュ情報
		_boneMapping,									//ボーン名とインデックスの連想配列
		_boneInfo,										//行列などのボーン情報のベクトル
		false,											//U座標を反転させるか
		true,											//V座標を反転させるか
	};

	_scene = _loader.LoadScene(filePath);				//シーンを読み込む

	_loader.LoadModel(_scene, filePath, settings);		//シーンを元にモデルを読み込む

	_boneMat.resize(_boneInfo.size());					//ボーン行列の個数を決める
}

/// <summary>
/// アニメーション関連の初期化を行う関数
/// </summary>
void
FBXActor::InitAnimation()
{
	for (int i = 0; i < _scene->mNumAnimations; i++)				//モデルが持つ全アニメーションに対し実行
	{
		_anims[_scene->mAnimations[i]->mName.C_Str()]				//連想配列にアニメーション名とアニメーションデータを格納
			= _scene->mAnimations[i];

		string name = _scene->mAnimations[i]->mName.C_Str();		//アニメーション名を取得

		name = name.erase(0, 21);									//余分な文字列を削除

		_animStr.push_back(name);									//ベクトルに追加
	}

	_currentAnimStr = _animStr[0];									//最初に実行するアニメーションを設定

	ReadNodeHeirarchy(0, _scene->mRootNode, XMMatrixIdentity());	//ボーン行列を初期化
}

/// <summary>
/// ボーン変換行列を書き込む関数
/// </summary>
/// <param name="timeInSeconds">現在の経過時間</param>
void
FBXActor::BoneTransform(float timeInSeconds)
{
	float ticksPerSecond =																//1秒当たりの更新回数を取得
		static_cast<float>(_anims[CHARA_REF + _currentAnimStr]->mTicksPerSecond != 0 
			? _anims[CHARA_REF + _currentAnimStr]->mTicksPerSecond : 30.0f);

	float timeInTicks = timeInSeconds * ticksPerSecond;									//現時点での更新回数を取得
	float duration = _anims[CHARA_REF + _currentAnimStr]->mDuration;					//アニメーション総時間

	auto secondFrame =																	//2つめのフレームの時間
		_anims[CHARA_REF + _currentAnimStr]->mChannels[0]->mPositionKeys[1].mTime;

	float animationTime = _isLoop ? fmod(timeInTicks, duration) : timeInTicks;			//真理値の値に応じて時間の取り方を変える

	if (secondFrame < animationTime && animationTime <= duration)						//階層構造から変換行列を読み込む
	{
		ReadNodeHeirarchy(animationTime, _scene->mRootNode, XMMatrixIdentity());
	}

	for (UINT i = 0,boneLength = _boneMat.size(); i < boneLength; ++i)					//変換行列を更新する
	{
		_boneMat[i] = _boneInfo[i]._finalTrans;
	}
}

/// <summary>
/// 再帰的にノードの階層を読み込み、座標変換を行う行列を取得する関数
/// </summary>
/// <param name="animationTime">現在の経過時間</param>
/// <param name="pNode">親ノード</param>
/// <param name="parentTrans">親ノードで適用されている行列</param>
void
FBXActor::ReadNodeHeirarchy(float animationTime, const aiNode* pNode, const XMMATRIX& parentTrans)
{
	string nodeName(pNode->mName.data);													//ノード名を取得

	const aiAnimation* pAnimation = _anims[CHARA_REF + _currentAnimStr];				//アニメーションを取得

	aiMatrix4x4 aiTrans = pNode->mTransformation;										//ノードのaiMatrix4x4をXMMATRIXに変換
	XMFLOAT4X4 temp = XMFLOAT4X4
	(
		aiTrans.a1, aiTrans.b1, aiTrans.c1, aiTrans.d1,
		aiTrans.a2, aiTrans.b2, aiTrans.c2, aiTrans.d2,
		aiTrans.a3, aiTrans.b3, aiTrans.c3, aiTrans.d3,
		aiTrans.a4, aiTrans.b4, aiTrans.c4, aiTrans.d4
	);
	XMMATRIX nodeTrans = XMLoadFloat4x4(&temp);

	const aiNodeAnim* pNodeAnim = FindNodeAnim(pAnimation, nodeName);					//ノード名と同名のaiNodeAnimを取り出す

	if (pNodeAnim != nullptr)															//アニメーションがある場合各行列を取得する
	{
		XMMATRIX scalingM = CalcInterpolatedScaling(animationTime, pNodeAnim);			//スケーリングの行列を取得

		XMMATRIX rotationM = CalcInterpolatedRotation(animationTime, pNodeAnim);		//回転の行列を取得

		XMMATRIX translationM = CalcInterpolatedPosition(animationTime, pNodeAnim);		//平行移動の行列を取得

		nodeTrans = scalingM * rotationM * translationM;								//行列をまとめて乗算する
	}

	XMMATRIX globalTrans = nodeTrans * parentTrans;										//親の変換行列にノードから読み込んだ行列を適用

	if (_boneMapping.find(nodeName) != _boneMapping.end())
	{
		unsigned int boneIdx = _boneMapping[nodeName];									//ボーン名に対応するインデックスを取得

		_boneInfo[boneIdx]._finalTrans =												//最終的な変換行列を取得
			_boneInfo[boneIdx]._boneOffset * globalTrans;
	}

	auto numChildren = pNode->mNumChildren;												//子ノードの個数

	for (UINT i = 0; i < numChildren; i++)												//子ノードの方にも再帰処理を行う
	{
		ReadNodeHeirarchy(animationTime, pNode->mChildren[i], globalTrans);
	}
}

/// <summary>
/// メッシュ毎に頂点バッファー・ビューを作成する関数
/// </summary>
/// <returns>処理が成功したかどうか</returns>
HRESULT
FBXActor::CreateVertexBufferView()
{
	result = S_OK;																//返り値を初期化

	vertexBuffers.reserve(_meshes.size());										//頂点バッファー・ビューを用意する
	VBViews.reserve(_meshes.size());

	for (size_t i = 0; i < _meshes.size(); i++)									//全メッシュに対し処理を実行
	{
		ID3D12Resource* tmpVertexBuffer = nullptr;								//格納用バッファー
		D3D12_VERTEX_BUFFER_VIEW tmpVBView = {};								//格納用ビュー

		auto size = sizeof(FBXVertex) * _meshes[i].vertices.size();				//頂点全体のデータサイズ
		auto stride = sizeof(FBXVertex);										//頂点一個のデータサイズ

		auto heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);		//ヒーププロパティ
		auto desc = CD3DX12_RESOURCE_DESC::Buffer(size);						//リソース設定

		result = _dx12.Device()->CreateCommittedResource						//バッファー作成
		(
			&heapProp,
			D3D12_HEAP_FLAG_NONE,
			&desc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&tmpVertexBuffer)
		);
		if (FAILED(result))
		{
			assert(0);
			return result;
		}

		vector<FBXVertex> data = _meshes[i].vertices;							//頂点データ取得

	   	FBXVertex* mappedVertex = nullptr;										//頂点データをバッファーへ書き込む
		tmpVertexBuffer->Map(0, nullptr, (void**)&mappedVertex);
		copy(begin(data), end(data), mappedVertex);
		tmpVertexBuffer->Unmap(0, nullptr);

		tmpVBView.BufferLocation = tmpVertexBuffer->GetGPUVirtualAddress();		//ビューにバッファー情報を書き込む
		tmpVBView.SizeInBytes = tmpVertexBuffer->GetDesc().Width;
		tmpVBView.StrideInBytes = sizeof(FBXVertex);

		vertexBuffers.push_back(tmpVertexBuffer);								//頂点バッファー・ビューを配列に追加
		VBViews.push_back(tmpVBView);
	}

	return result;
}

/// <summary>
/// メッシュ毎にインデックスバッファー・ビューを作成する関数
/// </summary>
/// <returns></returns>
HRESULT
FBXActor::CreateIndexBufferView()
{
	result = S_OK;																	//返り値を初期化

	indexBuffers.reserve(_meshes.size());											//インデックスバッファー・ビューを用意する
	IBViews.reserve(_meshes.size());
	for (size_t i = 0; i < _meshes.size(); i++)										//全メッシュに対し処理を実行
	{
		ID3D12Resource* tmpIndexBuffer = nullptr;									//格納用バッファー
		D3D12_INDEX_BUFFER_VIEW tmpIBView = {};										//格納用ビュー

		auto size = sizeof(uint32_t) * _meshes[i].indices.size();					//インデックス全体のデータサイズ

		auto heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);			//ヒーププロパティ
		auto desc = CD3DX12_RESOURCE_DESC::Buffer(size);							//リソース設定

		result = _dx12.Device()->CreateCommittedResource							//バッファー作成
		(
			&heapProp,
			D3D12_HEAP_FLAG_NONE,
			&desc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&tmpIndexBuffer)
		);
		if (FAILED(result))
		{
			assert(0);
			return result;
		}

		vector<uint32_t> data = _meshes[i].indices;									//インデックスデータを取得

		uint32_t* mappedIndex = nullptr;											//インデックスデータをバッファーに書き込む
		tmpIndexBuffer->Map(0, nullptr, (void**)&mappedIndex);
		copy(begin(data), end(data), mappedIndex);
		tmpIndexBuffer->Unmap(0, nullptr);

		tmpIBView.BufferLocation = tmpIndexBuffer->GetGPUVirtualAddress();			//ビューにバッファー情報を書き込む
		tmpIBView.Format = DXGI_FORMAT_R32_UINT;
		tmpIBView.SizeInBytes = static_cast<UINT>(size);

		indexBuffers.push_back(tmpIndexBuffer);										//インデックスバッファー・ビューを配列に追加
		IBViews.push_back(tmpIBView);
	}

	return result;
}

/// <summary>
/// オブジェクトの座標変換に用いられるヒープ・ビューを作成する関数
/// </summary>
/// <returns>作成できたかどうか</returns>
HRESULT
FBXActor::CreateTransformView()
{
	result = S_OK;

	auto buffSize = sizeof(XMMATRIX) * (1 + _boneMat.size());						//ワールド行列用バッファーの作成
	buffSize = (buffSize + 0xff) & ~0xff;
	auto resDesc = CD3DX12_RESOURCE_DESC::Buffer(buffSize);
	result = _dx12.Device()->CreateCommittedResource
	(
		&_uploadHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(transformBuffer.ReleaseAndGetAddressOf())
	);
	if (FAILED(result)) {
		assert(0);
		return result;
	}

	result = transformBuffer->Map(0, nullptr, (void**)&_mappedMatrices);			//座標変換用行列の書き込み
	if (FAILED(result))
	{
		assert(0);
		return result;
	}
	_mappedMatrices[0] = XMMatrixIdentity();
	copy(_boneMat.begin(), _boneMat.end(), _mappedMatrices + 1);

																					//ディスクリプタヒープ設定用構造体の作成
	D3D12_DESCRIPTOR_HEAP_DESC transformDescHeapDesc = {};
	transformDescHeapDesc.NumDescriptors = 1;										//とりあえずワールドひとつ
	transformDescHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;		//シェーダーから見えるようにする
	transformDescHeapDesc.NodeMask = 0;												//ノードマスクは0で
	transformDescHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;			//デスクリプタヒープ種別

	result = _dx12.Device()->CreateDescriptorHeap(&transformDescHeapDesc,			//ヒープの作成
		IID_PPV_ARGS(transformHeap.ReleaseAndGetAddressOf()));
	if (FAILED(result)) {
		assert(0);
		return result;
	}

	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};									//ビュー設定用構造体の作成
	cbvDesc.BufferLocation = transformBuffer->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = transformBuffer->GetDesc().Width;

	_dx12.Device()->CreateConstantBufferView(&cbvDesc,								//ビューの作成
		transformHeap->GetCPUDescriptorHandleForHeapStart());

	return result;
}

/// <summary>
/// オブジェクトのテクスチャに用いられるヒープ・ビューを作成する関数
/// </summary>
/// <returns>作成できたかどうか</returns>
HRESULT
FBXActor::CreateShaderResourceView()
{
	result = S_OK;

	D3D12_DESCRIPTOR_HEAP_DESC texHeapDesc = {};												//ディスクリプタヒープの作成
	texHeapDesc.NodeMask = 1;
	texHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	texHeapDesc.NumDescriptors = _meshes.size();
	texHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	result = _dx12.Device()->CreateDescriptorHeap(&texHeapDesc,
		IID_PPV_ARGS(texHeap.ReleaseAndGetAddressOf()));

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};												//SRV用構造体の作成
	srvDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;

	auto CPUHeapHandle = texHeap->GetCPUDescriptorHandleForHeapStart();							//ヒープの先頭アドレス(CPU)
	auto GPUHeapHandle = texHeap->GetGPUDescriptorHandleForHeapStart();							//ヒープの先頭アドレス(GPU)	
	auto incrementSize =																		//先頭アドレスのずらす幅
		_dx12.Device()->GetDescriptorHandleIncrementSize(texHeapDesc.Type);

	TexMetadata meta = {};																		//テクスチャ読み込み用データ
	ScratchImage scratch = {};

	DXGI_FORMAT format;																			//テクスチャのフォーマット
	size_t width;																				//幅
	size_t height;																				//高さ
	UINT16 arraySize;																			//テクスチャサイズ
	UINT16 mipLevels;
	void* pixels;
	UINT rowPitch;
	UINT slicePitch;

	for (size_t i = 0; i < _meshes.size(); i++)													//各メッシュに対しテクスチャの読み込み処理
	{
		ID3D12Resource* tmpTexBuffer = nullptr;

		wstring path = _meshes[i].diffuseMap;													//テクスチャのパス

		if (wcscmp(path.c_str(), L"") == 0)														//白テクスチャ
		{
			vector<unsigned char> data(4 * 4 * 4);
			fill(data.begin(), data.end(), 0x00);

			format = DXGI_FORMAT_R8G8B8A8_UNORM;
			width = 4;
			height = 4;
			arraySize = 1;
			mipLevels = 1;

			pixels = data.data();
			rowPitch = 4 * 4;
			slicePitch = data.size();
		}
		else																					//通常テクスチャ
		{
			auto ext = FileExtension(path);														//拡張子を取得

			
			if (wcscmp(ext.c_str(), L"psd") == 0)												//拡張子が"psd"だった場合、"tga"に変換する
			{
				path = ReplaceExtension(path, "tga");
				ext = FileExtension(path);
			}

			result = _dx12._loadLambdaTable[ToString(ext)](path, &meta, scratch);				//拡張子に応じて読み込み関数を変える
			if (FAILED(result))
			{
				assert(0);
				return result;
			}

			auto img = scratch.GetImage(0, 0, 0);												//テクスチャデータの用意
			format = meta.format;
			width = meta.width;
			height = meta.height;
			arraySize = static_cast<UINT16>(meta.arraySize);
			mipLevels = static_cast<UINT16>(meta.mipLevels);
			pixels = img->pixels;
			rowPitch = static_cast<UINT>(img->rowPitch);
			slicePitch = static_cast<UINT>(img->slicePitch);
		}

		auto heapProp = 
			CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK, D3D12_MEMORY_POOL_L0);	//バッファー用ヒーププロパティ
		auto desc = CD3DX12_RESOURCE_DESC::Tex2D(
			format,
			width,
			height,
			arraySize,
			mipLevels);

		result = _dx12.Device()->CreateCommittedResource										//バッファーの作成
		(
			&heapProp,
			D3D12_HEAP_FLAG_NONE,
			&desc,
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
			nullptr,
			IID_PPV_ARGS(&tmpTexBuffer)
		);
		if (FAILED(result))
		{
			assert(0);
			return result;
		}

		result = tmpTexBuffer->WriteToSubresource(0,											//テクスチャの書き込みs
			nullptr,
			pixels,
			rowPitch,
			slicePitch
		);
		if (FAILED(result))
		{
			assert(0);
			return result;
		}

		srvDesc.Format = tmpTexBuffer->GetDesc().Format;										//フォーマットを合わせる
		_dx12.Device()->CreateShaderResourceView(tmpTexBuffer, &srvDesc, CPUHeapHandle);		//テクスチャバッファービューを作成

		GPUHandles.push_back(GPUHeapHandle);													//GPUのアドレスを追加

		CPUHeapHandle.ptr += incrementSize;														//CPUのアドレスをずらす
		GPUHeapHandle.ptr += incrementSize;														//GPUのアドレスをずらす
	}
}

/// <summary>
/// アニメーション配列の中から名前が一致したアニメーションを取り出す関数
/// </summary>
/// <param name="animation">アニメーション配列</param>
/// <param name="nodeName">アニメーション</param>
/// <returns>ノード名が一致したアニメーション</returns>
const aiNodeAnim*
FBXActor::FindNodeAnim(const aiAnimation* animation, const string nodeName)
{
	for (int i = 0; i < animation->mNumChannels; i++)			//aiAnimationが持つ全aiNodeAnimを対象にループ
	{
		const aiNodeAnim* nodeAnim = animation->mChannels[i];	//aiNodeAnimの名前を取得

		if (string(nodeAnim->mNodeName.data) == nodeName)		//名前が一致していたらそれを返す
		{
			return nodeAnim;
		}
	}

	return nullptr;												//なかったらnullptr
}

/// <summary>
/// スケーリングの補間を実施する関数
/// </summary>
/// <param name="animationTime">現在の経過時間</param>
/// <param name="nodeAnim">アニメーション</param>
/// <returns>スケーリング行列</returns>
XMMATRIX
FBXActor::CalcInterpolatedScaling(float animationTime, const aiNodeAnim* nodeAnim)
{
	aiVector3D tempVec;

	if (nodeAnim->mNumScalingKeys == 1)											//スケーリングで用いるキーが1つの場合
	{
		tempVec = nodeAnim->mScalingKeys[0].mValue;								//キーを取得
	}
	else																		//そうでない場合
	{
		int scalingIdx = FindScaling(animationTime, nodeAnim);					//スケーリングで使うキーのインデックスを取得
		int nextScalingIdx = scalingIdx + 1;									//次のキーのインデックスを取得
		assert(nextScalingIdx < nodeAnim->mNumScalingKeys);						//インデックスを制御点の数と比較し大きかったら強制終了

		float deltaTime = static_cast<float>(									//キー同士の経過時間の差分を取得
			nodeAnim->mScalingKeys[nextScalingIdx].mTime
			- nodeAnim->mScalingKeys[scalingIdx].mTime);

		float factor =															//現在の経過時間とキーの経過時間の差分を取得し差分で割る
			(animationTime
				- static_cast<float>(nodeAnim->mScalingKeys[scalingIdx].mTime)
			)
			/ deltaTime;
		assert(factor >= 0.0f && factor <= 1.0f);								//因数が0以上1以下であることを確認

		const aiVector3D& start = nodeAnim->mScalingKeys[scalingIdx].mValue;	//現フレームのaiVector3D
		const aiVector3D& end = nodeAnim->mScalingKeys[nextScalingIdx].mValue;	//次フレームのaiVector3D
		aiVector3D delta = end - start;											//二つのaiVector3Dの差分を取得

		tempVec = start + factor * delta;										//現フレームのベクトルに差分と因数の積を加算したものを返す
	}

	XMVECTOR scaling(XMVectorSet(tempVec.x, tempVec.y, tempVec.z, 0));			//aiVector3DからXMVectorへ変換

	 XMMATRIX ret = XMMatrixScalingFromVector(scaling);							//更にXMVectorからスケーリング用のXMMATRIXを取得

	return ret;
}

/// <summary>
/// 回転の補完を実行する関数
/// </summary>
/// <param name="animationTime">現在の経過時間</param>
/// <param name="nodeAnim">アニメーション</param>
/// <returns>回転行列</returns>
XMMATRIX
FBXActor::CalcInterpolatedRotation(float animationTime, const aiNodeAnim* nodeAnim)
{
	aiQuaternion tempQuat;

	if (nodeAnim->mNumRotationKeys == 1)														//キーが一つしかない場合そのまま返す
	{
		tempQuat = nodeAnim->mRotationKeys[0].mValue;
	}
	else
	{
		int rotationIdx = FindRotation(animationTime, nodeAnim);								//キーのインデックスを取得
		int nextRotationIdx = rotationIdx + 1;													//次のキーのインデックスを取得
		assert(nextRotationIdx < nodeAnim->mNumRotationKeys);

		float deltaTime = static_cast<float>													//2つのキーの間の時間を取得
			(
				nodeAnim->mRotationKeys[nextRotationIdx].mTime -
				nodeAnim->mRotationKeys[rotationIdx].mTime
				);
		float factor =																			//現在時間とキーの時間の差分を取得し差分で割る
			(animationTime - static_cast<float>(nodeAnim->mRotationKeys[rotationIdx].mTime))
			/ deltaTime;
		assert(factor >= 0.0f && factor <= 1.0f);

		const aiQuaternion& start = nodeAnim->mRotationKeys[rotationIdx].mValue;				//回転の初期クォータニオン
		const aiQuaternion& end = nodeAnim->mRotationKeys[nextRotationIdx].mValue;				//回転の最終クォータニオン

		aiQuaternion::Interpolate(tempQuat, start, end, factor);								//補間して正規化
		tempQuat = tempQuat.Normalize();
	}

	aiMatrix3x3 rotMat = tempQuat.GetMatrix();													//クォータニオンからaiMatrix3x3を取得
	auto rotTemp = XMFLOAT3X3
	(
		rotMat.a1, rotMat.b1, rotMat.c1,
		rotMat.a2, rotMat.b2, rotMat.c2,
		rotMat.a3, rotMat.b3, rotMat.c3
	);
	XMMATRIX ret = XMLoadFloat3x3(&rotTemp);													//aiMatrix3x3からXMMATRIXに変換

	return ret;
}

/// <summary>
/// 平行移動の補間を実行する関数
/// </summary>
/// <param name="animationTime">現在の経過時間</param>
/// <param name="nodeAnim">アニメーション</param>
/// <returns>平行移動行列</returns>
XMMATRIX
FBXActor::CalcInterpolatedPosition(float animationTime, const aiNodeAnim* nodeAnim)
{
	aiVector3D tempPos;

	if (nodeAnim->mNumPositionKeys == 1)											//キーが一つだけだったらそのまま返す
	{
		tempPos = nodeAnim->mPositionKeys[0].mValue;
	}
	else
	{
		int positionIdx = FindPosition(animationTime, nodeAnim);					//キーのインデックスを取得
		int nextPositionIdx = positionIdx + 1;										//次のキーのインデックスを取得
		assert(nextPositionIdx < nodeAnim->mNumPositionKeys);

		float deltaTime =															//二つのキーの差分を取得
			static_cast<float>(
				nodeAnim->mPositionKeys[nextPositionIdx].mTime - 
				nodeAnim->mPositionKeys[positionIdx].mTime
			);
		float factor =																//現在時間とキーの時間の差分を取得し、差分で割る
			(
				animationTime - 
				static_cast<float>(nodeAnim->mPositionKeys[positionIdx].mTime)
			)
			/ deltaTime;
		assert(factor >= 0.0f && factor <= 1.0f);

		const aiVector3D& start = nodeAnim->mPositionKeys[positionIdx].mValue;		//キー
		const aiVector3D& end = nodeAnim->mPositionKeys[nextPositionIdx].mValue;	//次のキー
		aiVector3D delta = end - start;
		tempPos = start + factor * delta;											//二つのaiVector3Dの補間
	}
	XMVECTOR translation(XMVectorSet(tempPos.x, tempPos.y, tempPos.z, 0));			//aiVector3DからXMVectorに変換

	XMMATRIX ret = XMMatrixTranslationFromVector(translation);						//XMVectorから平行移動用のXMMATRIXに変換

	return ret;
}

/// <summary>
/// スケーリングアニメーションのキーのインデックスを取得する関数
/// </summary>
/// <param name="animationTime">現在の経過時間</param>
/// <param name="nodeAnim">アニメーションデータ</param>
/// <returns>インデックス</returns>
int
FBXActor::FindScaling(float animationTime, const aiNodeAnim* nodeAnim)
{
	auto numSclKeys = nodeAnim->mNumScalingKeys;

	assert(numSclKeys > 0);

	for (int i = 0; i < numSclKeys - 1; ++i)
	{
		if (animationTime < static_cast<float>(nodeAnim->mScalingKeys[i + 1].mTime))
		{
			return i;
		}
	}

	assert(0);
	return -1;
}

/// <summary>
/// 回転に関するアニメーションデータのインデックスを取得する関数
/// </summary>
/// <param name="animationTime"></param>
/// <param name="nodeAnim">アニメーションデータ</param>
/// <returns>インデックス</returns>
int 
FBXActor::FindRotation(float animationTime, const aiNodeAnim* nodeAnim)
{
	auto numRotKeys = nodeAnim->mNumRotationKeys;

	assert(numRotKeys > 0);

	for (int i = 0; i < numRotKeys - 1; ++i)
	{
		if (animationTime < static_cast<float>(nodeAnim->mRotationKeys[i + 1].mTime))
		{
			return i;
		}
	}

	assert(0);
	return -1;
}

/// <summary>
/// 平行移動に関するアニメーションデータのインデックスを取得する関数
/// </summary>
/// <param name="animationTime"></param>
/// <param name="nodeAnim">アニメーションデータ</param>
/// <returns>インデックス</returns>
int
FBXActor::FindPosition(float animationTime, const aiNodeAnim* nodeAnim)
{
	auto numPosKeys = nodeAnim->mNumPositionKeys;

	assert(numPosKeys > 0);

	for (int i = 0; i < numPosKeys - 1; ++i)
	{
		if (animationTime < static_cast<float>(nodeAnim->mPositionKeys[i + 1].mTime))
		{
			return i;
		}
	}

	assert(0);
	return -1;
}

/// <summary>
/// アクターが持っているアニメーションの名前のベクトルを返す関数
/// </summary>
/// <returns>アニメーション名のベクトル</returns>
vector<string>
FBXActor::GetAnimstr()
{
	return _animStr;
}

/// <summary>
/// 現在実行しているアニメーション名を返す関数
/// </summary>
/// <returns>アニメーション名</returns>
string
FBXActor::GetCurentAnimStr()
{
	return _currentAnimStr;
}

/// <summary>
/// 実行するアニメーションを指定する関数
/// </summary>
/// <param name="animStr"></param>
void
FBXActor::SetAnimStr(string animStr)
{
	_currentAnimStr = animStr;

	StartAnimation();
}

/// <summary>
/// アニメーションのループを決定する関数
/// </summary>
/// <param name="val">ループするかどうかを決める真理値</param>
void
FBXActor::SetLoop(bool val)
{
	_isLoop = val;

	StartAnimation();
}

/// <summary>
/// 毎フレームの描画処理を実行する関数
/// </summary>
void
FBXActor::Draw()
{
	//座標変換用ディスクリプタヒープをセット
	ID3D12DescriptorHeap* transformHeaps[] = { transformHeap.Get() };
	_dx12.CommandList()->SetDescriptorHeaps(1, transformHeaps);

	//ルートパラメータとディスクリプタヒープのハンドルを関連付け
	_dx12.CommandList()->SetGraphicsRootDescriptorTable(1, transformHeap->GetGPUDescriptorHandleForHeapStart());

	//モデルを構成するメッシュ毎に以下の処理を行う
	for (size_t i = 0; i < _meshes.size(); i++)
	{
		//頂点・インデックスバッファービューのセット
		_dx12.CommandList()->IASetVertexBuffers(0, 1, &VBViews[i]);
		_dx12.CommandList()->IASetIndexBuffer(&IBViews[i]);

		//テクスチャバッファービューのセット
		ID3D12DescriptorHeap* SetTexHeap[] = { texHeap.Get() };
		_dx12.CommandList()->SetDescriptorHeaps(1, SetTexHeap);
		_dx12.CommandList()->SetGraphicsRootDescriptorTable(2, GPUHandles[i]);

		//メッシュの描画
		_dx12.CommandList()->DrawIndexedInstanced(_meshes[i].indices.size(), 1, 0, 0, 0);
	}
}

/// <summary>
/// 毎フレームの座標変換処理を実行する関数
/// </summary>
void
FBXActor::Update()
{
	QueryPerformanceCounter(&_currentTime);										//現在時間を取得

	auto time = GetLIntDiff(_currentTime, _startTime);							//現在時間をfloatに変換
	BoneTransform(time);														//現在時間を渡し、行列を取得
	copy(_boneMat.begin(), _boneMat.end(), _mappedMatrices + 1);				//行列をシェーダーに渡し、アニメーションを実行
}
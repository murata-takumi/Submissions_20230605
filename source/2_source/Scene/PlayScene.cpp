#include "Actor/AssimpLoader.h"
#include "Actor/FBXActor.h"
#include "Manager/ImGuiManager.h"
#include "Scene/PlayScene.h"

const wchar_t* UNITY_PATH = L"Asset/model/Unitychan.fbx";		//モデルデータが格納されているファイルパス

/// <summary>
/// コンストラクタ
/// </summary>
/// <param name="dx12">Dx12Wrapperインスタンス</param>
/// <param name="input">InputManagerインスタンス</param>
/// <param name="sound">SoundManagerインスタンス</param>
PlayScene::PlayScene(
	Dx12Wrapper& dx12,
	InputManager& input,
	SoundManager& sound
)
	:SceneBase
(
	dx12,
	input,
	sound
)
{

}

/// <summary>
/// デストラクタ
/// </summary>
PlayScene::~PlayScene()
{
	
}

/// <summary>
/// 更新処理
/// </summary>
void
PlayScene::Update()
{
	InputUpdate();																//入力を更新

	if (_isPlaying)																//操作可能な場合入力を反映する
	{
		//WASDキーの入力に応じて視点を移動させる
		//注視点を中心に視点を回転
		if (_input.KeyState().A)
		{
			_dx12.RotateSphericalCoordinates(Degree::Azimth, 90);
		}
		if (_input.KeyState().D)
		{
			_dx12.RotateSphericalCoordinates(Degree::Azimth, -90);
		}
		if (_input.KeyState().W)
		{
			_dx12.RotateSphericalCoordinates(Degree::Elevation, 45);
		}
		if (_input.KeyState().S)
		{
			_dx12.RotateSphericalCoordinates(Degree::Elevation, -45);
		}

		//Q･Eキーを押した時は拡大・縮小
		//左シフトキーを押している場合はカメラを上下に平行移動させる
		if (_input.KeyState().Q)
		{
			if (_input.KeyState().LeftShift)
			{
				_dx12.TranslateSphericalCoordinates(Direction::Vertical, -90);
			}
			else
			{
				_dx12.ScalingSphericalCoordinates(-90);
			}
		}
		if (_input.KeyState().E)
		{
			if (_input.KeyState().LeftShift)
			{
				_dx12.TranslateSphericalCoordinates(Direction::Vertical, 90);
			}
			else
			{
				_dx12.ScalingSphericalCoordinates(90);
			}
		}
	}

	DrawUpdate();																//描画処理
}

/// <summary>
/// シーン開始時に実行する関数
/// </summary>
void
PlayScene::SceneStart()
{
	_dx12.ResetSphericalCoordinates();									//カメラ位置をリセット

	_direction = XMLoadFloat3(new XMFLOAT3(0.0f, 0.0f, 0.0f));			//移動ベクトルを初期化

	//モデル読み込み処理
	auto f = [&]()
	{
		_isLoading = true;												//ロード中だと示す

		vector<thread> ths;												//並列処理したいスレッドのベクトル

		ths.emplace_back												//スレッドの追加
		(	
			[&](){
				_actor.reset(new FBXActor(_dx12, UNITY_PATH));			//Unitychanインスタンスの追加
			}
		);

		for (auto& th : ths)											//スレッドの実行完了待ち
		{
			th.join();
		}

		_dx12.ImGui()->SetActor(_actor);								//アクターをImGuiManager側に渡す
		_dx12.ImGui()->SetCurrentAnimStr(_actor->GetCurentAnimStr());	//アクターが実行するアニメーション名をImGuiManager側に渡す

		_isLoading = false;												//ロード完了

		SceneBase::SceneStart();										//フェード明け
	};
	ParallelProcess(f);													//モデル読み込み処理を通常の描画処理と並行して行う
}

/// <summary>
/// シーン遷移時に実行する関数
/// </summary>
void
PlayScene::SceneEnd()
{
	_actor = nullptr;					//アクターを開放

	_dx12.ImGui()->ResetAnimStr();		//ImGuiで表示されているアニメーション名を初期化
}

/// <summary>
/// UIを描画する関数
/// </summary>
void
PlayScene::BackGroundDraw()
{
	SceneBase::BackGroundDraw();		//背景を描画
}

/// <summary>
/// モデル描画を行う関数
/// </summary>
void
PlayScene::ModelDraw()
{
	if (!_isLoading)			//プレイヤーが読み込み完了している場合描画処理
	{
		if (_actor != nullptr)
		{
			_actor->Draw();		//描画処理
			_actor->Update();	//更新処理
		}
	}
}

/// <summary>
/// エフェクト上の描画を行う関数
/// </summary>
void
PlayScene::EffectAndUIDraw()
{
	SceneBase::EffectAndUIDraw();		//マウスカーソルを描画

	if (_isLoading)						//ロード中の場合
	{
		_dx12.Sprite()->LoadingDraw();	//ロード画面を描画
	}
	else if(_isPlaying)					//操作可能な場合
	{
		_dx12.ImGui()->SetFPS(_fps);	//SceneからImGuiにデータを渡す
		_dx12.ImGui()->ImGuiDraw();		//ImGuiのウィンドウを描画
	}
}
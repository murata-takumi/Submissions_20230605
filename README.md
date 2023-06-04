# Submissions_20230605

# 作品の概要
  DirectX12を用いて開発した、FBXモデルの表示やアニメーションの実行等を処理するプログラム作品です。  
  ### DirectX12を使用した理由  
     以前お話を伺った本職のプログラマーから、「ゲームエンジンに過度に依存すると、不具合の修正方法が分からなくなる」  
     というアドバイスを受け、エンジンではなくグラフィックスAPIを使用して開発しようと考えた為です。  
     それを踏まえ、現在も積極的に更新されている点や、グラフィックスAPIの中では比較的簡単であるという点を考慮し、  
     DirectX12を選択しました。  

# 開発期間
  約半年  

# 開発環境
  Windows10,Visual Studio 2022  

# 画面説明
  1.	タイトル画面  
  ![TitleScene](https://github.com/murata-takumi/Submissions_20230603/blob/main/img/TitleScene.png)  
  - 「Start」ボタン  
    ゲーム画面に遷移  
  
  - 「End」ボタン  
    ゲーム終了

  2. ゲーム画面 
  ![PlayScene](https://github.com/murata-takumi/Submissions_20230603/blob/main/img/PlayScene.png)  
  - 左上ウィンドウ  
    FPSを更新・表示  
  - 各アニメボタン（右）  
    対応するアニメーションを実行  
  
  - Loopチェックボックス  
    アニメーションをループさせるかどうか
  
  - WASDキー  
    FBXモデルを中心にカメラ回転  
  
  - Qキー  
    モデルからカメラを遠ざける  
	 （左シフトキー押下時）カメラを下に平行移動  
 
  - Eキー  
    モデルにカメラを近付ける  
		（左シフトキー押下時）カメラを上に平行移動  
  
  - 「CameraReset」ボタン  
    カメラをリセット  
  
  - 「Exit」ボタン  
    タイトル画面に遷移  

# アピールポイント
  1.	約60FPSで動くアニメーション
    ![Animation](https://github.com/murata-takumi/Submissions_20230603/blob/main/img/Animation.gif)   
    私は3Dアクションゲームが好きで、アニメーションを滑らかに動かすことが重要だと考えています。そのため、座標変換でのforループの継続条件で使用する変数をループ開始前に定義したり、前置インクリメントを用いたりすることで軽量化し、約60FPSでのアニメーションを実現しました。
  
  2.	マルチパスレンダリングによるフェードイン・フェードアウトの実装  
    ![Fadein・Fadeout](https://github.com/murata-takumi/Submissions_20230603/blob/main/img/Fadein・Fadeout.gif)   
    マルチパスレンダリングによって、モデルやImGui等のレンダリング結果をテクスチャとしてペラポリゴンに貼り付け、そのRGB値を0から1に調整することで画面遷移時のフェードイン・フェードアウトを実現しています。

# 使用ライブラリ・用途一覧
各ライブラリのライセンスを以下の『ライセンス表記』に記載しております。
- Assimp  
  FBXモデルの表示に使用しました。  

- DirectXTex_master  
  テクスチャの表示に使用しました。  

- DirectXTK12_master  
  音声、テキスト、画像の表示に使用しました。  

- ImGui  
  アニメーション実行、FPS表示に使用しました。  

# 使用させて頂いた素材の配布元・用途一覧
-	モリサワのBIZ UD明朝　様 (https://github.com/googlefonts/morisawa-biz-ud-mincho)  
  フォントをImGuiにおける文字フォントとして使用しました。  
  OFLに基づいたライセンスを以下の『ライセンス表記』に記載しております。  
  
- 無料のAi・PNG白黒シルエットイラスト　様 (https://www.silhouette-illust.com/)  
  配布されていた素材をロード中アイコンおよびマウスカーソルとして使用しました。  
  
- かわいいフリー素材集 いらすとや　様 (https://www.irasutoya.com/)  
  サイト内で配布されていた素材を背景として使用しました。  
  
- ニコニ・コモンズ　様 (https://commons.nicovideo.jp/material/nc207993)  
  URL先の素材を加工し、タイトル画面のボタンとして使用しました。  
  
- 効果音ラボ　様 (https://soundeffect-lab.info/)  
  サイト内で配布されていた素材をボタン押下時の効果音として使用しました。  
  
- ユニティちゃん　様 (https://unity-chan.com/)  
  ゲーム画面で表示するためのFBXモデルとして使用しました。  
  モデルを用いた創作物の頒布時に必要なライセンス表記は以下の『ライセンス表記』に記載しております。  

# ライセンス表記
- Copyright (c) 2006-2016, assimp team All rights reserved.
- Copyright (c) 2011-2020 Microsoft Corp
- Copyright (c) 2014-2023 Omar Cornut 
- © Unity Technologies Japan/UCL
- Copyright 2022 The BIZ UDGothic Project Authors

# Submissions_20230605

# 作品の概要　*Overview of the Project*
  DirectX12を用いて開発した、FBXモデルの表示やアニメーションの実行等を処理するプログラム作品です。  
  
  This project handles the display and execution of animations for FBX models using DirectX 12.  
  ## DirectX12を使用した理由 *Reason for Using DirectX 12*  
     以前お話を伺った本職のプログラマーから、「ゲームエンジンに過度に依存すると、不具合の修正方法が分からなくなる」  
     というアドバイスを受け、エンジンではなくグラフィックスAPIを使用して開発しようと考えた為です。  
     それを踏まえ、現在も積極的に更新されている点や、グラフィックスAPIの中では比較的簡単であるという点を考慮し、  
     DirectX12を選択しました。  
     
     Based on advice from a professional programmer I spoke to before, who mentioned that relying too heavily on game engines would make it difficult to know how to fix issues, I decided to develop using graphics APIs instead of an engine. Considering that DirectX 12 is actively updated and relatively easy compared to other graphics APIs, I chose DirectX 12.  

# 開発期間 *Development Period*
  約半年  
  
  Approximately six months.  

# 開発環境 *Development Environment*
  Windows10,Visual Studio 2022  

# 画面説明 *Screen Descriptions*
  1.	タイトル画面 *Title Screen*
  ![TitleScene](https://github.com/murata-takumi/Submissions_20230605/blob/master/img/TitleScene.png)  
  - 「Start」ボタン *"Start" button*  
    ゲーム画面に遷移  
    
    Transition to the game screen.  
  
  - 「End」ボタン *"end" button*  
    ゲーム終了  
    
    Quit the game.  

  2. ゲーム画面 *Game Screen*
  ![PlayScene](https://github.com/murata-takumi/Submissions_20230605/blob/master/img/PlayScene.png)  
  - 左上ウィンドウ *Top-left window*    
    FPSを更新・表示  
    
    Updates and displays the FPS.  
  - 各アニメボタン（右） *Animation buttons (right)*  
    対応するアニメーションを実行  
    
    Executes the corresponding animation.  
  
  - Loopチェックボックス *Loop checkbox*  
    アニメーションをループさせるかどうか  
    
    Determines whether the animation should loop or not.  
  
  - WASDキー *WASD keys*  
    FBXモデルを中心にカメラ回転  
    
    Rotates the camera around the FBX model.  
  
  - Qキー *Q key*  
    モデルからカメラを遠ざける  
    
    Moves the camera away from the model.  
   （左シフトキー押下時）カメラを下に平行移動  
   
   (Hold the left Shift key at the same time to move the camera parallel downwards)  
 
  - Eキー *E key*  
    モデルにカメラを近付ける  
    
    Moves the camera closer to the model.  
   （左シフトキー押下時）カメラを上に平行移動  
   
   (Hold the left Shift key at the same time to move the camera parallel upwards)  
  
  - 「CameraReset」ボタン *"CameraReset" button*  
    カメラをリセット  
    
    Resets the camera.  
  
  - 「Exit」ボタン *"Exit" button*  
    タイトル画面に遷移  
    
    Returns to the title screen.  

# アピールポイント *Strengths*  
  1.	約60FPSで動くアニメーション *Smooth animations running at approximately 60 FPS*
    ![Animation](https://github.com/murata-takumi/Submissions_20230605/blob/master/img/Animation.gif)   
    私は3Dアクションゲームが好きで、アニメーションを滑らかに動かすことが重要だと考えています。そのため、座標変換でのforループの継続条件で使用する変数をループ開始前に定義したり、前置インクリメントを用いたりすることで軽量化し、約60FPSでのアニメーションを実現しました。  
    
    I love 3D action games and believe that smooth animation is crucial in 3D action games. To achieve this, I defined variables used in the continuation condition of the for loop in coordinate transformations before starting the loop and used prefix incrementation to optimize the code, resulting in animations running at approximately 60 FPS.  
  
  2.	マルチパスレンダリングによるフェードイン・フェードアウトの実装 *Implementation of Fade-in/Fade-out using Multipass-Rendering*  
    ![Fadein・Fadeout](https://github.com/murata-takumi/Submissions_20230605/blob/master/img/Fadein・Fadeout.gif)   
    マルチパスレンダリングによって、モデルやImGui等のレンダリング結果をテクスチャとしてペラポリゴンに貼り付け、そのRGB値を0から1に調整することで画面遷移時のフェードイン・フェードアウトを実現しています。  
    
    By using multipass rendering, I attached the rendering results of models and ImGui to polygon as textures and adjusted its RGB values from 0 to 1 to achieve fade-in and fade-out effects during screen transitions.  

# 使用ライブラリ・用途一覧 *List of Used Libraries and Purposes*  
各ライブラリのライセンスを以下の『ライセンス表記』に記載しております。  

The licenses for each library are listed in the 'License Notice' below.  
- Assimp  
  FBXモデルの表示に使用しました。  
  
  Used for displaying FBX models.  

- DirectXTex_master  
  テクスチャの表示に使用しました。  
  
  Used for displaying textures.  

- DirectXTK12_master  
  音声、テキスト、画像の表示に使用しました。  
  
  Used for displaying audio, text, and images.  

- ImGui  
  アニメーション実行、FPS表示に使用しました。  
  
  Used for executing animations and displaying FPS.  

# 使用させて頂いた素材の配布元・用途一覧 *List of Distributed Materials and Their Uses*  
-	モリサワのBIZ UD明朝　様 (https://github.com/googlefonts/morisawa-biz-ud-mincho)  
  フォントをImGuiにおける文字フォントとして使用しました。  
  OFLに基づいたライセンスを以下の『ライセンス表記』に記載しております。  
  
  Used the font as a character font in ImGui. The license based on OFL is mentioned in the 'License Notice' below.  
  
- 無料のAi・PNG白黒シルエットイラスト　様 (https://www.silhouette-illust.com/)  
  配布されていた素材をロード中アイコンおよびマウスカーソルとして使用しました。  
  
  Used the distributed materials as loading icons and mouse cursors.  
  
- かわいいフリー素材集 いらすとや　様 (https://www.irasutoya.com/)  
  サイト内で配布されていた素材を背景として使用しました。  
  
  Used the material distributed in the site as background.  
  
- ニコニ・コモンズ　様 (https://commons.nicovideo.jp/material/nc207993)  
  URL先の素材を加工し、タイトル画面のボタンとして使用しました。  
  
  Proccessed the material of URL and used as button of Title Screen.  
  
- 効果音ラボ　様 (https://soundeffect-lab.info/)  
  サイト内で配布されていた素材をボタン押下時の効果音として使用しました。  
  
  Used the material distributed in the site as sound effect pushing the buttons.  
  
- ユニティちゃん　様 (https://unity-chan.com/)  
  ゲーム画面で表示するためのFBXモデルとして使用しました。  
  モデルを用いた創作物の頒布時に必要なライセンス表記は以下の『ライセンス表記』に記載しております。  
  
  Used the model as a FBX model for displaying the Game Screen. The license needed for distributing project using the model is mentioned in the 'License Notice' below.  

# ライセンス表記 *Liscense Notice*  
- Copyright (c) 2006-2016, assimp team All rights reserved.
- Copyright (c) 2011-2020 Microsoft Corp
- Copyright (c) 2014-2023 Omar Cornut 
- © Unity Technologies Japan/UCL
- Copyright 2022 The BIZ UDGothic Project Authors

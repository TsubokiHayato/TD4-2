# TD4-2

共有エンジン **TuboEngine（EngineCore）** を使ったチーム制作用のゲームプロジェクトです。
このページは「最初に読む入口」です。シーンを作る・動かすのに必要な最低限をまとめています。

---

## 開発環境

| 項目 | 内容 |
| --- | --- |
| IDE | Visual Studio 2022（プラットフォームツールセット v145） |
| プラットフォーム | **x64** |
| 言語 | C++20 |
| Windows SDK | 10.0 |
| グラフィックス | DirectX 12 |

---

## セットアップ（最初の1回）

エンジンは git submodule（`project/EngineCore`）です。**サブモジュールごと取得**してください。

```bash
# 新規に clone する場合
git clone --recursive <このリポジトリのURL>

# すでに clone 済みでサブモジュールが空の場合
git submodule update --init --recursive
```

> `project/EngineCore` の中身が空のときは、ほぼ確実にサブモジュール未取得です。上のコマンドを実行してください。

---

## ビルド & 実行

1. `project/TD4-2.sln` を Visual Studio で開く
2. 構成を **`Debug` / `x64`** にする
3. `F5`（デバッグ実行）または `Ctrl+F5`（デバッグなし実行）

- **Debug** ビルドでは `USE_IMGUI` が有効になり、画面に ImGui の操作ウィンドウが出ます（シーン切替・各オブジェクトの調整など）。
- **Release** ビルドでは ImGui は無効です（本番用）。
- 実行時シェーダはビルド前イベントで EngineCore からコピーされます（`project/Resources/Shaders/` は生成物なので git 管理外）。

---

## 実行中の操作（Debug ビルド）

| キー / 操作 | 動作 |
| --- | --- |
| `F2` | デバッグカメラ ON/OFF（フリーカメラで自由に見回せる） |
| デバッグカメラ ON 中：右ドラッグ | 視点回転 |
| デバッグカメラ ON 中：中ドラッグ | 平行移動 |
| デバッグカメラ ON 中：ホイール | ズーム |
| ImGui の **Scene ウィンドウ** | ボタンで好きなシーンへ即切り替え |

SampleScene には `WASD / Q,E` でオブジェクトを動かすなどの操作デモも入っています。
**まず SampleScene を起動して、エンジンでできることを一通り触ってみてください**（`project/application/scene/Sample/SampleScene.cpp` がコメント付きの教材になっています）。

---

## プロジェクト構成

```
project/
├─ main.cpp                     … エントリーポイント（基本さわらない）
├─ application/
│  ├─ Order.cpp / .h            … 起動クラス（Framework 継承）。開始シーンの指定はここ
│  └─ scene/
│     ├─ GameScenes.h           … シーン番号 enum（シーンを増やすとき①ここに追加）
│     ├─ SceneRegistration.cpp  … シーンの登録（シーンを増やすとき②ここに追加）
│     ├─ HOW_TO_ADD_SCENE.md    … ★シーンの追加手順とテンプレ（コピペ用）
│     ├─ Sample/                … 教材シーン（エンジンの使い方デモ）
│     ├─ Title/ Stage/ Clear/ Over/
│     └─ User/                  … 各メンバーの個人開発用サンドボックス（User1〜4）
├─ Resources/                   … モデル / テクスチャ / 音声 など
└─ EngineCore/                  … 共有エンジン（git submodule・別リポジトリ）
```

---

## リソース（Resources）の置き場ルール

各アセットは `project/Resources/` 配下の種類別フォルダに置きます。**コードに渡すパスは「その種類フォルダからの相対」**です（`Resources/` までは API が自動で付けます）。

| 種類 | 置き場 | コードでの指定例 | 実ファイル |
| --- | --- | --- | --- |
| 3Dモデル | `Resources/Models/` | `obj->Initialize("axis/axis.obj")` | `Resources/Models/axis/axis.obj` |
| テクスチャ | `Resources/Textures/` | `sprite->Initialize("uvChecker.png")` | `Resources/Textures/uvChecker.png` |
| 音声(WAV) | `Resources/Audio/` | `audio->Initialize("test.wav")` | `Resources/Audio/test.wav` |
| パーティクル画像 | `Resources/Textures/` | `preset.texture = "particle.png"` | `Resources/Textures/particle.png` |

その他のフォルダ：`Font/`（フォント）, `Particles/`（パーティクル定義）, `Stage/`（ステージデータ）, `Text/`（テキスト）, `Shaders/`（**自動生成・git管理外**。エンジンから PreBuild でコピーされる）。

ルール:
- **モデルは1モデル＝1フォルダ**（`Models/player/player.obj` のように）。`.mtl` やテクスチャも同じフォルダに。
- パスの**大文字小文字・拡張子**はファイル名と完全一致させること（出ない原因の大半がここ）。
- 追加したアセットは **git に add** を忘れずに（メンバーの環境で「ファイルが無い」事故防止）。

---

## シーンを追加する

3手順だけです。詳しい手順とコピペ用テンプレは
**[application/scene/HOW_TO_ADD_SCENE.md](project/application/scene/HOW_TO_ADD_SCENE.md)** を見てください。

ざっくり:
1. `GameScenes.h` の enum に番号を1つ足す
2. シーンクラス（`.h` / `.cpp`）を作る（テンプレをコピー）
3. `SceneRegistration.cpp` に1行登録する

---

## シーン遷移のしかた

遷移APIは **1つだけ** です。シーンの `Update()` などから呼びます。

```cpp
#include "SceneManager.h"

// 別シーンへ移る
SceneManager::GetInstance()->ChangeScene(STAGE);

// 同じ番号を渡すと、そのシーンをリロード（Finalize → Initialize）
SceneManager::GetInstance()->ChangeScene(GetSceneNoOfThis);
```

- 切り替えは **次のフレームの Update で** 行われます（描画の途中で差し替わらないので安全）。
- 開始シーンは `Order::Initialize()` の `SceneManager::GetInstance()->Initialize(SAMPLE);` で指定します。

> 旧バージョンにあった「各シーンの Initialize 先頭で `SetSceneNo(自分)` を書くおまじない」は**不要になりました**。`ChangeScene` だけ使ってください。

---

## よくあるハマりどころ

- **画面が真っ暗 / 何も映らない** … カメラを用意して `GetMainCamera()` で返しているか、`Object3d::SetCamera()` を呼んだか確認。
- **モデル / テクスチャが出ない** … パスは `Resources/Models/`・`Resources/Textures/` からの相対。ファイル名のタイプミスに注意。
- **新しく作った `.cpp` がビルドされない** … ファイルを「フォルダに置いただけ」では不足。VS のソリューションエクスプローラーから **追加（プロジェクトに登録）** すること（[HOW_TO_ADD_SCENE.md](project/application/scene/HOW_TO_ADD_SCENE.md) 参照）。
- **`EngineCore` が空** … サブモジュール未取得。`git submodule update --init --recursive`。

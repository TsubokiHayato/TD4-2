# シーンの追加方法（コピペ用テンプレ付き）

新しいシーンを作るときの手順です。ここでは例として `MyScene`（番号 `MYSCENE`）を追加します。
**自分の名前に置き換えて**進めてください。

---

## チェックリスト（この3ステップだけ）

- [ ] **①** `GameScenes.h` の enum に `MYSCENE` を足す
- [ ] **②** `MyScene.h` / `MyScene.cpp` を作る（下のテンプレをコピー）
- [ ] **③** `SceneRegistration.cpp` に include と登録を1行ずつ足す
- [ ] （④ VS のソリューションエクスプローラーで `.h/.cpp` を **プロジェクトに追加**）

> ④ が抜けると「フォルダにはあるのにビルドされない / リンクエラー」になります。
> VS で `scene` フォルダを右クリック →「追加」→「既存の項目」で新しい2ファイルを選ぶのが確実です。

---

## ① `GameScenes.h` に番号を足す

```cpp
// enum の末尾に自分の番号を追加するだけ（数値は自動で割り振られる）
enum SCENE { SAMPLE, TITLE, STAGE, CLEAR, OVER, USER1, USER2, USER3, USER4, MYSCENE };
//                                                                          ^^^^^^^ 追加
```

---

## ② シーンクラスを作る（テンプレ）

`scene/My/` などにフォルダを作り、以下の2ファイルを置きます。
`MyScene` の部分を自分のクラス名に置換してください。

### `MyScene.h`

```cpp
#pragma once
#include "IScene.h"
#include "Camera.h"
#include <memory>

// ○○用シーン。ここに自分の処理を足していく（カメラだけ持った最小の雛形）。
class MyScene : public IScene {
public:
	void Initialize() override;
	void Update() override;
	void Finalize() override;
	void Object3DDraw() override;
	void SpriteDraw() override;
	void ImGuiDraw() override;
	void ParticleDraw() override;

	// このシーンの主カメラをエンジンへ渡す（ライン描画などが参照する）
	TuboEngine::Camera* GetMainCamera() const override { return camera_.get(); }

private:
	std::unique_ptr<TuboEngine::Camera> camera_;
};
```

### `MyScene.cpp`

```cpp
#include "MyScene.h"
#include "GameScenes.h"
#include "SceneManager.h" // シーン遷移を使うとき用

void MyScene::Initialize() {
	// カメラ（3D描画には最低1台必要。scale 既定は {1,1,1} なので設定しなくても映る）
	camera_ = std::make_unique<TuboEngine::Camera>();
	camera_->SetTranslate({ 0.0f, 0.0f, -15.0f });
	camera_->setRotation({ 0.0f, 0.0f, 0.0f });
	camera_->Update();

	// TODO: モデル / スプライト / 音声などの生成・読み込みをここに書く
}

void MyScene::Update() {
	camera_->Update();
	// TODO: ここに更新処理（入力・ゲームロジック）を書く
	// 別シーンへ遷移する例:  SceneManager::GetInstance()->ChangeScene(STAGE);   // 次フレームで切り替わる
}

void MyScene::Finalize() {}

void MyScene::Object3DDraw() {}   // TODO: 3Dオブジェクト描画
void MyScene::SpriteDraw()  {}    // TODO: 2Dスプライト描画
void MyScene::ImGuiDraw()   {}    // TODO: ImGui描画（Debugビルドのみ呼ばれる）
void MyScene::ParticleDraw(){}    // TODO: パーティクル描画
```

> 各 `Draw()` が用途別に分かれているのは、エンジンが
> **3D → 2D(Sprite) → Line → Particle** の順に正しいパイプライン状態を作ってから呼ぶためです。
> 「何をどのフェーズで描くか」を意識してください。実例は `Sample/SampleScene.cpp`。

---

## ③ `SceneRegistration.cpp` に登録する

include を1行と、`RegisterGameScenes()` の中に登録を1行足します。

```cpp
// 上の include 群に追加
#include "My/MyScene.h"

void RegisterGameScenes() {
	SceneManager* sm = SceneManager::GetInstance();
	// ...既存の登録...
	sm->RegisterScene(MYSCENE, [] { return std::make_unique<MyScene>(); }, "MyScene");
	//                 ^^^^^^^ ①のenum      ^^^^^^^ クラス名              ^^^^^^^ ImGuiのScene窓に出る表示名
}
```

これで完成です。Debug ビルドで実行すると、ImGui の **Scene** ウィンドウに「MyScene」ボタンが増え、
押すとそのシーンへ切り替わります。

---

## 補足：シーン遷移とリロード

```cpp
SceneManager::GetInstance()->ChangeScene(TITLE);   // 別シーンへ
SceneManager::GetInstance()->ChangeScene(MYSCENE); // 現在と同じ番号ならリロード（作り直し）
```

- 切り替えは **次の Update** で行われます（描画の途中では差し替わらない）。
- 遷移時は今のシーンの `Finalize()` → 新シーンの `Initialize()` が呼ばれます。
  生成したパーティクルや再生中の音などは `Finalize()` で後始末しておくと、再入場時の二重生成を防げます（`SampleScene::Finalize()` 参照）。

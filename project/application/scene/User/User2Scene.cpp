#include "User2Scene.h"
#include "GameScenes.h"
#include "SceneManager.h" // シーン遷移を使うとき用

void User2Scene::Initialize() {
	// このシーンを現在シーンとして固定（静的sceneNoのクセ対策。エンジン改善までの作法）
	SetSceneNo(USER2);

	// 最低限のカメラ
	camera_ = std::make_unique<TuboEngine::Camera>();
	camera_->SetTranslate({ 0.0f, 0.0f, -15.0f });
	camera_->setRotation({ 0.0f, 0.0f, 0.0f });
	camera_->setScale({ 1.0f, 1.0f, 1.0f });
	camera_->Update();
}

void User2Scene::Update() {
	camera_->Update();
	// TODO: ここに更新処理（入力・ゲームロジック）を書く
	// 別シーンへ遷移する例:  SetSceneNo(STAGE);   // 次フレームで切り替わる
}

void User2Scene::Finalize() {}

void User2Scene::Object3DDraw() {}   // TODO: 3Dオブジェクト描画
void User2Scene::SpriteDraw()  {}    // TODO: 2Dスプライト描画
void User2Scene::ImGuiDraw()   {}    // TODO: ImGui描画
void User2Scene::ParticleDraw(){}    // TODO: パーティクル描画

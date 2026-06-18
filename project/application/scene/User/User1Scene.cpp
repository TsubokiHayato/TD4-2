#include "User1Scene.h"
#include "GameScenes.h"
#include "SceneManager.h" // シーン遷移を使うとき用

void User1Scene::Initialize() {
	// 最低限のカメラ
	camera_ = std::make_unique<TuboEngine::Camera>();
	camera_->SetTranslate({ 0.0f, 0.0f, -15.0f });
	camera_->setRotation({ 0.0f, 0.0f, 0.0f });
	camera_->setScale({ 1.0f, 1.0f, 1.0f });
	camera_->Update();
}

void User1Scene::Update() {
	camera_->Update();
	// TODO: ここに更新処理（入力・ゲームロジック）を書く
	// 別シーンへ遷移する例:  SceneManager::GetInstance()->ChangeScene(STAGE);   // 次フレームで切り替わる
}

void User1Scene::Finalize() {}

void User1Scene::Object3DDraw() {}   // TODO: 3Dオブジェクト描画
void User1Scene::SpriteDraw()  {}    // TODO: 2Dスプライト描画
void User1Scene::ImGuiDraw()   {}    // TODO: ImGui描画
void User1Scene::ParticleDraw(){}    // TODO: パーティクル描画

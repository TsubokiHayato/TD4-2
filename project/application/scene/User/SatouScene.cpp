#include "SatouScene.h"
#include "GameScenes.h"
#include "SceneManager.h" // シーン遷移を使うとき用

void SatouScene::Initialize() {
	// 最低限のカメラ
	camera_ = std::make_unique<TuboEngine::Camera>();
	camera_->SetTranslate({ 0.0f, 0.0f, -15.0f });
	camera_->setRotation({ 0.0f, 0.0f, 0.0f });
	camera_->setScale({ 1.0f, 1.0f, 1.0f });
	camera_->Update();
}

void SatouScene::Update() {
	camera_->Update();
	// TODO: ここに更新処理（入力・ゲームロジック）を書く
	// 別シーンへ遷移する例:  SceneManager::GetInstance()->ChangeScene(STAGE);   // 次フレームで切り替わる
}

void SatouScene::Finalize() {}

void SatouScene::Object3DDraw() {}   // TODO: 3Dオブジェクト描画
void SatouScene::SpriteDraw()  {}    // TODO: 2Dスプライト描画
void SatouScene::ImGuiDraw()   {}    // TODO: ImGui描画
void SatouScene::ParticleDraw(){}    // TODO: パーティクル描画

#include "OosakiScene.h"
#include "OosakiScene.h"
#include "GameScenes.h"
#include "SceneManager.h" // シーン遷移を使うとき用

#include "Input.h"

void OosakiScene::Initialize() {
	// 最低限のカメラ
	camera_ = std::make_unique<TuboEngine::Camera>();
	camera_->SetTranslate({ 0.0f, 0.0f, -15.0f });
	camera_->setRotation({ 0.0f, 0.0f, 0.0f });
	camera_->setScale({ 1.0f, 1.0f, 1.0f });
	camera_->Update();

	rubikCube_ = std::make_unique<RubikCube>();
	rubikCube_->Initialize();
}

void OosakiScene::Update() {
	camera_->Update();
	// TODO: ここに更新処理（入力・ゲームロジック）を書く
	// 別シーンへ遷移する例:  SceneManager::GetInstance()->ChangeScene(STAGE);   // 次フレームで切り替わる

	rubikCube_->Update();
}

void OosakiScene::Finalize() {}

void OosakiScene::Object3DDraw() {} // TODO: 3Dオブジェクト描画
void OosakiScene::SpriteDraw() {}   // TODO: 2Dスプライト描画
void OosakiScene::ImGuiDraw() {
	rubikCube_->Debug();
}    // TODO: ImGui描画


void OosakiScene::ParticleDraw() {} // TODO: パーティクル描画

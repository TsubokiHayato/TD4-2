#include "GameClearScene.h"
#include "GameScenes.h"
#include "SceneManager.h" // シーン遷移を使うとき用

void GameClearScene::Initialize() {
	// このシーンを現在シーンとして固定（静的sceneNoのクセ対策。エンジン改善までの作法）
	SetSceneNo(CLEAR);

	// 最低限のカメラ
	camera_ = std::make_unique<TuboEngine::Camera>();
	camera_->SetTranslate({ 0.0f, 0.0f, -15.0f });
	camera_->setRotation({ 0.0f, 0.0f, 0.0f });
	camera_->setScale({ 1.0f, 1.0f, 1.0f });
	camera_->Update();
}

void GameClearScene::Update() {
	camera_->Update();
	// TODO: ここに更新処理（入力・ゲームロジック）を書く
	// 別シーンへ遷移する例:  SetSceneNo(STAGE);   // 次フレームで切り替わる
}

void GameClearScene::Finalize() {}

void GameClearScene::Object3DDraw() {}   // TODO: 3Dオブジェクト描画
void GameClearScene::SpriteDraw()  {}    // TODO: 2Dスプライト描画
void GameClearScene::ImGuiDraw()   {}    // TODO: ImGui描画
void GameClearScene::ParticleDraw(){}    // TODO: パーティクル描画

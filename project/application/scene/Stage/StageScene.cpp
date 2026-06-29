#include "StageScene.h"
#include "GameScenes.h"
#include "SceneManager.h" // シーン遷移を使うとき用

void StageScene::Initialize() {
	// 最低限のカメラ
	camera_ = std::make_unique<TuboEngine::Camera>();
	camera_->SetTranslate({ 0.0f, 0.0f, -15.0f });
	camera_->setRotation({ 0.0f, 0.0f, 0.0f });
	camera_->setScale({ 1.0f, 1.0f, 1.0f });
	camera_->Update();

	//Uiクラスの初期化
	ui_ = std::make_unique<Ui>();
	ui_->Initialize();

}

void StageScene::Update() {
	camera_->Update();
	// TODO: ここに更新処理（入力・ゲームロジック）を書く

	//Uiの更新
	ui_->Update();

	// 別シーンへ遷移する例:  SceneManager::GetInstance()->ChangeScene(CLEAR);   // 次フレームで切り替わる
}

void StageScene::Finalize() {}

void StageScene::Object3DDraw() {

}   // TODO: 3Dオブジェクト描画
void StageScene::SpriteDraw() {
	//UIの描画
	ui_->DrawStageScene();
}    // TODO: 2Dスプライト描画
void StageScene::ImGuiDraw() {
    //
	ui_->Debug();
}    // TODO: ImGui描画
void StageScene::ParticleDraw() {}    // TODO: パーティクル描画

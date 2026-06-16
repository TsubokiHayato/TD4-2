#include "SampleScene.h"
#include "GameScenes.h"

void SampleScene::Initialize() {
	// このシーンを開始シーンとして固定する。
	// （エンジンの IScene::sceneNo 静的初期値=1 が次フレームで効くクセ対策）
	SetSceneNo(SAMPLE);

	// カメラを1つ用意（描画に最低限必要）
	camera_ = std::make_unique<TuboEngine::Camera>();
	camera_->SetTranslate({ 0.0f, 0.0f, -15.0f });
	camera_->setRotation({ 0.0f, 0.0f, 0.0f });
	camera_->setScale({ 1.0f, 1.0f, 1.0f });
	camera_->Update();
}

void SampleScene::Update() {
	camera_->Update();
}

void SampleScene::Finalize() {}

// まだ何も描かない＝画面クリア色だけが見える状態。ここから足していく。
void SampleScene::Object3DDraw() {}
void SampleScene::SpriteDraw() {}
void SampleScene::ImGuiDraw() {}
void SampleScene::ParticleDraw() {}

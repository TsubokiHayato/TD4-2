#include "OosakiScene.h"
#include "OosakiScene.h"
#include "GameScenes.h"
#include "SceneManager.h" // シーン遷移を使うとき用

#include "Input.h"

#define _USE_MATH_DEFINES
#include <math.h>

void OosakiScene::Initialize() {
	// 最低限のカメラ
	camera_ = std::make_unique<TuboEngine::Camera>();
	camera_->SetTranslate({ 0.0f, 0.0f, -15.0f });
	camera_->setRotation({ 0.0f, 0.0f, 0.0f });
	camera_->setScale({ 1.0f, 1.0f, 1.0f });
	camera_->Update();

	rubikCube_ = std::make_unique<RubikCube>();
	rubikCube_->Initialize(camera_.get());


	transform_.translate = { 0,0,-15.0f };
	transform_.rotate = { 0,0,0 };
	transform_.scale = { 1,1,1 };
}

void OosakiScene::Update() {
	if (TuboEngine::Input::GetInstance()->PushKey(DIK_LEFT)) {
		transform_.translate.x -= 0.25f;
	}
	if (TuboEngine::Input::GetInstance()->PushKey(DIK_RIGHT)) {
		transform_.translate.x += 0.25f;
	}
	if (TuboEngine::Input::GetInstance()->PushKey(DIK_UP)) {
		transform_.translate.y += 0.25f;
	}
	if (TuboEngine::Input::GetInstance()->PushKey(DIK_DOWN)) {
		transform_.translate.y -= 0.25f;
	}
	if (TuboEngine::Input::GetInstance()->PushKey(DIK_P)) {
		transform_.translate.z += 0.25f;
	}
	if (TuboEngine::Input::GetInstance()->PushKey(DIK_L)) {
		transform_.translate.z -= 0.25f;
	}


	if (TuboEngine::Input::GetInstance()->PushKey(DIK_U)) {
		transform_.rotate.y += (float(M_PI) / 180.0f);
	}
	if (TuboEngine::Input::GetInstance()->PushKey(DIK_O)) {
		transform_.rotate.y -= (float(M_PI) / 180.0f);
	}
		
	camera_->SetTranslate(transform_.translate);
	camera_->setRotation(transform_.rotate);
	camera_->Update();
	
	
	// TODO: ここに更新処理（入力・ゲームロジック）を書く
	// 別シーンへ遷移する例:  SceneManager::GetInstance()->ChangeScene(STAGE);   // 次フレームで切り替わる

	rubikCube_->Update();
}

void OosakiScene::Finalize() {}

void OosakiScene::Object3DDraw() {
	rubikCube_->Draw();
} // TODO: 3Dオブジェクト描画

void OosakiScene::SpriteDraw() {}   // TODO: 2Dスプライト描画
void OosakiScene::ImGuiDraw() {
	rubikCube_->Debug();
}    // TODO: ImGui描画


void OosakiScene::ParticleDraw() {} // TODO: パーティクル描画

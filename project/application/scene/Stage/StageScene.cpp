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

	//ポーズメニューキューブの初期化
	pauseMenuCube_ = std::make_unique<TuboEngine::Object3d>();
	pauseMenuCube_->Initialize("block/block.obj");
	pauseMenuCube_->SetCamera(camera_.get());
	pauseMenuCube_->SetPosition({ 0,0,0 });
	pauseMenuCube_->SetScale({ 0,0,0 });

	//Uiクラスの初期化
	ui_ = std::make_unique<Ui>();
	ui_->Initialize();

}

void StageScene::Update() {
	camera_->Update();
	// TODO: ここに更新処理（入力・ゲームロジック）を書く

		//キューブの回転アニメーション
	CubeAnimation();

	//ポーズメニューキューブ
	pauseMenuCube_->Update();

	//Uiの更新
	ui_->Update();

	//ポーズメニューのスケールを取得
	cubeScale_ = ui_->GetPauseScale();

	pauseMenuCube_->SetScale({
		cubeBaseScale_ * cubeScale_,
		cubeBaseScale_ * cubeScale_,
		cubeBaseScale_ * cubeScale_
		});


	//ポーズメニューでのシーン切り替え
	ChangeSceneFromPause();

	// 別シーンへ遷移する例:  SceneManager::GetInstance()->ChangeScene(CLEAR);   // 次フレームで切り替わる
}

void StageScene::Finalize() {}

void StageScene::Object3DDraw() {

	//ポーズメニューキューブの描画
	if (cubeScale_ > 0.01f) {

		pauseMenuCube_->Draw();
	}
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
//キューブの回転アニメーション
void StageScene::CubeAnimation() {

	bool rotating = ui_->IsRotating();

	const float PI = 3.1415926f;
	const float DEG90 = PI * 0.5f;

	// 回転終了時に確定
	if (prevRotating_ && !rotating) {

		basecubeAngle_ += ui_->GetRotateDir() * DEG90;

		// 正規化（-π〜πでもOK）
		if (basecubeAngle_ >= PI * 2.0f) basecubeAngle_ -= PI * 2.0f;
		if (basecubeAngle_ < 0.0f) basecubeAngle_ += PI * 2.0f;
	}

	float drawAngle = basecubeAngle_;

	if (rotating) {

		float t = ui_->GetRotateTimer();

		// ease
		t = (t < 0.5f)
			? 2.0f * t * t
			: 1.0f - std::pow(-2.0f * t + 2.0f, 2.0f) / 2.0f;

		drawAngle += ui_->GetRotateDir() * DEG90 * t;
	}

	pauseMenuCube_->SetRotation({
		0.0f,
		drawAngle,
		0.0f
		});

	prevRotating_ = rotating;
}
//ポーズメニューでのシーン切り替え
void StageScene::ChangeSceneFromPause() {
	switch (ui_->GetPauseMenu()) {
	case Ui::PauseMenuType::Retry:
		SceneManager::GetInstance()->ChangeScene(STAGE);
		ui_->SetPauseMenu(Ui::PauseMenuType::None);
		break;
	case Ui::PauseMenuType::ToTitle:
		SceneManager::GetInstance()->ChangeScene(TITLE);
		ui_->SetPauseMenu(Ui::PauseMenuType::None);
		break;
	case Ui::PauseMenuType::ToSelect:

		ui_->SetPauseMenu(Ui::PauseMenuType::None);
		break;
	default:
		break;
	}
}
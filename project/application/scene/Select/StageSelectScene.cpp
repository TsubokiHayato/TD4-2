#include "StageSelectScene.h"
#include "GameScenes.h"
#include "SceneManager.h" // シーン遷移を使うとき用
#include "TextManager.h"
#include "Stage/StageScene.h"
#include "Input.h"
#ifdef USE_IMGUI
#include "externals/imgui/imgui.h"
#endif

// このシーンで使うエンジンの型は基本 TuboEngine 名前空間にある。
using namespace TuboEngine;

void StageSelectScene::Initialize() {
	// 最低限のカメラ
	camera_ = std::make_unique<TuboEngine::Camera>();
	camera_->SetTranslate({ 0.0f, 0.0f, -15.0f });
	camera_->setRotation({ 0.0f, 0.0f, 0.0f });
	camera_->setScale({ 1.0f, 1.0f, 1.0f });
	camera_->Update();
	// デバッグカメラ
	debugCamera_ = std::make_unique<DebugCamera>();

	auto* tm = TextManager::GetInstance();

	// タイトル画面の見た目（タイトル・メニュー・カーソル・ヒント・バージョン）はすべて JSON で定義。
	// ImGui の TextManager パネルで位置や色を調整でき、Save で書き戻せる。
	tm->LoadTextLayout("Resources/Text/Select.json");

	//ステージ
	// 3x3のブロックを並べる
	for (int y = 0; y < kGridSize; ++y) {
		for (int x = 0; x < kGridSize; ++x) {
			auto block = std::make_unique<TuboEngine::Object3d>();
			block->Initialize("block/block.obj");
			block->SetCamera(camera_.get());
			block->SetScale({ kBlockScale, kBlockScale, kBlockScale });

			// 中央揃えで3x3に配置
			float posX = (static_cast<float>(x) - (kGridSize - 1) * 0.5f) * kBlockSpacing;
			float posY = -(static_cast<float>(y) - (kGridSize - 1) * 0.5f) * kBlockSpacing;
			block->SetPosition({ posX, posY, 0.0f });

			blocks_[ToIndex(x, y)] = std::move(block);
		}
	}

	selectedX_ = 0;
	selectedY_ = 0;

	fadeScreen_ = std::make_unique<FadeScreen>();
	fadeScreen_->Initialize();
}
//更新
void StageSelectScene::Update() {
	//カメラ更新
	camera_->Update();
	debugCamera_->Update(camera_.get());
	// Tキーでチュートリアルへ
	if (Input::GetInstance()->TriggerKey(DIK_T) && !isTutorialSelecting_) {
		isTutorialSelecting_ = true;
		fadeScreen_->FadeOut();
	}

	// フェード終了後にシーン遷移
	if (isTutorialSelecting_) {
		if (fadeScreen_->IsFadeOuting()) {
			SceneManager::GetInstance()->ChangeScene(TUTORIAL);
		}
	}

	//セレクトブロック
	for (auto& block : blocks_) {
		block->Update();
	}
	//選択ブロックの切り替え
	UpdateSelection();
	//選択中のブロックアニメーション
	UpdateBlockAppearance();
	//ステージ決定
	ConfirmSelection();
	// テキストの更新
	TuboEngine::TextManager::GetInstance()->UpdateAll();
	// 別シーンへ遷移する例:  SceneManager::GetInstance()->ChangeScene(STAGE);   // 次フレームで切り替わる

	fadeScreen_->Update();
}

void StageSelectScene::Finalize() {
	// タイトルの UI を次シーンへ持ち越さないよう片付ける。
	TuboEngine::TextManager::GetInstance()->ClearAllTexts();
	TuboEngine::TextManager::GetInstance()->ClearAllSprites();
}

void StageSelectScene::Object3DDraw() {
	//セレクトブロック
	for (auto& block : blocks_) {
		block->Draw();
	}
	
} // TODO: 3Dオブジェクト描画
void StageSelectScene::SpriteDraw() { 
	TuboEngine::TextManager::GetInstance()->DrawAll();
	fadeScreen_->Draw();

}   // TODO: 2Dスプライト描画

void StageSelectScene::ImGuiDraw() {
	ImGui::Begin("Stage Select");
	ImGui::Text("Selected : (%d, %d)  Index: %d / %d",
		selectedX_, selectedY_, ToIndex(selectedX_, selectedY_) + 1, kStageCount);
	ImGui::Text("[Arrow Keys] : move  [SPACE] : decide");
	ImGui::End();

	TuboEngine::TextManager::GetInstance()->DrawImGui();
}
void StageSelectScene::ParticleDraw() {} // TODO: パーティクル描画

//選択ブロックの切り替え
void StageSelectScene::UpdateSelection() {
	Input* input = Input::GetInstance();
	// 矢印キー または WASD で選択ブロックを切り替える
	if (input->TriggerKey(DIK_RIGHT) || input->TriggerKey(DIK_D)) {
		selectedX_ = (selectedX_ + 1) % kGridSize;
	}
	if (input->TriggerKey(DIK_LEFT) || input->TriggerKey(DIK_A)) {
		selectedX_ = (selectedX_ - 1 + kGridSize) % kGridSize;
	}
	if (input->TriggerKey(DIK_UP) || input->TriggerKey(DIK_W)) {
		selectedY_ = (selectedY_ - 1 + kGridSize) % kGridSize;
	}
	if (input->TriggerKey(DIK_DOWN) || input->TriggerKey(DIK_S)) {
		selectedY_ = (selectedY_ + 1) % kGridSize;
	}
}

//選択中のブロックアニメーション
void StageSelectScene::UpdateBlockAppearance() {
	// 選択中のブロックの拡縮
	for (int y = 0; y < kGridSize; ++y) {
		for (int x = 0; x < kGridSize; ++x) {
			bool isSelected = (x == selectedX_ && y == selectedY_);
			float scale = isSelected ? kBlockScale * 1.3f : kBlockScale;
			blocks_[ToIndex(x, y)]->SetScale({ scale, scale, scale });
		}
	}
}
//ステージ決定
void StageSelectScene::ConfirmSelection() {
	Input* input = Input::GetInstance();

	if (input->TriggerKey(DIK_RETURN) || input->TriggerKey(DIK_SPACE) || isSelecting_) {
		isSelecting_ = true;
		fadeScreen_->FadeOut();//フェードアウト開始
		if (!fadeScreen_->IsFadeOuting()) return;//フェードアウトが終わるまで待機
		int index = ToIndex(selectedX_, selectedY_);
		StageScene::SetSelectedStageIndex(index + 1);
		SceneManager::GetInstance()->ChangeScene(STAGE);
	}
}
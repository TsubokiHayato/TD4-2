#include "Ui.h"
#include "Input.h"

#ifdef USE_IMGUI
#include "externals/imgui/imgui.h"
#endif

using namespace TuboEngine;

//初期化
void Ui::Initialize() {
	//ポーズスプライトの初期化
	pauseSprite_ = std::make_unique<TuboEngine::Sprite>();
	pauseSprite_->Initialize("uvChecker.png");
	pauseSprite_->SetAnchorPoint({ 0.5f,0.5f });
	//リトライスプライトの初期化
	retrySprite_ = std::make_unique<TuboEngine::Sprite>();
	retrySprite_->Initialize("particle.png");
	retrySprite_->SetAnchorPoint({ 0.5f,0.5f });
	//タイトルへスプライトの初期化
	toTitleSprite_ = std::make_unique<TuboEngine::Sprite>();
	toTitleSprite_->Initialize("yellow.png");
	toTitleSprite_->SetAnchorPoint({ 0.5f,0.5f });
	//セレクトへスプライトの初期化
	toSelectSprite_ = std::make_unique<TuboEngine::Sprite>();
	toSelectSprite_->Initialize("tile.png");
	toSelectSprite_->SetAnchorPoint({ 0.5f,0.5f });
	//操作説明メニュースプライトの初期化
	controlsMenuSprite_ = std::make_unique<TuboEngine::Sprite>();
	controlsMenuSprite_->Initialize("noise0.png");
	controlsMenuSprite_->SetAnchorPoint({ 0.5f,0.5f });
	//操作説明画面スプライトの初期化
	controlsDetailSprite_ = std::make_unique<TuboEngine::Sprite>();
	controlsDetailSprite_->Initialize("noise1.png");
	controlsDetailSprite_->SetAnchorPoint({ 0.5f,0.5f });

}
//更新
void Ui::Update() {

	//ポーズメニュー更新
	UpdatePauseMenu();

	//ポーズスプライトの描画
	pauseSprite_->Update();
	retrySprite_->Update();
	toTitleSprite_->Update();
	toSelectSprite_->Update();
	controlsMenuSprite_->Update();
	controlsDetailSprite_->Update();
}
//ステージシーンの描画
void Ui::DrawStageScene() {
	//ポーズスプライトの描画
	if (pauseScale_ > 0.01f) {
		pauseSprite_->Draw();
		retrySprite_->Draw();
		toTitleSprite_->Draw();
		toSelectSprite_->Draw();
		controlsMenuSprite_->Draw();
	}
	if (pauseMenutype_ == PauseMenuType::Options && !isShowPause_) {
		controlsDetailSprite_->Draw();
	}
}

//ポーズメニューの更新
void Ui::UpdatePauseMenu() {
	//ポーズ画面表示
	if (Input::GetInstance()->TriggerKey(DIK_Q)) {

		//操作説明画面を閉じる
		if (pauseMenutype_ == PauseMenuType::Options) {
			pauseMenutype_ = PauseMenuType::None;
			isShowPause_ = true;
		}
		//通常のポーズ開閉
		else {
			isShowPause_ = !isShowPause_;

			if (isShowPause_) {
				pauseMenutype_ = PauseMenuType::None;
				pauseSelectIndex_ = 0;
				pauseSelectAnimTimer_ = 0.0f;
			}
		}
	}
	//ポーズメニューの選択
	if (isShowPause_) {
		//インデックスの範囲チェック
		if (pauseSelectIndex_ < 0 || pauseSelectIndex_ > 3) {
			assert(false);
		}
		//ポーズメニューの選択
		if (TuboEngine::Input::GetInstance()->TriggerKey(DIK_UP)) {
			pauseSelectIndex_--;
			if (pauseSelectIndex_ < 0) {
				pauseSelectIndex_ = 3;
			}
		}
		if (TuboEngine::Input::GetInstance()->TriggerKey(DIK_DOWN)) {
			pauseSelectIndex_++;
			if (pauseSelectIndex_ > 3) {
				pauseSelectIndex_ = 0;
			}
		}
		//ポーズメニューの決定
		if (TuboEngine::Input::GetInstance()->TriggerKey(DIK_SPACE)) {
			switch (pauseSelectIndex_) {

			case 0:
				pauseMenutype_ = PauseMenuType::Options;//操作説明
				isShowPause_ = false;
				break;
			case 1:
				pauseMenutype_ = PauseMenuType::Retry;//リトライ
				break;
			case 2:
				pauseMenutype_ = PauseMenuType::ToSelect;//セレクトへ
				break;
			case 3:
				pauseMenutype_ = PauseMenuType::ToTitle;//タイトルへ
				break;
			}
		}
	}
	//選択アニメーションタイマー更新
	pauseSelectAnimTimer_ += 1.0f / 60.0f;

	float selectScale = 1.0f + sinf(pauseSelectAnimTimer_ * 5.0f) * 0.08f;//選択アニメーションのスケール

	if (isShowPause_) {
		pauseScale_ += pauseAnimSpeed_;
	} else {
		pauseScale_ -= pauseAnimSpeed_;
	}

	pauseScale_ = std::clamp(pauseScale_, 0.0f, 1.0f);//スケールの範囲を制限
	//イージング計算
	float eased;
	if (pauseScale_ < 0.5f) {
		eased = 2.0f * pauseScale_ * pauseScale_;
	} else {
		eased = 1.0f - std::pow(-2.0f * pauseScale_ + 2.0f, 2.0f) / 2.0f;
	}

	float baseW = pauseSize_.x;//ポーズメニューの幅
	float baseH = pauseSize_.y;//ポーズメニューの高さ

	pauseSprite_->SetPosition(pausePos_);//ポーズメニューの位置を設定
	controlsDetailSprite_->SetPosition(controlsDetailPos_);
	pauseSprite_->SetSize({ baseW * eased, baseH * eased });//ポーズメニューのサイズを設定
	retrySprite_->SetSize({ retryBaseSize_.x * eased,retryBaseSize_.y * eased });//リトライボタンのサイズを設定
	toTitleSprite_->SetSize({ titleBaseSize_.x * eased,titleBaseSize_.y * eased });//タイトルへボタンのサイズを設定
	toSelectSprite_->SetSize({ selectBaseSize_.x * eased,selectBaseSize_.y * eased });//セレクトへボタンのサイズを設定
	controlsMenuSprite_->SetSize({ controlsMenuBaseSize_.x * eased,controlsMenuBaseSize_.y * eased });//操作説明メニューのサイズを設定
	controlsDetailSprite_->SetSize(controlsDetailBaseSize_);

	//選択中のボタンのサイズを設定
	switch (pauseSelectIndex_) {

	case 0://操作説明メニュー
		controlsMenuSprite_->SetSize({ controlsMenuBaseSize_.x * eased * selectScale,
									controlsMenuBaseSize_.y * eased * selectScale });
		break;
	case 1://リトライボタン
		retrySprite_->SetSize({ retryBaseSize_.x * eased * selectScale,
								retryBaseSize_.y * eased * selectScale });
		break;
	case 2://セレクトへボタン
		toSelectSprite_->SetSize({ selectBaseSize_.x * eased * selectScale,
								selectBaseSize_.y * eased * selectScale });
		break;
	case 3://タイトルへボタン
		toTitleSprite_->SetSize({ titleBaseSize_.x * eased * selectScale,
								titleBaseSize_.y * eased * selectScale });
		break;
	}
	
	//操作説明メニューの位置を設定
	controlsMenuSprite_->SetPosition({ (1.0f - eased) * center_.x + eased * controlsMenuPos_.x,
										(1.0f - eased) * center_.y + eased * controlsMenuPos_.y });
	//リトライボタンの位置を設定
	retrySprite_->SetPosition({ (1.0f - eased) * center_.x + eased * retryPos_.x,
								(1.0f - eased) * center_.y + eased * retryPos_.y });
	//タイトルへボタンの位置を設定
	toTitleSprite_->SetPosition({ (1.0f - eased) * center_.x + eased * totitlePos_.x,
								 (1.0f - eased) * center_.y + eased * totitlePos_.y, });
	//セレクトへボタンの位置を設定
	toSelectSprite_->SetPosition({ (1.0f - eased) * center_.x + eased * toSelectPos_.x,
									(1.0f - eased) * center_.y + eased * toSelectPos_.y });
}

//デバック
void Ui::Debug() {
#ifdef USE_IMGUI

	if (ImGui::Begin("UI Debug")) {

		ImGui::SeparatorText("Pause");

		ImGui::DragFloat2("Pause Pos", &pausePos_.x, 1.0f);
		ImGui::DragFloat2("Pause Size", &pauseSize_.x, 1.0f);

		ImGui::SeparatorText("Retry");

		ImGui::DragFloat2("Retry Pos", &retryPos_.x, 1.0f);
		ImGui::DragFloat2("Retry Size", &retryBaseSize_.x, 1.0f);

		ImGui::SeparatorText("Title");

		ImGui::DragFloat2("Title Pos", &totitlePos_.x, 1.0f);
		ImGui::DragFloat2("Title Size", &titleBaseSize_.x, 1.0f);

		ImGui::SeparatorText("Select");

		ImGui::DragFloat2("Select Pos", &toSelectPos_.x, 1.0f);
		ImGui::DragFloat2("Select Size", &selectBaseSize_.x, 1.0f);

		ImGui::SeparatorText("Controls Menu");

		ImGui::DragFloat2("Controls Menu Pos", &controlsMenuPos_.x, 1.0f);
		ImGui::DragFloat2("Controls Menu Size", &controlsMenuBaseSize_.x, 1.0f);

		ImGui::SeparatorText("Controls Detail");

		ImGui::DragFloat2("Controls Detail Pos", &controlsDetailPos_.x, 1.0f);
		ImGui::DragFloat2("Controls Detail Size", &controlsDetailBaseSize_.x, 1.0f);

		ImGui::SeparatorText("Animation");

		ImGui::Text("PauseScale : %.3f", pauseScale_);
		ImGui::Text("SelectIndex : %d", pauseSelectIndex_);

		ImGui::Separator();
	}

	ImGui::End();

#endif
}
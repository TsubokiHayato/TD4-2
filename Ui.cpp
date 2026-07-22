#include "Ui.h"
#include "Input.h"

#ifdef USE_IMGUI
#include "externals/imgui/imgui.h"
#endif

using namespace TuboEngine;

//初期化
void Ui::Initialize(){
	//ポーズスプライトの初期化
	pauseSprite_ = std::make_unique<TuboEngine::Sprite>();
	pauseSprite_->Initialize("uvChecker.png");
	pauseSprite_->SetPosition({ 50.0f,50.0f });
	pauseSprite_->SetSize({ 120.0f,120.0f });
	pauseSprite_->SetAnchorPoint({ 0.5f,0.5f });
	//再開スプライトの初期化
	resumeSprite_ = std::make_unique<TuboEngine::Sprite>();
	resumeSprite_->Initialize("Hp.png");
	resumeSprite_->SetPosition({ 640.0f,300.0f });
	resumeSprite_->SetSize({ 0.0f,0.0f });
	resumeSprite_->SetAnchorPoint({ 0.5f,0.5f });
	//リトライスプライトの初期化
	retrySprite_ = std::make_unique<TuboEngine::Sprite>();
	retrySprite_->Initialize("particle.png");
	retrySprite_->SetPosition({ 640.0f,400.0f });
	retrySprite_->SetSize({ 0.0f,0.0f });
	retrySprite_->SetAnchorPoint({ 0.5f,0.5f });
	//タイトルへスプライトの初期化
	toTitleSprite_ = std::make_unique<TuboEngine::Sprite>();
	toTitleSprite_->Initialize("yellow.png");
	toTitleSprite_->SetPosition({ 640.0f,500.0f });
	toTitleSprite_->SetSize({ 0.0f,0.0f });
	toTitleSprite_->SetAnchorPoint({ 0.5f,0.5f });
}
//更新
void Ui::Update(){

	//ポーズメニュー更新
	UpdatePauseMenu();

	//ポーズスプライトの描画
	pauseSprite_->Update();
	resumeSprite_->Update();
	retrySprite_->Update();
	toTitleSprite_->Update();
}
//ステージシーンの描画
void Ui::DrawStageScene() {
	//ポーズスプライトの描画
	if (pauseScale_ > 0.01f) {
		pauseSprite_->Draw();
		resumeSprite_->Draw();
		retrySprite_->Draw();
		toTitleSprite_->Draw();
	}
}

//ポーズメニューの更新
void Ui::UpdatePauseMenu() {
	//ポーズ画面表示
	if (TuboEngine::Input::GetInstance()->TriggerKey(DIK_Q)) {
		isShowPause_ = !isShowPause_;
		if (isShowPause_) {
			pauseMenutype_ = PauseMenuType::None;
		}
	}
	//ポーズメニューの選択
	if (isShowPause_) {
		//インデックスの範囲チェック
		if (pauseSelectIndex_ < 0 || pauseSelectIndex_ > 2) {
			assert(false);
		}
		//ポーズメニューの選択
		if (TuboEngine::Input::GetInstance()->TriggerKey(DIK_UP)) {
			pauseSelectIndex_--;
			if (pauseSelectIndex_ < 0) {
				pauseSelectIndex_ = 2;
			}
		}
		if (TuboEngine::Input::GetInstance()->TriggerKey(DIK_DOWN)) {
			pauseSelectIndex_++;
			if (pauseSelectIndex_ > 2) {
				pauseSelectIndex_ = 0;
			}
		}
		//ポーズメニューの決定
		if (TuboEngine::Input::GetInstance()->TriggerKey(DIK_SPACE)) {
			switch (pauseSelectIndex_) {

			case 0:
				pauseMenutype_ = PauseMenuType::Resume;//再開
				isShowPause_ = false;
				break;
			case 1:
				pauseMenutype_ = PauseMenuType::Retry;//リトライ
				break;
			case 2:
				pauseMenutype_ = PauseMenuType::ToTitle;//タイトルへ
				break;
			}
		}
	}
	//選択アニメーションタイマー更新
	pauseSelectAnimTimer_ += 1.0f / 60.0f;

	float offsetX = sinf(pauseSelectAnimTimer_ * 4.0f) * 8.0f;//選択アニメーションのオフセット
	float selectScale = 1.0f + sinf(pauseSelectAnimTimer_ * 5.0f) * 0.08f;//選択アニメーションのスケール
	
	if (isShowPause_) {
		pauseScale_ += pauseAnimSpeed_;
	} else {
		pauseScale_ -= pauseAnimSpeed_;
	}
    
	pauseScale_ = std::clamp(pauseScale_, 0.0f, 1.0f);//スケールの範囲を制限

	float eased = pauseScale_ * pauseScale_;//イージング処理

	float baseW = pauseSize_.x;//ポーズメニューの幅
	float baseH = pauseSize_.y;//ポーズメニューの高さ

	pauseSprite_->SetSize({ baseW * eased, baseH * eased });//ポーズメニューのサイズを設定
	resumeSprite_->SetSize({ resumeBaseSize_.x * eased,resumeBaseSize_.y * eased });//再開ボタンのサイズを設定
	retrySprite_->SetSize({ retryBaseSize_.x * eased,retryBaseSize_.y * eased });//リトライボタンのサイズを設定
	toTitleSprite_->SetSize({ titleBaseSize_.x * eased,titleBaseSize_.y * eased });//タイトルへボタンのサイズを設定

	//選択中のボタンのサイズを設定
	switch (pauseSelectIndex_) {
	 
	case 0:
		resumeSprite_->SetSize({ resumeBaseSize_.x * eased * selectScale,
								resumeBaseSize_.y * eased * selectScale });
		break;
	case 1:
		retrySprite_->SetSize({ retryBaseSize_.x * eased * selectScale,
								retryBaseSize_.y * eased * selectScale });
		break;
	case 2:
		toTitleSprite_->SetSize({ titleBaseSize_.x * eased * selectScale,
								titleBaseSize_.y * eased * selectScale });
		break;
	}
	//ポーズメニューの位置を設定
	resumeSprite_->SetPosition({(1.0f - eased) * center_.x + eased * resumePos_.x,
	                            (1.0f - eased) * center_.y + eased * resumePos_.y});
	//リトライボタンの位置を設定
	retrySprite_->SetPosition({ (1.0f - eased) * center_.x + eased * retryPos_.x,
								(1.0f - eased) * center_.y + eased * retryPos_.y });
	//タイトルへボタンの位置を設定
	toTitleSprite_->SetPosition({ (1.0f - eased) * center_.x + eased * totitlePos_.x,
								 (1.0f - eased) * center_.y + eased * totitlePos_.y, });
}

//デバック
void Ui::Debug() {
#ifdef USE_IMGUI

	if (ImGui::Begin("UI Debug")) {

		ImGui::SeparatorText("Pause");

		ImGui::DragFloat2("Pause Size", &pauseSize_.x, 1.0f);

		ImGui::SeparatorText("Resume");

		ImGui::DragFloat2("Resume Pos", &resumePos_.x, 1.0f);
		ImGui::DragFloat2("Resume Size", &resumeBaseSize_.x, 1.0f);

		if (ImGui::Button("Copy Resume")) {
			printf(
				"\nresumePos_ = { %.1ff, %.1ff };"
				"\nresumeBaseSize_ = { %.1ff, %.1ff };\n",
				resumePos_.x, resumePos_.y,
				resumeBaseSize_.x, resumeBaseSize_.y
			);
		}

		ImGui::SeparatorText("Retry");

		ImGui::DragFloat2("Retry Pos", &retryPos_.x, 1.0f);
		ImGui::DragFloat2("Retry Size", &retryBaseSize_.x, 1.0f);

		if (ImGui::Button("Copy Retry")) {
			printf(
				"\nretryPos_ = { %.1ff, %.1ff };"
				"\nretryBaseSize_ = { %.1ff, %.1ff };\n",
				retryPos_.x, retryPos_.y,
				retryBaseSize_.x, retryBaseSize_.y
			);
		}

		ImGui::SeparatorText("Title");

		ImGui::DragFloat2("Title Pos", &totitlePos_.x, 1.0f);
		ImGui::DragFloat2("Title Size", &titleBaseSize_.x, 1.0f);

		if (ImGui::Button("Copy Title")) {
			printf(
				"\ntotitlePos_ = { %.1ff, %.1ff };"
				"\ntitleBaseSize_ = { %.1ff, %.1ff };\n",
				totitlePos_.x, totitlePos_.y,
				titleBaseSize_.x, titleBaseSize_.y
			);
		}

		ImGui::SeparatorText("Animation");

		ImGui::Text("PauseScale : %.3f", pauseScale_);
		ImGui::Text("SelectIndex : %d", pauseSelectIndex_);

		ImGui::Separator();

		if (ImGui::Button("Dump All Settings")) {

			printf("\n================ UI SETTINGS ================\n");

			printf("pauseSize_ = { %.1ff, %.1ff };\n",
				pauseSize_.x,
				pauseSize_.y);

			printf("resumePos_ = { %.1ff, %.1ff };\n",
				resumePos_.x,
				resumePos_.y);

			printf("resumeBaseSize_ = { %.1ff, %.1ff };\n",
				resumeBaseSize_.x,
				resumeBaseSize_.y);

			printf("retryPos_ = { %.1ff, %.1ff };\n",
				retryPos_.x,
				retryPos_.y);

			printf("retryBaseSize_ = { %.1ff, %.1ff };\n",
				retryBaseSize_.x,
				retryBaseSize_.y);

			printf("totitlePos_ = { %.1ff, %.1ff };\n",
				totitlePos_.x,
				totitlePos_.y);

			printf("titleBaseSize_ = { %.1ff, %.1ff };\n",
				titleBaseSize_.x,
				titleBaseSize_.y);

			printf("pauseAnimSpeed_ = %.3ff;\n",
				pauseAnimSpeed_);

			printf("=============================================\n");
		}
	}

	ImGui::End();

#endif
}
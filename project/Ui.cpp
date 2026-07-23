#include "Ui.h"
#include "Input.h"
#include <algorithm>
#include <cassert>
#include <cmath>

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
	retrySprite_->Initialize("retry.png");
	retrySprite_->SetAnchorPoint({ 0.5f,0.5f });
	retrySprite_->SetPosition(center_);
	retrySprite_->SetSize({ 0,0 });
	//タイトルへスプライトの初期化
	toTitleSprite_ = std::make_unique<TuboEngine::Sprite>();
	toTitleSprite_->Initialize("totitle.png");
	toTitleSprite_->SetAnchorPoint({ 0.5f,0.5f });
	toTitleSprite_->SetPosition(center_);
	toTitleSprite_->SetSize({ 0,0 });
	//セレクトへスプライトの初期化
	toSelectSprite_ = std::make_unique<TuboEngine::Sprite>();
	toSelectSprite_->Initialize("toselect.png");
	toSelectSprite_->SetAnchorPoint({ 0.5f,0.5f });
	toSelectSprite_->SetPosition(center_);
	toSelectSprite_->SetSize({ 0,0 });
	//操作説明メニュースプライトの初期化
	controlsMenuSprite_ = std::make_unique<TuboEngine::Sprite>();
	controlsMenuSprite_->Initialize("operationtext.png");
	controlsMenuSprite_->SetAnchorPoint({ 0.5f,0.5f });
	controlsMenuSprite_->SetPosition(center_);
	controlsMenuSprite_->SetSize({ 0,0 });
	//操作説明画面スプライトの初期化
	controlsDetailSprite_ = std::make_unique<TuboEngine::Sprite>();
	controlsDetailSprite_->Initialize("operation.png");
	controlsDetailSprite_->SetAnchorPoint({ 0.5f,0.5f });
	controlsDetailSprite_->SetSize({ 3,3 });
	//選択ガイドスプライト初期化
	selectguideSprite_ = std::make_unique<TuboEngine::Sprite>();
	selectguideSprite_->Initialize("selectguide.png");
	selectguideSprite_->SetAnchorPoint({ 0.5f,0.5f });
	selectguideSprite_->SetPosition(selectguidePos_);
	selectguideSprite_->SetSize(selectguideBaseSize_);
	//決定ガイドスプライト初期化
	enterguideSprite_ = std::make_unique<TuboEngine::Sprite>();
	enterguideSprite_->Initialize("enter.png");
	enterguideSprite_->SetAnchorPoint({ 0.5f,0.5f });
	enterguideSprite_->SetPosition(enterguidePos_);
	enterguideSprite_->SetSize(enterguideBaseSize_);
	
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
	selectguideSprite_->Update();
	enterguideSprite_->Update();

	//ポーズメニューの回転処理
	if (isRotating_) {

		rotateTimer_ += rotateSpeed_;//回転タイマーの更新
		//回転タイマーの範囲を制限
		if (rotateTimer_ >= 1.0f) {

			rotateTimer_ = 1.0f;
			isRotating_ = false;

			pauseSelectIndex_ =
				(pauseSelectIndex_ + rotateDir_ + 4) % 4;//インデックスの範囲を制限
		}
	}
}
//ステージシーンの描画
void Ui::DrawStageScene() {
	if (pauseScale_ > 0.01f) {

		switch (pauseSelectIndex_) {

		case 0://操作説明
			controlsMenuSprite_->Draw();
			break;

		case 1://リトライ
			retrySprite_->Draw();
			break;

		case 2://セレクトへ
			toSelectSprite_->Draw();
			break;

		case 3://タイトルへ
			toTitleSprite_->Draw();
			break;
		}
		//選択キーガイドの描画
		selectguideSprite_->Draw();
		//決定キーガイドの描画
		enterguideSprite_->Draw();
	}
	//操作説明画面の描画
	if (pauseMenutype_ == PauseMenuType::Options && !isShowPause_) {
		controlsDetailSprite_->Draw();
	}
}

//ポーズメニューの更新
void Ui::UpdatePauseMenu() {
	//ポーズ画面表示
	if (Input::GetInstance()->TriggerKey(DIK_ESCAPE)) {

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
		TuboEngine::Input* in = TuboEngine::Input::GetInstance();
		if (!isRotating_) {
		//ポーズメニューの選択(←/A/W=左, →/D/S=右)
		if (in->TriggerKey(DIK_LEFT) || in->TriggerKey(DIK_A) || in->TriggerKey(DIK_UP) || in->TriggerKey(DIK_W)) {

			rotateDir_ = -1;
			rotateTimer_ = 0.0f;
			isRotating_ = true;
		}

		if (in->TriggerKey(DIK_RIGHT) || in->TriggerKey(DIK_D) || in->TriggerKey(DIK_DOWN) || in->TriggerKey(DIK_S)) {

			rotateDir_ = 1;
			rotateTimer_ = 0.0f;
			isRotating_ = true;
		}
	}
		//ポーズメニューの決定(Enter/Space)
		if (in->TriggerKey(DIK_RETURN) || in->TriggerKey(DIK_SPACE)) {
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

	float selectScale = 1.0f + sinf(pauseSelectAnimTimer_ * 5.0f) * 0.08f;

	Vector2 frontPos = center_;//正面の位置
	Vector2 rightPos = { center_.x + 260.0f, center_.y };//右の位置
	Vector2 leftPos = { center_.x - 260.0f, center_.y };//左の位置
	Vector2 backPos = { center_.x, center_.y - 120.0f };//後ろの位置

	const float frontScale = 1.0f;//正面のスケール
	const float sideScale = 0.75f;//左右のスケール
	const float backScale = 0.6f;//後ろのスケール

	pauseSprite_->SetPosition(pausePos_);//ポーズメニューの位置を設定
	controlsDetailSprite_->SetPosition(controlsDetailPos_);//操作説明画面の位置を設定
	selectguideSprite_->SetPosition(selectguidePos_);//選択キーガイドの位置を設定
	enterguideSprite_->SetPosition(enterguidePos_);//決定キーガイドの位置を設定
	pauseSprite_->SetSize({ baseW * eased, baseH * eased });//ポーズメニューのサイズを設定
	controlsDetailSprite_->SetSize(controlsDetailBaseSize_);//操作説明画面のサイズを設定
	selectguideSprite_->SetSize(selectguideBaseSize_);//選択キーガイドのサイズを設定
	enterguideSprite_->SetSize(enterguideBaseSize_);//決定キーガイドのサイズを設定

	TuboEngine::Sprite* sprite = nullptr;
	Vector2 size;
	//選択中のスプライトとサイズを設定
	switch (pauseSelectIndex_) {
	case 0://操作説明
		sprite = controlsMenuSprite_.get();
		size = controlsMenuBaseSize_;
		break;
	case 1://リトライ
		sprite = retrySprite_.get();
		size = retryBaseSize_;
		break;
	case 2://セレクトへ
		sprite = toSelectSprite_.get();
		size = selectBaseSize_;
		break;
	case 3://タイトルへ
		sprite = toTitleSprite_.get();
		size = titleBaseSize_;
		break;
	}

	Vector2 drawPos = center_;//描画位置
	float drawScale = eased * selectScale;//描画スケール
	float widthScale = 1.0f;//描画幅スケール
	// 回転中の処理
	if (isRotating_) {

		float t = rotateTimer_;

		// イージング
		if (t < 0.5f) {
			t = 2.0f * t * t;
		} else {
			t = 1.0f - std::pow(-2.0f * t + 2.0f, 2.0f) / 2.0f;
		}
		//右回転の時
		if (rotateDir_ == 1) {

			
			if (t < 0.5f) {

				float local = t / 0.5f;

				drawPos.x = center_.x - 120.0f * local;
				widthScale = 1.0f - local;
			} else {

				float local = (t - 0.5f) / 0.5f;

				drawPos.x = center_.x + 120.0f * (1.0f - local);
				widthScale = local;
			}
		} else {//左回転の時

			if (t < 0.5f) {

				float local = t / 0.5f;

				drawPos.x = center_.x + 120.0f * local;
				widthScale = 1.0f - local;
			} else {

				float local = (t - 0.5f) / 0.5f;

				drawPos.x = center_.x - 120.0f * (1.0f - local);
				widthScale = local;
			}
		}
	}
	//スプライトの位置を設定
	sprite->SetPosition(drawPos);
	//スプライトのサイズを設定
	sprite->SetSize({
		size.x * drawScale * widthScale,
		size.y * drawScale
		});

}
//ポーズメニューの取得
Ui::PauseMenuType Ui::GetPauseMenu() {
	return pauseMenutype_;
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

		ImGui::SeparatorText("Select Guide");

		ImGui::DragFloat2("Select Guide Pos", &selectguidePos_.x, 1.0f);
		ImGui::DragFloat2("Select Guide Size", &selectguideBaseSize_.x, 1.0f);

		ImGui::SeparatorText("Enter Guide");

		ImGui::DragFloat2("Enter Guide Pos", &enterguidePos_.x, 1.0f);
		ImGui::DragFloat2("Enter Guide Size", &enterguideBaseSize_.x, 1.0f);

		ImGui::SeparatorText("Animation");

		ImGui::Text("PauseScale : %.3f", pauseScale_);
		ImGui::Text("SelectIndex : %d", pauseSelectIndex_);

		ImGui::Separator();
	}

	ImGui::End();

#endif
}
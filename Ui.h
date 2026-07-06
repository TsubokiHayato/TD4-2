#pragma once
#include "Sprite.h"

//UIクラス
class Ui
{
public:
	//ポーズメニューの種類
	enum class PauseMenuType {
		Resume,//再開
		Retry,//リトライ
		ToTitle,//タイトルへ
		None//なし
	};

	//初期化
	void Initialize();
	//更新
	void Update();
	//ステージシーンの描画
	void DrawStageScene();
	//デバック
	void Debug();
	//ポーズメニューの取得
	PauseMenuType GetPauseMenu();
	//ポーズメニューの設定
	void SetPauseMenu(PauseMenuType type) { type = pauseMenutype_; }
private:
	//ポーズメニューの更新
	void UpdatePauseMenu();
private:
	PauseMenuType pauseMenutype_ = PauseMenuType::None;//ポーズメニューの種類

	std::unique_ptr<TuboEngine::Sprite>pauseSprite_;//ポーズ
	std::unique_ptr<TuboEngine::Sprite>resumeSprite_;//再開
	std::unique_ptr<TuboEngine::Sprite>retrySprite_;//リトライ
	std::unique_ptr<TuboEngine::Sprite>toTitleSprite_;//タイトルへ

	Math::Vector2 pauseSize_ = { 400.0f,300.0f };//ポーズメニューのサイズ
	Math::Vector2 resumeBaseSize_ = { 268.0f,49.0f };//再開ボタンのサイズ
	Math::Vector2 retryBaseSize_ = { 192.0f,49.0f };//リトライボタンのサイズ
	Math::Vector2 titleBaseSize_ = { 336.0f,49.0f };//タイトルへボタンのサイズ

	Math::Vector2 center_ = { 640.0f,360.0f };//画面の中心座標
	Math::Vector2 resumePos_ = { 640.0f,250.0f };//再開ボタンの座標
	Math::Vector2 retryPos_ = { 640.0f,380.0f };//リトライボタンの座標
	Math::Vector2 totitlePos_ = { 640.0f,510.0f };//タイトルへボタンの座標}

	int pauseSelectIndex_ = 0;//ポーズメニューの選択インデックス

	float pauseSelectAnimTimer_ = 0.0f;//ポーズメニューの選択アニメーションタイマー
	float pauseScale_ = 0.0f;//ポーズメニューのスケール

	bool isShowPause_ = false;//ポーズメニュー表示フラグ

	const float pauseAnimSpeed_ = 0.1f;//ポーズメニューのアニメーション速度
};
#pragma once
#include "Sprite.h"

using TuboEngine::Math::Vector2;

//UIクラス
class Ui
{
public:
	//ポーズメニューの種類
	enum class PauseMenuType {
		Options,//操作説明
		Retry,//リトライ
		ToSelect,//セレクトへ
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
	//ポーズメニューの表示状態の取得
	bool IsRotating(){ return isRotating_; }
	//ポーズメニューが開いているか
	bool IsPauseVisible()  {return isShowPause_;}
	//回転方向の取得
	int GetRotateDir() { return rotateDir_; }
	//回転タイマーの取得
	float GetRotateTimer() { return rotateTimer_; }
	//ポーズのスケール取得
	float GetPauseScale() { return pauseScale_; }

	//ポーズメニューの設定
	void SetPauseMenu(PauseMenuType type) { pauseMenutype_ = type; }
private:
	//ポーズメニューの更新
	void UpdatePauseMenu();
private:
	PauseMenuType pauseMenutype_ = PauseMenuType::None;//ポーズメニューの種類

	std::unique_ptr<TuboEngine::Sprite>pauseSprite_;//ポーズ
	std::unique_ptr<TuboEngine::Sprite>retrySprite_;//リトライ
	std::unique_ptr<TuboEngine::Sprite>toTitleSprite_;//タイトルへ
	std::unique_ptr<TuboEngine::Sprite>toSelectSprite_;//セレクトへ
	std::unique_ptr<TuboEngine::Sprite>controlsMenuSprite_;//操作説明メニュー
	std::unique_ptr<TuboEngine::Sprite>controlsDetailSprite_;//操作説明画面
	std::unique_ptr<TuboEngine::Sprite>selectguideSprite_;//選択キーガイド
	std::unique_ptr<TuboEngine::Sprite>enterguideSprite_;//決定キーガイド

    Vector2 pauseSize_ = { 400.0f,300.0f };//ポーズメニューのサイズ
	Vector2 retryBaseSize_ = { 192.0f,49.0f };//リトライボタンのサイズ
	Vector2 titleBaseSize_ = { 336.0f,49.0f };//タイトルへボタンのサイズ
	Vector2 selectBaseSize_ = { 336.0f,49.0f };//セレクトへボタンのサイズ
	Vector2 controlsMenuBaseSize_ = { 330.0f,40.0f };//操作説明メニューのサイズ
	Vector2 controlsDetailBaseSize_ = { 200.0f,200.0f };//操作説明画面のサイズ
	Vector2 selectguideBaseSize_ = { 330.0f,100.0f };//選択キーガイドのサイズ
	Vector2 enterguideBaseSize_ = { 330.0f,100.0f };//決定キーガイドのサイズ

	Vector2 center_ = { 640.0f,360.0f };//画面の中心座標
	Vector2 pausePos_ = { 640.0f,360.0f };   //ポーズメニューの位置
	Vector2 retryPos_ = { 640.0f,380.0f };//リトライボタンの座標
	Vector2 totitlePos_ = { 640.0f,510.0f };//タイトルへボタンの座標
	Vector2 toSelectPos_ = { 640.0f,520.0f };//セレクトへボタンの座標
	Vector2 controlsMenuPos_ = { 640.0f,540.0f };//操作説明メニューの座標
	Vector2 controlsDetailPos_ = { 640.0f,360.0f };//操作説明画面の座標
	Vector2 selectguidePos_ = { 462.0f,615.0f };//選択キーガイドの座標
	Vector2 enterguidePos_ = { 876.0f,620.0f };//決定キーガイドの座標

	int pauseSelectIndex_ = 0;//ポーズメニューの選択インデックス
	int rotateDir_ = 0;

	float pauseSelectAnimTimer_ = 0.0f;//ポーズメニューの選択アニメーションタイマー
	float pauseScale_ = 0.0f;//ポーズメニューのスケール
	float rotateTimer_ = 0.0f;

	bool isShowPause_ = false;//ポーズメニュー表示フラグ
	bool isRotating_ = false;

	const float pauseAnimSpeed_ = 0.05f;//ポーズメニューのアニメーション速度
	const float rotateSpeed_ = 0.08f;
};
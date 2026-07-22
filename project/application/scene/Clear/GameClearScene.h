#pragma once
#include "IScene.h"
#include "Camera.h"
#include "Object3d.h"              // 背景スカイボックス
#include "rubikCube/RubikCube.h"   // クリア演出の自動回転キューブ
#include <memory>
#include <string>

// クリア画面
// 「ゲームクリア！」表示＋金色の紙吹雪で演出し、Enter/A でタイトルへ戻る。
class GameClearScene : public IScene {
public:
	void Initialize() override;
	void Update() override;
	void Finalize() override;
	void Object3DDraw() override;
	void SpriteDraw() override;
	void ImGuiDraw() override;
	void ParticleDraw() override;
	TuboEngine::Camera* GetMainCamera() const override { return camera_.get(); }

private:
	// 決定入力（Enter/Space/Aボタン）のトリガー。押した瞬間だけ true。
	bool TakeDecideInput() const;

	std::unique_ptr<TuboEngine::Camera> camera_;
	std::unique_ptr<TuboEngine::Object3d> background_; // 背景スカイボックス

	// クリア演出: 画面中央のルービックキューブを、カメラが周回して見せる
	//（ターンテーブル演出。固定カメラのタイトル画面とは見た目が異なる）。
	std::unique_ptr<RubikCube> rubikCube_;
	int autoRotateTimer_ = 0; // 次の面回転までの残りフレーム（アクセント用に控えめ）
	float orbitYaw_ = 0.6f;   // カメラ周回角（毎フレーム増やしてぐるっと回す）
	float cubeSpin_ = 0.0f;   // キューブ全体の自転角（描画専用。カメラとは別に回す）

	std::string particleName_; // 紙吹雪エミッター（Finalize で片付ける用）
	float elapsed_ = 0.0f;     // 経過時間（入力受付を少し待つ／演出用）
};

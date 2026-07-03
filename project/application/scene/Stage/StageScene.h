#pragma once
#include "IScene.h"
#include "Camera.h"
#include "Object3d.h"   
#include  "Ui.h"

#include <memory>

// ゲーム本編（ステージ）
// ここに自分の処理を足していく（カメラだけ持った最小の雛形）。
class StageScene : public IScene {
public:
	void Initialize() override;
	void Update() override;
	void Finalize() override;
	void Object3DDraw() override;
	void SpriteDraw() override;
	void ImGuiDraw() override;
	void ParticleDraw() override;
	TuboEngine::Camera* GetMainCamera() const override { return camera_.get(); }
	//キューブの回転アニメーション
	void CubeAnimation();
	//ポーズメニューでのシーン切り替え
	void ChangeSceneFromPause();

private:
	std::unique_ptr<TuboEngine::Camera> camera_;
	std::unique_ptr<TuboEngine::Object3d>pauseMenuCube_;//ポーズメニューキューブ
	std::unique_ptr<Ui>ui_;//UIクラス

	const float PI = 3.1415926f;
	const float DEG90 = PI / 2.0f;

	float cubeBaseScale_ = 1.5f;
	float cubeScale_ = 0.0f;
	float basecubeAngle_ = 0.0f;
	bool prevRotating_ = false;
};
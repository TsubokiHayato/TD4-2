#pragma once
#include "Camera.h"
#include "IScene.h"
#include <memory>

#include "RubikCube/RubikCube.h"

// User1 の個人開発用シーン（サンドボックス）
// ここに自分の処理を足していく（カメラだけ持った最小の雛形）。
 class OosakiScene : public IScene {
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
	std::unique_ptr<TuboEngine::Camera> camera_;
	TuboEngine::Transform transform_;

	std::unique_ptr<RubikCube> rubikCube_ = nullptr;
};

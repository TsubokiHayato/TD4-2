#pragma once
#include "IScene.h"
#include "Camera.h"
#include <memory>

// 最小サンプルシーン：カメラ1つだけ持ち、画面クリア色を表示する。
// ここに自分のゲーム（オブジェクト・入力・UI…）を足していく出発点。
class SampleScene : public IScene {
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
};

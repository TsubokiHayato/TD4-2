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

private:
	std::unique_ptr<TuboEngine::Camera> camera_;
	std::unique_ptr<Ui>ui_;//UIクラス
};

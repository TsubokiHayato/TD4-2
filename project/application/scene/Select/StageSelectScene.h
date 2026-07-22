#pragma once
#include "Camera.h"
#include "DebugCamera.h" 
#include "IScene.h"
#include "Object3d.h"
#include <array>
#include <memory>

//ステージセレクトシーン
class StageSelectScene : public IScene {
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
	//左右キーで選択ブロックを切り替える
	void UpdateSelection();
	//スペースキーで決定
	void ConfirmSelection();
	//選択中のブロックアニメーション
	void UpdateBlockAppearance();
private:
	static constexpr int kGridSize = 3;
	static constexpr int kStageCount = kGridSize * kGridSize;
	static constexpr float kBlockSpacing = 1.2f;
	static constexpr float kBlockScale = 0.5f;

	std::unique_ptr<TuboEngine::Camera> camera_;
	std::unique_ptr<TuboEngine::DebugCamera> debugCamera_;
	// 9個のブロック(3x3グリッドに配置
	std::array<std::unique_ptr<TuboEngine::Object3d>, kStageCount> blocks_;

	int selectedX_ = 0; // 選択中の列
	int selectedY_ = 0; // 選択中の行
	// 2D座標を1D配列のインデックスに変換する
	static int ToIndex(int x, int y) { return y * kGridSize + x; }
};

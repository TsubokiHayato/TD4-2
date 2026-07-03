#pragma once
#include "IScene.h"
#include "Camera.h"
#include <memory>
#include <StageLoader.h>
#include <CubeMapConverter.h>
#include <Object3d.h>
#include <WallData.h>
#include <DebugCamera.h>

using namespace TuboEngine;
// Satou の個人開発用シーン（サンドボックス）
// ここに自分の処理を足していく（カメラだけ持った最小の雛形）。
class SatouScene : public IScene {
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
	void CreateWalls();

	void RebuildWalls(); // 壁の再構築（csvData_ の変更後に呼ぶ）

	void UpdateWallTransform(); // wallData_ の変更後に呼ぶ
private:
	std::unique_ptr<TuboEngine::Camera> camera_;
	std::unique_ptr<TuboEngine::DebugCamera> debugCamera_; // F2 で乗っ取るフリーカメラ


	std::vector<std::vector<int>> csvData_;
	std::vector<HoleData> holeData_;
	std::vector<WallData> wallData_;
	std::vector<std::unique_ptr<Object3d>> wallObjects_;

	static constexpr int kCubeSize = 3;
	static constexpr float kCellSize = 2.0f;

	float stageCellSize_ = 2.0f;
	float wallThickness_ = 0.2f; // 壁の厚み
	float cubeMargin_ = 5.0f;    // 中央からの距離
};

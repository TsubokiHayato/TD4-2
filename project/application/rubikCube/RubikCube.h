#pragma once
#include <cstdint>
#include <memory>
#include "BaseRubikCubeState.h"
#include "Object3d.h"

/// <summary>
/// ルービックキューブ
/// </summary>
class RubikCube {

public:
	/// <summary>
	/// 初期化処理
	/// </summary>
	void Initialize(TuboEngine::Camera* camera);
	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();
	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw();
	/// <summary>
	/// デバッグ(ImGui表示)
	/// </summary>
	void Debug();

private:

	SixCube sixCube_;

	//最大マス
	const uint32_t kMaxNum_ = 3;

	int row_ = 1;//回す列　今は真ん中

	std::unique_ptr<BaseRubikCubeState> rubikCubeState_;

	uint32_t rotateDirectionNum_ = 0;//回転する向き0～2

	std::vector<std::unique_ptr<TuboEngine::Object3d>> objects_;//キューブ本体

	std::vector<std::unique_ptr<TuboEngine::Object3d>> tips_;//先端たち


	TuboEngine::Camera* camera_;
};


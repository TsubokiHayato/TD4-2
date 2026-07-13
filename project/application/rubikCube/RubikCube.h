#pragma once
#include <cstdint>
#include <memory>
#include "BaseRubikCubeState.h"
#include "Object3d.h"

#define _USE_MATH_DEFINES
#include <math.h>

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

	/// <summary>
	/// 現在の先端配置を取得する(クリア判定用の読み取り専用アクセサ)。
	/// </summary>
	const SixCube& GetState() const { return sixCube_; }

	/// <summary>
	/// 先端配置を外部から設定する(CSV/エディターでの初期配置用)。
	/// </summary>
	void SetState(const SixCube& state) { sixCube_ = state; }

private:
	/// -+- 関数 -+-
	

	/// <summary>
	/// 回転軸を変更する
	/// </summary>
	void AxisChange();

	/// <summary>
	/// 先端の配置
	/// </summary>
	void PlaceTip();

	/// -+- end -+-
private:

	/// -+- 変数 -+-

	/// -- ルービックキューブ --
	SixCube sixCube_;

	//最大マス目
	const uint32_t kMaxNum_ = 3;

	int row_ = 1;//回転列　今は真ん中

	//ルービックキューブ回転ステート(X軸、Y軸、Z軸)
	std::unique_ptr<BaseRubikCubeState> rubikCubeState_;
	bool isRotationCube_ = true;

	uint32_t rotateDirectionNum_ = 0;//回転する向き0～2

	std::vector<std::unique_ptr<TuboEngine::Object3d>> cubeObjects_;//キューブ本体オブジェクト
	std::vector<std::unique_ptr<TuboEngine::Object3d>> tipObjects_;//先端たちオブジェクト
	const float kSize_ = 0.5f;//大きさ
	/// -- end --


	// 回転方向(1 or 0)
	int rotateDirection_ = 0;

	/// -- 選択回転軸 --
	std::unique_ptr<TuboEngine::Object3d> selectAxisObject_;
	TuboEngine::Math::Vector3 selectAxisPosition_;//座標位置
	TuboEngine::Math::Vector3 selectAxisRotate_;//回転
	/// -- end --


	/// -- 回転方向の矢印 --
	std::unique_ptr<TuboEngine::Object3d> rotateArrowObject_;
	TuboEngine::Math::Vector3 rotationArrow_{};//回転
	/// -- end --

	/// -- 回転角度 --
	const float kNinetyRadian_ = 90.0f * float(M_PI) / 180.0f;//90度
	const float kHundredEightyRadian_ = float(M_PI);//180度
	/// -- end --

	TuboEngine::Camera* camera_;

	/// -+- end -+-
};


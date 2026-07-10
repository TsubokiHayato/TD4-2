#pragma once
#include <cstdint>
#include <memory>
#include "BaseRubikCubeState.h"
#include "Object3d.h"

#define M_PI 3.14159265358979323846

/// <summary>
/// ルービックキューブ
/// </summary>
class RubikCube {

	enum class RotationAxis {
		X,
		Y,
		Z
	};

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

	struct Mat3 { float m[3][3]; };

	void StartRotationAnimation();
	
	void UpdateRotationAnimation();

	TuboEngine::Math::Vector3 RotateAroundAxis(const TuboEngine::Math::Vector3& pos, RotationAxis axis, float angle);

	Mat3 EulerToMat3(const TuboEngine::Math::Vector3& euler);

	TuboEngine::Math::Vector3 Mat3ToEuler(const Mat3& mat);

	Mat3 Mat3Mul(const Mat3& a, const Mat3& b);

	Mat3 AxisRotationMat3(RotationAxis axis, float angle);

	float GetRowSign(RotationAxis axis);

	float GetRotationSign(RotationAxis axis);

private:

	SixCube sixCube_;

	//最大マス
	const uint32_t kMaxNum_ = 3;

	int row_ = 1;//回す列　今は真ん中

	std::unique_ptr<BaseRubikCubeState> rubikCubeState_;

	uint32_t rotateDirectionNum_ = 0;//回転する向き0～2

	std::vector<std::unique_ptr<TuboEngine::Object3d>> objects_;//キューブ本体

	std::vector<std::unique_ptr<TuboEngine::Object3d>> tips_;//先端たち

	RotationAxis currentAxis_ = RotationAxis::X;//回転軸

	std::vector<TuboEngine::Object3d*>rotatingObjects_;//回転中のオブジェクト
	std::vector<TuboEngine::Math::Vector3>rotatingInitialPos_;//回転中のオブジェクトの初期位置
	std::vector<TuboEngine::Math::Vector3> rotatingInitialRot_;//回転中オブジェクトの初期回転

	//回転方向
	int rotation_ = 0;
	//回転軸
	float rotateAngle_ = 0.0f;
	//回転速度
	const float kRotateSpeedRad_ = (float(M_PI) / 2.0f) / 20.0f;
	//回転中かどうか
	bool isRotating_ = false;

	TuboEngine::Camera* camera_;
};


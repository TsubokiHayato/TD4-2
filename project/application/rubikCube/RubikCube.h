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

	enum class RotationAxis {//回転軸
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

	/// <summary>
	/// 現在の先端配置を取得する(クリア判定用の読み取り専用アクセサ)。
	/// </summary>
	const SixCube& GetState() const { return sixCube_; }

	/// <summary>
	/// 先端配置を外部から設定する(CSV/エディターでの初期配置用)。
	/// </summary>
	void SetState(const SixCube& state) { sixCube_ = state; }

private:
	/// <summary>
	// 3x3行列
	/// </summary>
	struct Mat3 { float m[3][3]; };
	/// <summary>
	/// 回転アニメーション開始
	/// </summary>
	void StartRotationAnimation();
	/// <summary>
	/// 回転アニメーション更新
	/// </summary>
	void UpdateRotationAnimation();
	/// <summary>
	/// 回転軸を中心に回転させる
	/// </summary>
	TuboEngine::Math::Vector3 RotateAroundAxis(const TuboEngine::Math::Vector3& pos, RotationAxis axis, float angle);
	/// <summary>
	/// オイラー角から3x3行列に変換
	/// </summary>
	Mat3 EulerToMat3(const TuboEngine::Math::Vector3& euler);
	/// <summary>
	/// 3x3行列からオイラー角に変換
	/// </summary>
	TuboEngine::Math::Vector3 Mat3ToEuler(const Mat3& mat);
	/// <summary>
	/// 3x3行列の掛け算
	/// </summary>
	Mat3 Mat3Mul(const Mat3& a, const Mat3& b);
	/// <summary>
	/// 回転軸と角度から3x3行列を作成
	/// </summary>
	Mat3 AxisRotationMat3(RotationAxis axis, float angle);
	/// <summary>
	/// 回転軸の符号を取得
	/// </summary>
	float GetRowSign(RotationAxis axis);
	/// <summary>
	/// 回転方向の符号を取得
	/// </summary>
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


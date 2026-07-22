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

	/// <summary>
	/// 外部(マウス操作など)から回転アニメーションを依頼する。
	/// アニメ中は無視して false を返す。開始できたら true。
	/// </summary>
	/// <param name="axis">0=X, 1=Y, 2=Z</param>
	/// <param name="row">回す列 0～2</param>
	/// <param name="dir">回す向き 0 or 1</param>
	bool RequestRotation(int axis, int row, int dir);

	void GuideRotationAxis(int axis[], int row[]);

	/// <summary>
	/// 回転アニメーション中か(入力ガード用)。
	/// </summary>
	bool IsRotating() const { return isRotating_; }

	/// <summary>
	/// キューブ全体を Y 軸まわりに回して「見せる」ための角度(ラジアン)を設定する。
	/// 描画時だけ適用する“見た目専用”の回転で、面回転判定に使う論理座標は変えない。
	/// 既定は 0（＝何もしない）なので、設定しない Title/Stage では従来どおり。
	/// </summary>
	void SetWholeSpinYaw(float angleRad) { wholeSpinYaw_ = angleRad; }

	/// <summary>
	/// 回転軸ガイド／方向矢印(操作補助UI)を表示するか。既定は true。
	/// タイトルやクリアなど「見せるだけ」の場面では false にして、
	/// ゲーム操作用のガイドがキューブに重ならないようにする。
	/// </summary>
	void SetGuideVisible(bool visible) { guideVisible_ = visible; }

private:

	/// ------
	///  関数
	/// ------

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

	/// ------
	///  変数
	/// ------

	/// -- ルービックキューブ --
	SixCube sixCube_;
	//最大マス
	const uint32_t kMaxNum_ = 3;

	int row_ = 1;//回す列　今は真ん中

	std::unique_ptr<BaseRubikCubeState> rubikCubeState_;

	uint32_t rotateDirectionNum_ = 0;//回転する向き0～2

	std::vector<std::unique_ptr<TuboEngine::Object3d>> objects_;//キューブ本体
	
	std::vector<std::unique_ptr<TuboEngine::Object3d>> tips_;//先端たち
	
	const float kSize_ = 0.5f;

	/// -- 選択回転軸 --
	std::unique_ptr<TuboEngine::Object3d> selectAxisObject_[2];
	TuboEngine::Math::Vector3 selectAxisPosition_[2];//座標位置
	TuboEngine::Math::Vector3 selectAxisRotate_[2];//回転
	/// -- end --

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

	// 描画時だけ適用する、キューブ全体の Y 軸回転角(ラジアン)。0 なら通常描画。
	float wholeSpinYaw_ = 0.0f;
	// 操作補助UI(選択軸ガイド・方向矢印)を描画するか。Title/Clear では false にする。
	bool guideVisible_ = true;
	/// -- 回転方向の矢印 --
	std::unique_ptr<TuboEngine::Object3d> rotateArrowObject_;
	TuboEngine::Math::Vector3 positionArrow_{};//座標位置
	TuboEngine::Math::Vector3 rotationArrow_{};//回転
	/// -- end --

	/// -- 回転角度 --
	const float kNinetyRadian_ = 90.0f * float(M_PI) / 180.0f;//90度
	const float kHundredEightyRadian_ = float(M_PI);//180度
	/// -- end --


	TuboEngine::Camera* camera_;
};


#pragma once
#include <cstdint>

#define _USE_MATH_DEFINES
#include <math.h>

#include "Vector3.h"
#include <memory>

struct OneCube {
	uint32_t cube[3][3];
};

struct SixCube {
	OneCube oneCube[6];
};

/// <summary>
/// 
/// </summary>
class BaseRubikCubeState {
public:

	/// <summary>
	/// 回転
	/// </summary>
	/// <param name="sixCube">ルービックキューブ六面</param>
	/// <param name="row">回転列</param>
	/// <param name="rotation">回す方向</param>
	virtual void Rotation(SixCube& sixCube, const uint32_t& row,int rotation) = 0;

	/// <summary>
	/// 更新処理(主に回転軸可視化の位置など)
	/// </summary>
	/// <param name="selectAxisPosition">回転列の場所</param>
	/// <param name="row">選択された回転列</param>
	/// <param name="rotationArrow">矢印の回転方向</param>
	virtual void SelectAxis(TuboEngine::Math::Vector3& selectAxisPosition, float row) = 0;

	virtual void RotationDirection(int rotation, TuboEngine::Math::Vector3& rotationArrow) = 0;


protected:

	/// <summary>
	/// 回転により面自体が回転
	/// </summary>
	void RotationAround(SixCube& sixCube, const uint32_t& row, int rotation, const uint32_t& aroundA, const uint32_t& aroundB);

	/// <summary>
	/// 左回り
	/// </summary>
	/// <param name="sixCube"></param>
	/// <param name="around"></param>
	void LeftRotate(SixCube& sixCube, const uint32_t& around);

	/// <summary>
	/// 右回り
	/// </summary>
	/// <param name="sixCube"></param>
	/// <param name="around"></param>
	void RightRotate(SixCube& sixCube, const uint32_t& around);

	struct RotationRow {
		int num[4];
	};

	RotationRow rotationRow_ = {};

	uint32_t matrixNum = -1;

	//開店前のキューブ1面
	SixCube prevSixCube_ = {};

	//回転角度 3度ずつ回転
	const float kRotationAngle_ = 3.0f * float(M_PI) / 180.0f;
};

/// <summary>
/// 
/// </summary>
class RotationXState : public BaseRubikCubeState {
public:
	/// <summary>
	/// 回転
	/// </summary>
	/// <param name="sixCube">ルービックキューブ六面</param>
	/// <param name="row">回転列</param>
	/// <param name="rotation">回す方向</param>
	void Rotation(SixCube& sixCube, const uint32_t& row, int rotation) override;

	/// <summary>
	/// 軸の選択
	/// </summary>
	/// <param name="selectAxisPosition">選択の可視化する位置</param>
	/// <param name="row">列</param>
	void SelectAxis(TuboEngine::Math::Vector3& selectAxisPosition, float row) override;

	/// <summary>
	/// 回転方向の可視化
	/// </summary>
	/// <param name="dir">回転方向</param>
	/// <param name="rotationArrow">矢印モデルの回転</param>
	void RotationDirection(int dir, TuboEngine::Math::Vector3& rotationArrow) override;

private:
	const uint32_t kLeftAround_ = 2 + matrixNum;
	const uint32_t kRightAround_ = 4 + matrixNum;
	//回転角度 180度
	const float kHundredEightyRadian_ = float(M_PI);
};

/// <summary>
/// 
/// </summary>
class RotationYState : public BaseRubikCubeState {
public:
	/// <summary>
	/// 回転
	/// </summary>
	/// <param name="sixCube">ルービックキューブ六面</param>
	/// <param name="row">回転列</param>
	/// <param name="rotation">回す方向</param>
	void Rotation(SixCube& sixCube, const uint32_t& row, int rotation) override;


	/// <summary>
	/// 軸の選択
	/// </summary>
	/// <param name="selectAxisPosition">選択の可視化する位置</param>
	/// <param name="row">列</param>
	void SelectAxis(TuboEngine::Math::Vector3& selectAxisPosition, float row) override;

	/// <summary>
	/// 回転方向の可視化
	/// </summary>
	/// <param name="dir">回転方向</param>
	/// <param name="rotationArrow">矢印モデルの回転</param>
	void RotationDirection(int dir, TuboEngine::Math::Vector3& rotationArrow) override;

private:
	const uint32_t kUpAround_ = 1 + matrixNum;
	const uint32_t kDownAround_ = 5 + matrixNum;
	//回転角度 90度
	const float kNinetyRadian_ = 90.0f * float(M_PI) / 180.0f;
};

/// <summary>
/// 
/// </summary>
class RotationZState : public BaseRubikCubeState {
public:
	/// <summary>
	/// 回転
	/// </summary>
	/// <param name="sixCube">ルービックキューブ六面</param>
	/// <param name="row">回転列</param>
	/// <param name="rotation">回す方向</param>
	void Rotation(SixCube& sixCube, const uint32_t& row, int rotation) override;

	/// <summary>
	/// 軸の選択
	/// </summary>
	/// <param name="selectAxisPosition">選択の可視化する位置</param>
	/// <param name="row">列</param>
	void SelectAxis(TuboEngine::Math::Vector3& selectAxisPosition, float row) override;

	/// <summary>
	/// 回転方向の可視化
	/// </summary>
	/// <param name="dir">回転方向</param>
	/// <param name="rotationArrow">矢印モデルの回転</param>
	void RotationDirection(int dir, TuboEngine::Math::Vector3& rotationArrow) override;

private:
	const uint32_t kNearAround_ = 3 + matrixNum;
	const uint32_t kFarAround_ = 6 + matrixNum;
	//回転角度 90度
	const float kNinetyRadian_ = 90.0f * float(M_PI) / 180.0f;
};
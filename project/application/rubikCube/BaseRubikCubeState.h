#pragma once
#include <cstdint>

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
	virtual void Rotation(SixCube& sixCube, const uint32_t& row) = 0;


protected:

	/// <summary>
	/// 回転により面自体が回転
	/// </summary>
	void RotationAround(SixCube& sixCube, const uint32_t& row, const uint32_t& aroundA, const uint32_t& aroundB);

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
};

/// <summary>
/// 
/// </summary>
class RotationXState : public BaseRubikCubeState {
public:
	/// <summary>
	/// 回転
	/// </summary>
	void Rotation(SixCube& sixCube, const uint32_t& row) override;

private:
	const uint32_t kLeftAround_ = 2 + matrixNum;
	const uint32_t kRightAround_ = 4 + matrixNum;
};

/// <summary>
/// 
/// </summary>
class RotationYState : public BaseRubikCubeState {
public:
	/// <summary>
	/// 回転
	/// </summary>
	void Rotation(SixCube& sixCube, const uint32_t& row) override;

private:
	const uint32_t kUpAround_ = 1 + matrixNum;
	const uint32_t kDownAround_ = 5 + matrixNum;
};

/// <summary>
/// 
/// </summary>
class RotationZState : public BaseRubikCubeState {
public:
	/// <summary>
	/// 回転
	/// </summary>
	void Rotation(SixCube& sixCube, const uint32_t& row) override;

private:
	const uint32_t kNearAround_ = 3 + matrixNum;
	const uint32_t kFarAround_ = 6 + matrixNum;
};


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
/// 回転軸作成ステート(基盤クラス)
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
	virtual void Update(TuboEngine::Math::Vector3& selectAxisPosition, float row, int rotation, TuboEngine::Math::Vector3& rotationArrow) = 0;

	/// <summary>
	/// 次の回転軸ステートの変更
	/// </summary>
	/// <returns>次の回転軸ステート</returns>
	virtual std::unique_ptr<BaseRubikCubeState> GetNextState() = 0;

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

	/// -- 選択回転軸 --
	TuboEngine::Math::Vector3 selectAxisPosition_;//座標位置
	TuboEngine::Math::Vector3 selectAxisRotate_;//回転
	/// -- end --

	/// -- 回転方向の矢印 --
	TuboEngine::Math::Vector3 rotationArrow_{};//回転
	float rotation_ = 0.0f;
	/// -- end --

	//回転角度 3度ずつ回転
	const float kRotationAngle_ = 3.0f * float(M_PI) / 180.0f;
};

/// <summary>
/// 回転軸Xステート
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
	/// 更新処理(主に回転軸可視化の位置など)
	/// </summary>
	/// <param name="selectAxisPosition">回転列の場所</param>
	/// <param name="row">選択された回転列</param>
	/// <param name="rotationArrow">矢印の回転方向</param>
	void Update(TuboEngine::Math::Vector3& selectAxisPosition, float row, int rotation, TuboEngine::Math::Vector3& rotationArrow) override;

	/// <summary>
	/// 次の回転軸ステートの変更
	/// x軸からy軸に
	/// </summary>
	/// <returns>回転 Y軸 ステート</returns>
	std::unique_ptr<BaseRubikCubeState> GetNextState() override;

private:
	//端の面
	const uint32_t kLeftAround_ = 2 + matrixNum;
	const uint32_t kRightAround_ = 4 + matrixNum;

	//回転角度 180度
	const float kHundredEightyRadian_ = float(M_PI);
};

/// <summary>
/// 回転軸Yステート
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
	/// 更新処理(主に回転軸可視化の位置など)
	/// </summary>
	/// <param name="selectAxisPosition">回転列の場所</param>
	/// <param name="row">選択された回転列</param>
	/// <param name="rotationArrow">矢印の回転方向</param>
	void Update(TuboEngine::Math::Vector3& selectAxisPosition, float row, int rotation, TuboEngine::Math::Vector3& rotationArrow) override;

	/// <summary>
	/// 次の回転軸ステートの変更
	/// y軸からz軸に
	/// </summary>
	/// <returns>回転 Z軸 ステート</returns>
	std::unique_ptr<BaseRubikCubeState> GetNextState() override;

private:
	//端の面
	const uint32_t kUpAround_ = 1 + matrixNum;
	const uint32_t kDownAround_ = 5 + matrixNum;

	//回転角度 90度
	const float kNinetyRadian_ = 90.0f * float(M_PI) / 180.0f;
};

/// <summary>
/// 回転軸Zステート
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
	/// 更新処理(主に回転軸可視化の位置など)
	/// </summary>
	/// <param name="selectAxisPosition">回転列の場所</param>
	/// <param name="row">選択された回転列</param>
	/// <param name="rotationArrow">矢印の回転方向</param>
	void Update(TuboEngine::Math::Vector3& selectAxisPosition, float row, int rotation, TuboEngine::Math::Vector3& rotationArrow) override;

	/// <summary>
	/// 次の回転軸ステートの変更
	/// z軸からx軸に
	/// </summary>
	/// <returns>回転 X軸 ステート</returns>
	std::unique_ptr<BaseRubikCubeState> GetNextState() override;

private:
	//端の面
	const uint32_t kNearAround_ = 3 + matrixNum;
	const uint32_t kFarAround_ = 6 + matrixNum;
	//回転角度 90度
	const float kNinetyRadian_ = 90.0f * float(M_PI) / 180.0f;
};

#include "BaseRubikCubeState.h"
#include "EngineCore/engine/math/Vector3.h"
#include "EngineCore/engine/Input/Input.h"
#include "EngineCore/engine/imgui/ImGuiManager.h"

void BaseRubikCubeState::RotationAround(SixCube& sixCube, const uint32_t& row, int rotation, const uint32_t& aroundA, const uint32_t& aroundB) {
	if (row != 0 && row != 2) return;//両端でないなら(真ん中以外)


	prevSixCube_ = sixCube;

	if (row == 0) {


		if (rotation == 0) {
			//6 + matrixNumは逆行列
			if (aroundA == 5) {
				RightRotate(sixCube, aroundA);
			}
			else {
				LeftRotate(sixCube, aroundA);
			}
		}
		else if (rotation == 1) {
			//6 + matrixNumは逆行列
			if (aroundA == 5) {
				LeftRotate(sixCube, aroundA);
			}
			else {
				RightRotate(sixCube, aroundA);
			}
		}
	}
	else if (row == 2) {
		if (rotation == 1) {
			LeftRotate(sixCube, aroundB);
		}
		else if (rotation == 0) {
			RightRotate(sixCube, aroundB);
		}
	}
}

void BaseRubikCubeState::LeftRotate(SixCube& sixCube, const uint32_t& around) {
	sixCube.oneCube[around].cube[0][0] = prevSixCube_.oneCube[around].cube[2][0];
	sixCube.oneCube[around].cube[0][1] = prevSixCube_.oneCube[around].cube[1][0];
	sixCube.oneCube[around].cube[0][2] = prevSixCube_.oneCube[around].cube[0][0];
	sixCube.oneCube[around].cube[1][0] = prevSixCube_.oneCube[around].cube[2][1];
	//[1][1]は中心のため外す
	sixCube.oneCube[around].cube[1][2] = prevSixCube_.oneCube[around].cube[0][1];
	sixCube.oneCube[around].cube[2][0] = prevSixCube_.oneCube[around].cube[2][2];
	sixCube.oneCube[around].cube[2][1] = prevSixCube_.oneCube[around].cube[1][2];
	sixCube.oneCube[around].cube[2][2] = prevSixCube_.oneCube[around].cube[0][2];
}

void BaseRubikCubeState::RightRotate(SixCube& sixCube, const uint32_t& around) {
	sixCube.oneCube[around].cube[0][0] = prevSixCube_.oneCube[around].cube[0][2];
	sixCube.oneCube[around].cube[0][1] = prevSixCube_.oneCube[around].cube[1][2];
	sixCube.oneCube[around].cube[0][2] = prevSixCube_.oneCube[around].cube[2][2];
	sixCube.oneCube[around].cube[1][0] = prevSixCube_.oneCube[around].cube[0][1];
	//[1][1]は中心のため外す
	sixCube.oneCube[around].cube[1][2] = prevSixCube_.oneCube[around].cube[2][1];
	sixCube.oneCube[around].cube[2][0] = prevSixCube_.oneCube[around].cube[0][0];
	sixCube.oneCube[around].cube[2][1] = prevSixCube_.oneCube[around].cube[1][0];
	sixCube.oneCube[around].cube[2][2] = prevSixCube_.oneCube[around].cube[2][0];
}

void RotationXState::Rotation(SixCube& sixCube, const uint32_t& row, int rotation) {
	rotationRow_.num[0] = 1 + matrixNum;
	rotationRow_.num[1] = 3 + matrixNum;
	rotationRow_.num[2] = 5 + matrixNum;
	rotationRow_.num[3] = 6 + matrixNum;

	int reverce = 0;

	if (row == 0) {
		reverce += 2;
	}
	else if (row == 2) {
		reverce -= 2;
	}
		
	TuboEngine::Math::Vector3 prevCube = { (float)sixCube.oneCube[rotationRow_.num[0]].cube[0][row],(float)sixCube.oneCube[rotationRow_.num[0]].cube[1][row],(float)sixCube.oneCube[rotationRow_.num[0]].cube[2][row] };

	if (rotation == 1) {		
		sixCube.oneCube[rotationRow_.num[0]].cube[0][row] = sixCube.oneCube[rotationRow_.num[1]].cube[0][row];
		sixCube.oneCube[rotationRow_.num[0]].cube[1][row] = sixCube.oneCube[rotationRow_.num[1]].cube[1][row];
		sixCube.oneCube[rotationRow_.num[0]].cube[2][row] = sixCube.oneCube[rotationRow_.num[1]].cube[2][row];

		sixCube.oneCube[rotationRow_.num[1]].cube[0][row] = sixCube.oneCube[rotationRow_.num[2]].cube[0][row];
		sixCube.oneCube[rotationRow_.num[1]].cube[1][row] = sixCube.oneCube[rotationRow_.num[2]].cube[1][row];
		sixCube.oneCube[rotationRow_.num[1]].cube[2][row] = sixCube.oneCube[rotationRow_.num[2]].cube[2][row];

		sixCube.oneCube[rotationRow_.num[2]].cube[0][row] = sixCube.oneCube[rotationRow_.num[3]].cube[0][row + reverce];
		sixCube.oneCube[rotationRow_.num[2]].cube[1][row] = sixCube.oneCube[rotationRow_.num[3]].cube[1][row + reverce];
		sixCube.oneCube[rotationRow_.num[2]].cube[2][row] = sixCube.oneCube[rotationRow_.num[3]].cube[2][row + reverce];

		sixCube.oneCube[rotationRow_.num[3]].cube[0][row + reverce] = (uint32_t)prevCube.x;
		sixCube.oneCube[rotationRow_.num[3]].cube[1][row + reverce] = (uint32_t)prevCube.y;
		sixCube.oneCube[rotationRow_.num[3]].cube[2][row + reverce] = (uint32_t)prevCube.z;

	}
	else {
		sixCube.oneCube[rotationRow_.num[0]].cube[0][row] = sixCube.oneCube[rotationRow_.num[3]].cube[0][row + reverce];
		sixCube.oneCube[rotationRow_.num[0]].cube[1][row] = sixCube.oneCube[rotationRow_.num[3]].cube[1][row + reverce];
		sixCube.oneCube[rotationRow_.num[0]].cube[2][row] = sixCube.oneCube[rotationRow_.num[3]].cube[2][row + reverce];

		sixCube.oneCube[rotationRow_.num[3]].cube[0][row + reverce] = sixCube.oneCube[rotationRow_.num[2]].cube[0][row];
		sixCube.oneCube[rotationRow_.num[3]].cube[1][row + reverce] = sixCube.oneCube[rotationRow_.num[2]].cube[1][row];
		sixCube.oneCube[rotationRow_.num[3]].cube[2][row + reverce] = sixCube.oneCube[rotationRow_.num[2]].cube[2][row];

		sixCube.oneCube[rotationRow_.num[2]].cube[0][row] = sixCube.oneCube[rotationRow_.num[1]].cube[0][row];
		sixCube.oneCube[rotationRow_.num[2]].cube[1][row] = sixCube.oneCube[rotationRow_.num[1]].cube[1][row];
		sixCube.oneCube[rotationRow_.num[2]].cube[2][row] = sixCube.oneCube[rotationRow_.num[1]].cube[2][row];

		sixCube.oneCube[rotationRow_.num[1]].cube[0][row] = (uint32_t)prevCube.x;
		sixCube.oneCube[rotationRow_.num[1]].cube[1][row] = (uint32_t)prevCube.y;
		sixCube.oneCube[rotationRow_.num[1]].cube[2][row] = (uint32_t)prevCube.z;
	}

	RotationAround(sixCube, row, rotation, kLeftAround_, kRightAround_);
}

void RotationXState::SelectAxis(TuboEngine::Math::Vector3& selectAxisPosition, float row) {
	//回転する列の変更
	selectAxisPosition.x = row - 1.0f;
}


void RotationXState::RotationDirection(int rotation, TuboEngine::Math::Vector3& rotationArrow) {
	//X軸回転
	if (rotation == 0) {
		rotationArrow.x -= kRotationAngle_;
		rotationArrow.y = 0.0f;
	}
	else {
		rotationArrow.x -= kRotationAngle_;
		rotationArrow.y = kHundredEightyRadian_;//180度回転
	}
	rotationArrow.z = 0.0f;//変更なし
}


void RotationYState::Rotation(SixCube& sixCube, const uint32_t& row, int rotation) {
	rotationRow_.num[0] = 2 + matrixNum;
	rotationRow_.num[1] = 3 + matrixNum;
	rotationRow_.num[2] = 4 + matrixNum;
	rotationRow_.num[3] = 6 + matrixNum;

	int reverce = 0;

	if (row == 0) {
		reverce += 2;
	}
	else if (row == 2) {
		reverce -= 2;
	}
	
	TuboEngine::Math::Vector3 prevCube = { (float)sixCube.oneCube[rotationRow_.num[0]].cube[row][0],(float)sixCube.oneCube[rotationRow_.num[0]].cube[row][1],(float)sixCube.oneCube[rotationRow_.num[0]].cube[row][2] };

	if (rotation == 1) {

		sixCube.oneCube[rotationRow_.num[0]].cube[row][0] = sixCube.oneCube[rotationRow_.num[3]].cube[row + reverce][0];
		sixCube.oneCube[rotationRow_.num[0]].cube[row][1] = sixCube.oneCube[rotationRow_.num[3]].cube[row + reverce][1];
		sixCube.oneCube[rotationRow_.num[0]].cube[row][2] = sixCube.oneCube[rotationRow_.num[3]].cube[row + reverce][2];

		sixCube.oneCube[rotationRow_.num[3]].cube[row + reverce][0] = sixCube.oneCube[rotationRow_.num[2]].cube[row][0];
		sixCube.oneCube[rotationRow_.num[3]].cube[row + reverce][1] = sixCube.oneCube[rotationRow_.num[2]].cube[row][1];
		sixCube.oneCube[rotationRow_.num[3]].cube[row + reverce][2] = sixCube.oneCube[rotationRow_.num[2]].cube[row][2];

		sixCube.oneCube[rotationRow_.num[2]].cube[row][0] = sixCube.oneCube[rotationRow_.num[1]].cube[row][0];
		sixCube.oneCube[rotationRow_.num[2]].cube[row][1] = sixCube.oneCube[rotationRow_.num[1]].cube[row][1];
		sixCube.oneCube[rotationRow_.num[2]].cube[row][2] = sixCube.oneCube[rotationRow_.num[1]].cube[row][2];

		sixCube.oneCube[rotationRow_.num[1]].cube[row][0] = (uint32_t)prevCube.x;
		sixCube.oneCube[rotationRow_.num[1]].cube[row][1] = (uint32_t)prevCube.y;
		sixCube.oneCube[rotationRow_.num[1]].cube[row][2] = (uint32_t)prevCube.z;

	}
	else {
		sixCube.oneCube[rotationRow_.num[0]].cube[row][0] = sixCube.oneCube[rotationRow_.num[1]].cube[row][0];
		sixCube.oneCube[rotationRow_.num[0]].cube[row][1] = sixCube.oneCube[rotationRow_.num[1]].cube[row][1];
		sixCube.oneCube[rotationRow_.num[0]].cube[row][2] = sixCube.oneCube[rotationRow_.num[1]].cube[row][2];

		sixCube.oneCube[rotationRow_.num[1]].cube[row][0] = sixCube.oneCube[rotationRow_.num[2]].cube[row][0];
		sixCube.oneCube[rotationRow_.num[1]].cube[row][1] = sixCube.oneCube[rotationRow_.num[2]].cube[row][1];
		sixCube.oneCube[rotationRow_.num[1]].cube[row][2] = sixCube.oneCube[rotationRow_.num[2]].cube[row][2];

		sixCube.oneCube[rotationRow_.num[2]].cube[row][0] = sixCube.oneCube[rotationRow_.num[3]].cube[row + reverce][0];
		sixCube.oneCube[rotationRow_.num[2]].cube[row][1] = sixCube.oneCube[rotationRow_.num[3]].cube[row + reverce][1];
		sixCube.oneCube[rotationRow_.num[2]].cube[row][2] = sixCube.oneCube[rotationRow_.num[3]].cube[row + reverce][2];

		sixCube.oneCube[rotationRow_.num[3]].cube[row + reverce][0] = (uint32_t)prevCube.x;
		sixCube.oneCube[rotationRow_.num[3]].cube[row + reverce][1] = (uint32_t)prevCube.y;
		sixCube.oneCube[rotationRow_.num[3]].cube[row + reverce][2] = (uint32_t)prevCube.z;

	}

	RotationAround(sixCube, row, rotation, kUpAround_, kDownAround_);
}

void RotationYState::SelectAxis(TuboEngine::Math::Vector3& selectAxisPosition, float row) {
	//回転する列の変更
	selectAxisPosition.y = -(row - 1.0f);
}


void RotationYState::RotationDirection(int rotation, TuboEngine::Math::Vector3& rotationArrow) {
	//Y軸回転
	if (rotation == 0) {
		rotationArrow.x -= kRotationAngle_;
		rotationArrow.z = -kNinetyRadian_;
	}
	else {
		rotationArrow.x -= kRotationAngle_;
		rotationArrow.z = kNinetyRadian_;
	}
	rotationArrow.y = 0.0f;//変更なし
}


void RotationZState::Rotation(SixCube& sixCube, const uint32_t& row, int rotation) {
	rotationRow_.num[0] = 1 + matrixNum;//[row][]
	rotationRow_.num[1] = 2 + matrixNum;//[][row]
	rotationRow_.num[2] = 5 + matrixNum;//[row][]
	rotationRow_.num[3] = 4 + matrixNum;//[][row]


	int reverce = 0;

	if (row == 0) {
		reverce += 2;
	}
	else if (row == 2) {
		reverce -= 2;
	}

	TuboEngine::Math::Vector3 prevCube = { (float)sixCube.oneCube[rotationRow_.num[0]].cube[row][0],(float)sixCube.oneCube[rotationRow_.num[0]].cube[row][1],(float)sixCube.oneCube[rotationRow_.num[0]].cube[row][2] };


	if (rotation == 1) {

		sixCube.oneCube[rotationRow_.num[0]].cube[row][0] = sixCube.oneCube[rotationRow_.num[1]].cube[2][row];
		sixCube.oneCube[rotationRow_.num[0]].cube[row][1] = sixCube.oneCube[rotationRow_.num[1]].cube[1][row];
		sixCube.oneCube[rotationRow_.num[0]].cube[row][2] = sixCube.oneCube[rotationRow_.num[1]].cube[0][row];

		sixCube.oneCube[rotationRow_.num[1]].cube[0][row] = sixCube.oneCube[rotationRow_.num[2]].cube[row + reverce][0];
		sixCube.oneCube[rotationRow_.num[1]].cube[1][row] = sixCube.oneCube[rotationRow_.num[2]].cube[row + reverce][1];
		sixCube.oneCube[rotationRow_.num[1]].cube[2][row] = sixCube.oneCube[rotationRow_.num[2]].cube[row + reverce][2];

		sixCube.oneCube[rotationRow_.num[2]].cube[row + reverce][0] = sixCube.oneCube[rotationRow_.num[3]].cube[2][row + reverce];
		sixCube.oneCube[rotationRow_.num[2]].cube[row + reverce][1] = sixCube.oneCube[rotationRow_.num[3]].cube[1][row + reverce];
		sixCube.oneCube[rotationRow_.num[2]].cube[row + reverce][2] = sixCube.oneCube[rotationRow_.num[3]].cube[0][row + reverce];

		sixCube.oneCube[rotationRow_.num[3]].cube[0][row + reverce] = (uint32_t)prevCube.x;
		sixCube.oneCube[rotationRow_.num[3]].cube[1][row + reverce] = (uint32_t)prevCube.y;
		sixCube.oneCube[rotationRow_.num[3]].cube[2][row + reverce] = (uint32_t)prevCube.z;

	}
	else {
		sixCube.oneCube[rotationRow_.num[0]].cube[row][0] = sixCube.oneCube[rotationRow_.num[3]].cube[0][row + reverce];
		sixCube.oneCube[rotationRow_.num[0]].cube[row][1] = sixCube.oneCube[rotationRow_.num[3]].cube[1][row + reverce];
		sixCube.oneCube[rotationRow_.num[0]].cube[row][2] = sixCube.oneCube[rotationRow_.num[3]].cube[2][row + reverce];

		sixCube.oneCube[rotationRow_.num[3]].cube[2][row + reverce] = sixCube.oneCube[rotationRow_.num[2]].cube[row + reverce][0];
		sixCube.oneCube[rotationRow_.num[3]].cube[1][row + reverce] = sixCube.oneCube[rotationRow_.num[2]].cube[row + reverce][1];
		sixCube.oneCube[rotationRow_.num[3]].cube[0][row + reverce] = sixCube.oneCube[rotationRow_.num[2]].cube[row + reverce][2];

		sixCube.oneCube[rotationRow_.num[2]].cube[row + reverce][0] = sixCube.oneCube[rotationRow_.num[1]].cube[0][row];
		sixCube.oneCube[rotationRow_.num[2]].cube[row + reverce][1] = sixCube.oneCube[rotationRow_.num[1]].cube[1][row];
		sixCube.oneCube[rotationRow_.num[2]].cube[row + reverce][2] = sixCube.oneCube[rotationRow_.num[1]].cube[2][row];

		sixCube.oneCube[rotationRow_.num[1]].cube[2][row] = (uint32_t)prevCube.x;
		sixCube.oneCube[rotationRow_.num[1]].cube[1][row] = (uint32_t)prevCube.y;
		sixCube.oneCube[rotationRow_.num[1]].cube[0][row] = (uint32_t)prevCube.z;
	}



	RotationAround(sixCube, row, rotation, kFarAround_, kNearAround_);
}

void RotationZState::SelectAxis(TuboEngine::Math::Vector3& selectAxisPosition, float row) {
	//回転する列の変更
	selectAxisPosition.z = -(row - 1.0f);
}

void RotationZState::RotationDirection(int rotation, TuboEngine::Math::Vector3& rotationArrow) {
	//Z軸回転
	if (rotation == 0) {
		rotationArrow.z += kRotationAngle_;
		rotationArrow.y = kNinetyRadian_;
	}
	else {
		rotationArrow.z -= kRotationAngle_;
		rotationArrow.y = -kNinetyRadian_;
	}
	rotationArrow.x = 0.0f;//変更なし
}

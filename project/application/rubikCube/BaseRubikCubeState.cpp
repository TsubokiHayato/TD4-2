#include "BaseRubikCubeState.h"
#include "EngineCore/engine/math/Vector3.h"
#include "EngineCore/engine/Input/Input.h"
#include "EngineCore/engine/imgui/ImGuiManager.h"

void BaseRubikCubeState::RotationAround(SixCube& sixCube, const uint32_t& row, const uint32_t& aroundA, const uint32_t& aroundB) {
	if (row != 0 && row != 2) return;//両端でないなら(真ん中以外)


	prevSixCube_ = sixCube;

	if (row == 0) {

		LeftRotate(sixCube,aroundA);
		//RightRotate(sixCube, aroundA);
	}
	else if(row == 2){

		//LeftRotate(sixCube, aroundB);
		RightRotate(sixCube, aroundB);
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

void RotationXState::Rotation(SixCube& sixCube, const uint32_t& row) {
	rotationRow_.num[0] = 1 + matrixNum;
	rotationRow_.num[1] = 3 + matrixNum;
	rotationRow_.num[2] = 5 + matrixNum;
	rotationRow_.num[3] = 6 + matrixNum;

	TuboEngine::Math::Vector3 prevCube = { (float)sixCube.oneCube[rotationRow_.num[0]].cube[0][row],(float)sixCube.oneCube[rotationRow_.num[0]].cube[1][row],(float)sixCube.oneCube[rotationRow_.num[0]].cube[2][row] };

	for (int i = 0; i < 4 - 1; i++) {
		sixCube.oneCube[rotationRow_.num[i]].cube[0][row] = sixCube.oneCube[rotationRow_.num[i + 1]].cube[0][row];
		sixCube.oneCube[rotationRow_.num[i]].cube[1][row] = sixCube.oneCube[rotationRow_.num[i + 1]].cube[1][row];
		sixCube.oneCube[rotationRow_.num[i]].cube[2][row] = sixCube.oneCube[rotationRow_.num[i + 1]].cube[2][row];
	}

	sixCube.oneCube[rotationRow_.num[3]].cube[0][row] = (uint32_t)prevCube.x;
	sixCube.oneCube[rotationRow_.num[3]].cube[1][row] = (uint32_t)prevCube.y;
	sixCube.oneCube[rotationRow_.num[3]].cube[2][row] = (uint32_t)prevCube.z;


	RotationAround(sixCube,row,kLeftAround_, kRightAround_);

}


void RotationYState::Rotation(SixCube& sixCube, const uint32_t& row) {
	rotationRow_.num[0] = 2 + matrixNum;
	rotationRow_.num[1] = 3 + matrixNum;
	rotationRow_.num[2] = 4 + matrixNum;
	rotationRow_.num[3] = 6 + matrixNum;


	TuboEngine::Math::Vector3 prevCube = { (float)sixCube.oneCube[rotationRow_.num[0]].cube[row][0],(float)sixCube.oneCube[rotationRow_.num[0]].cube[row][1],(float)sixCube.oneCube[rotationRow_.num[0]].cube[row][2] };

	for (int i = 0; i < 4 - 1; i++) {
		sixCube.oneCube[rotationRow_.num[i]].cube[row][0] = sixCube.oneCube[rotationRow_.num[i + 1]].cube[row][0];
		sixCube.oneCube[rotationRow_.num[i]].cube[row][1] = sixCube.oneCube[rotationRow_.num[i + 1]].cube[row][1];
		sixCube.oneCube[rotationRow_.num[i]].cube[row][2] = sixCube.oneCube[rotationRow_.num[i + 1]].cube[row][2];
	}

	sixCube.oneCube[rotationRow_.num[3]].cube[row][0] = (uint32_t)prevCube.x;
	sixCube.oneCube[rotationRow_.num[3]].cube[row][1] = (uint32_t)prevCube.y;
	sixCube.oneCube[rotationRow_.num[3]].cube[row][2] = (uint32_t)prevCube.z;



	RotationAround(sixCube, row, kDownAround_, kUpAround_);
}


void RotationZState::Rotation(SixCube& sixCube, const uint32_t& row) {
	rotationRow_.num[0] = 1 + matrixNum;
	rotationRow_.num[1] = 2 + matrixNum;
	rotationRow_.num[2] = 5 + matrixNum;
	rotationRow_.num[3] = 4 + matrixNum;


	TuboEngine::Math::Vector3 prevCube = { (float)sixCube.oneCube[rotationRow_.num[0]].cube[0][row],(float)sixCube.oneCube[rotationRow_.num[0]].cube[1][row],(float)sixCube.oneCube[rotationRow_.num[0]].cube[2][row] };

	for (int i = 0; i < 4 - 1; i++) {
		sixCube.oneCube[rotationRow_.num[i]].cube[0][row] = sixCube.oneCube[rotationRow_.num[i + 1]].cube[0][row];
		sixCube.oneCube[rotationRow_.num[i]].cube[1][row] = sixCube.oneCube[rotationRow_.num[i + 1]].cube[1][row];
		sixCube.oneCube[rotationRow_.num[i]].cube[2][row] = sixCube.oneCube[rotationRow_.num[i + 1]].cube[2][row];
	}

	sixCube.oneCube[rotationRow_.num[3]].cube[0][row] = (uint32_t)prevCube.x;
	sixCube.oneCube[rotationRow_.num[3]].cube[1][row] = (uint32_t)prevCube.y;
	sixCube.oneCube[rotationRow_.num[3]].cube[2][row] = (uint32_t)prevCube.z;



	RotationAround(sixCube, row, kNearAround_, kFarAround_);
}

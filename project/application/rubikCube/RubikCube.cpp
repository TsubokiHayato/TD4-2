#include "RubikCube.h"
#include "EngineCore/engine/math/Vector3.h"
#include "EngineCore/engine/Input/Input.h"
#include "EngineCore/engine/imgui/ImGuiManager.h"

void RubikCube::Initialize() {

	rubikCubeState_ = std::make_unique<RotationXState>();


	//マス目の設定
	for (int i = 0; i < 6; i++) {
		sixCube_.oneCube[i].cube[0][0] = 0;
		sixCube_.oneCube[i].cube[0][1] = 0;
		sixCube_.oneCube[i].cube[0][2] = 0;
		sixCube_.oneCube[i].cube[1][0] = 0;
		sixCube_.oneCube[i].cube[1][1] = 0;
		sixCube_.oneCube[i].cube[1][2] = 0;
		sixCube_.oneCube[i].cube[2][0] = 0;
		sixCube_.oneCube[i].cube[2][1] = 0;
		sixCube_.oneCube[i].cube[2][2] = 0;
	}

	sixCube_.oneCube[0].cube[0][1] = 1;
	sixCube_.oneCube[1].cube[0][1] = 1;
	sixCube_.oneCube[2].cube[0][1] = 1;
	sixCube_.oneCube[3].cube[0][1] = 1;
	sixCube_.oneCube[4].cube[0][1] = 1;
	sixCube_.oneCube[5].cube[0][0] = 1;

	///イメージ
	///     上
	/// 
	///      1
	/// 右 2 3 4 6 左
	///      5
	///      6
	/// 
	/// x 1 3 5 6
	/// y 2 3 4 6
	/// z 1 2 5 4
	///     下
	/// 6は逆行列にするかも
}

void RubikCube::Update() {

	//向きを変更するのは全9方向

	if (TuboEngine::Input::GetInstance()->TriggerKey(DIK_E)) {
		rubikCubeState_.reset();

		if (rotateDirectionNum_ == 0) {
			rubikCubeState_ = std::make_unique<RotationYState>();
		}
		else if (rotateDirectionNum_ == 1) {
			rubikCubeState_ = std::make_unique<RotationZState>();
		}
		else if (rotateDirectionNum_ == 2) {
			rubikCubeState_ = std::make_unique<RotationXState>();
		}
		rotateDirectionNum_++;
		if (rotateDirectionNum_ > 2) {
			rotateDirectionNum_ = 0;
		}
	}

	if (TuboEngine::Input::GetInstance()->TriggerKey(DIK_SPACE)) {
		rubikCubeState_->Rotation(sixCube_, row_);
	}

	if (TuboEngine::Input::GetInstance()->TriggerKey(DIK_A)) {
		row_++;
		if (row_ > 2) {
			row_ = 0;
		}
	}
	else if (TuboEngine::Input::GetInstance()->TriggerKey(DIK_D)) {
		row_--;
		if (row_ < 0) {
			row_ = 2;
		}
	}
}

void RubikCube::Draw() {

}

void RubikCube::Debug() {

	ImGui::Begin("Rubik Cube");

	ImGui::Text("Row: %d", row_);
	ImGui::Text("RotateDirectionNum: %d", rotateDirectionNum_);

	ImGui::Separator();

	for (int masume = 0; masume < 6; masume++) {
		ImGui::Text("Cube: %d", masume + 1);
		for (int tate = 0; tate < 3; tate++) {
			ImGui::Text("%d %d %d", sixCube_.oneCube[masume].cube[tate][0], sixCube_.oneCube[masume].cube[tate][1], sixCube_.oneCube[masume].cube[tate][2]);
		}
		ImGui::Separator();
	}
	ImGui::End();

}



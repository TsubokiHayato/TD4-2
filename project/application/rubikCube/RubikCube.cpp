#include "RubikCube.h"
#include "EngineCore/engine/math/Vector3.h"
#include "EngineCore/engine/Input/Input.h"
#include "EngineCore/engine/imgui/ImGuiManager.h"

#define _USE_MATH_DEFINES
#include <math.h>

void RubikCube::Initialize(TuboEngine::Camera* camera) {

	rubikCubeState_ = std::make_unique<RotationXState>();

	camera_ = camera;

	for (int x = -1; x <= 1; x++) {
		for (int y = -1; y <= 1; y++) {
			for (int z = -1; z <= 1; z++) {
				std::unique_ptr<TuboEngine::Object3d> object = std::make_unique<TuboEngine::Object3d>();
				object->Initialize("OneCube/OneCube.obj");
				object->SetCamera(camera);
				object->SetPosition({ float(x),float(y),float(z) });
				object->SetScale({ 0.5f,0.5f,0.5f });
				objects_.push_back(std::move(object));
			}
		}
	}

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

	sixCube_.oneCube[0].cube[1][1] = 1;


	sixCube_.oneCube[1].cube[0][0] = 1;
	sixCube_.oneCube[1].cube[0][1] = 1;

	sixCube_.oneCube[1].cube[2][1] = 1;
	sixCube_.oneCube[1].cube[2][2] = 1;



	///イメージ
	///     上
	/// 
	///      1
	/// 右 2 3 4 6 左
	///      5
	///      6
	/// 
	///     下
	/// x 1 3 5 6
	/// y 2 3 4 6
	/// z 1 2 5 4
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




	tips_.clear();
	//マス目の設定
	for (int i = 0; i < 6; i++) {
			for (int y = -1; y <= 1; y++) {
				for (int x = -1; x <= 1; x++) {
				if (sixCube_.oneCube[i].cube[1 + y][1 + x] >= 1) {
					std::unique_ptr<TuboEngine::Object3d> object = std::make_unique<TuboEngine::Object3d>();
					object->Initialize("tip/tip.obj");
					object->SetCamera(camera_);

					if (i == 0) {
						object->SetPosition({ float(x),1,float(y) });
						object->SetRotation({ 0 ,0, 0 });//上
					}
					else if (i == 1) {
						object->SetPosition({ float(x),float(y),-1 });
						object->SetRotation({ -90.0f * (float(M_PI) / 180.0f),0,0 });//右
					}
					else if (i == 2) {
						object->SetPosition({ 1,float(y),float(x) });
						object->SetRotation({ 0,0,-90.0f * (float(M_PI) / 180.0f) });//手前
					}
					else if (i == 3) {
						object->SetPosition({ float(x),float(y),1 });
						object->SetRotation({ 90.0f * (float(M_PI) / 180.0f),0,0 });//左
					}
					else if (i == 4) {
						object->SetPosition({ float(x),-1,float(y) });
						object->SetRotation({ 180.0f  * (float(M_PI) / 180.0f),0,0 });//下
					}
					else if (i == 5) {
						object->SetPosition({ -1,float(y),float(x) });
						object->SetRotation({ 0,0,90.0f * (float(M_PI) / 180.0f) });//奥
					}

					object->SetScale({ 0.5f,0.5f,0.5f });
					tips_.push_back(std::move(object));
				}
			}
		}
	}








	for (auto& object : objects_) {
		object->Update();
	}

	for (auto& tip : tips_) {
		tip->Update();
	}
}

void RubikCube::Draw() {
	for (auto& object : objects_) {
		object->Draw();
	}
	for (auto& tip : tips_) {
		tip->Draw();
	}
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



#include "RubikCube.h"
#include "EngineCore/engine/math/Vector3.h"
#include "EngineCore/engine/Input/Input.h"
#include "EngineCore/engine/imgui/ImGuiManager.h"


void RubikCube::Initialize(TuboEngine::Camera* camera) {
	//回転軸ステート初期化
	rubikCubeState_ = std::make_unique<RotationXState>();
	camera_ = camera;

	//ルービックキューブ本体初期化
	for (int x = -1; x <= 1; x++) {
		for (int y = -1; y <= 1; y++) {
			for (int z = -1; z <= 1; z++) {
				if (x == 0 && y == 0 && z == 0) continue;//真ん中は見えないからいらない
				std::unique_ptr<TuboEngine::Object3d> object = std::make_unique<TuboEngine::Object3d>();
				object->Initialize("OneCube/OneCube.obj");
				object->SetCamera(camera);
				object->SetPosition({ float(x),float(y),float(z) });
				object->SetScale({ kSize_,kSize_,kSize_ });
				cubeObjects_.push_back(std::move(object));
			}
		}
	}

	//面のマス目の設定
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

	///イメージ
	///     上
	/// 
	///      1
	/// 右 2 3 4 左
	///      5
	///      6
	/// 
	///     下
	/// x軸回転 1 3 5 6
	/// y軸回転 2 3 4 6
	/// z軸回転 1 2 5 4
	/// 6は逆行列にするかも
	
	//選択した回転列
	selectAxisObject_ = std::make_unique<TuboEngine::Object3d>();
	selectAxisObject_->Initialize("SelectAxis/SelectAxis.obj");
	selectAxisObject_->SetCamera(camera);
	selectAxisObject_->SetPosition({ 0,0,0 });
	selectAxisObject_->SetScale({ kSize_,kSize_,kSize_ });

	//回転方向の矢印
	rotateArrowObject_ = std::make_unique<TuboEngine::Object3d>();
	rotateArrowObject_->Initialize("RotateArrow/RotateArrow.obj");
	rotateArrowObject_->SetCamera(camera);
	rotateArrowObject_->SetPosition({ 0,0,0 });
	rotateArrowObject_->SetScale({ kSize_,kSize_,kSize_ });

	//先端の配置(初期化)
	PlaceTip();
}

void RubikCube::Update() {

	//向きを変更するのは全9方向

	//回転方向の変更
	if (TuboEngine::Input::GetInstance()->TriggerKey(DIK_E)) {
		rotateDirection_ = 1;
		rotationArrow_.y = kHundredEightyRadian_;
	}
	else if (TuboEngine::Input::GetInstance()->TriggerKey(DIK_Q)) {
   		rotateDirection_ = 0;
		rotationArrow_.y = 0.0f;
	}

	//回転軸の変更
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


	//回転軸の変更
	AxisChange();

	if (rotateDirectionNum_ == 0) {
		SelectAxisSetting({ (float)row_ - 1.0f,0.0f,0.0f }, { -kRotationAngle_ ,0.0f,0.0f });
	}
	else if (rotateDirectionNum_ == 1) {
		SelectAxisSetting({ 0.0f,-((float)row_ - 1.0f),0.0f }, { kRotationAngle_ ,0.0f,0.0f });
		rotationArrow_.z = kNinetyRadian_;
	}
	else if (rotateDirectionNum_ == 2) {
		SelectAxisSetting({ 0,0,-((float)row_ - 1.0f) }, { 0.0f ,0.0f, kRotationAngle_ });
	}


	//回転の開始
	if (TuboEngine::Input::GetInstance()->TriggerKey(DIK_SPACE)) {
		rubikCubeState_->Rotation(sixCube_, row_,rotateDirection_);
		isRotationCube_ = true;//回転したフラグ
	}

	//先端の配置
	PlaceTip();

	///オブジェクト更新処理
	for (auto& object : cubeObjects_) {
		object->Update();
	}
	for (auto& tip : tipObjects_) {
		tip->Update();
	}
	selectAxisObject_->Update();
	rotateArrowObject_->Update();
	///オブジェクト更新終了
}

void RubikCube::Draw() {
	for (auto& object : cubeObjects_) {
		object->Draw();
	}
	for (auto& tip : tipObjects_) {
		tip->Draw();
	}

	selectAxisObject_->Draw();
	rotateArrowObject_->Draw();
}

void RubikCube::Debug() {

	ImGui::Begin("Rubik Cube");

	ImGui::Text("Row: %d", row_);
	ImGui::Text("RotateDirectionNum: %d", rotateDirectionNum_);

	ImGui::Separator();

	for (int masume = 0; masume < 6; masume++) {
		ImGui::Text("Cube: %d", masume);
		for (int tate = 0; tate < 3; tate++) {
			ImGui::Text("%d %d %d", sixCube_.oneCube[masume].cube[tate][0], sixCube_.oneCube[masume].cube[tate][1], sixCube_.oneCube[masume].cube[tate][2]);
		}
		ImGui::Separator();
	}
	ImGui::End();

}

void RubikCube::AxisChange() {
	//軸回転の変更　x y z
	if (TuboEngine::Input::GetInstance()->TriggerKey(DIK_R)) {
		rubikCubeState_.reset();//リセット

		// 選択軸回転表示 / 矢印の位置リセット
		selectAxisRotate_ = { 0,0,0 };
		rotationArrow_ = { 0,0,0 };

		// 順番 x軸(0) → y軸(1) → z軸(2) → x軸(0) ...
		if (rotateDirectionNum_ == 0) {
			rubikCubeState_ = std::make_unique<RotationYState>();//Y軸回転モード
			selectAxisRotate_.z = kNinetyRadian_;// 軸回転
		}
		else if (rotateDirectionNum_ == 1) {
			rubikCubeState_ = std::make_unique<RotationZState>();//Z軸回転モード
			selectAxisRotate_.y = kNinetyRadian_;// 軸回転
		}
		else if (rotateDirectionNum_ == 2) {
			rubikCubeState_ = std::make_unique<RotationXState>();//X軸回転モード
			selectAxisRotate_.x = kNinetyRadian_;// 軸回転
		}

		rotateDirectionNum_++;//加算
		if (rotateDirectionNum_ > 2) rotateDirectionNum_ = 0;//0にリセット
	}
}

void RubikCube::PlaceTip() {
	//回転していない(配置が変っていない)なら通らない
	if (!isRotationCube_) return;

	//一度リセット
	tipObjects_.clear();
	//マス目の設定
	for (int i = 0; i < 6; i++) {
		for (int y = -1; y <= 1; y++) {
			for (int x = -1; x <= 1; x++) {
				uint32_t shape = sixCube_.oneCube[i].cube[1 + y][1 + x];
				if (shape >= 1) {
					std::unique_ptr<TuboEngine::Object3d> object = std::make_unique<TuboEngine::Object3d>();
					// 形状ID(1=Cone / 2=Square)に応じて先端モデルを切り替える
					if (shape == 1) {
						object->Initialize("tip/tip.obj");
					}
					else if (shape == 2) {
						object->Initialize("square/square.obj");
					}
					else {
						object->Initialize("tip/tip.obj");
					}
					object->SetCamera(camera_);

					if (i == 0) {
						object->SetPosition({ float(x),1,-float(y) });
						object->SetRotation({ 0 ,0, 0 });//上
					}
					else if (i == 1) {
						object->SetPosition({ -1,-float(y),-float(x) });
						object->SetRotation({ 0,0,kNinetyRadian_ });//右
					}
					else if (i == 2) {
						object->SetPosition({ float(x),-float(y),-1 });
						object->SetRotation({ -kNinetyRadian_,0,0 });//手前
					}
					else if (i == 3) {
						object->SetPosition({ 1,-float(y),float(x) });
						object->SetRotation({ 0,0,-kNinetyRadian_ });//左
					}
					else if (i == 4) {
						object->SetPosition({ float(x),-1,float(y) });
						object->SetRotation({ 0,0,kHundredEightyRadian_ });//下
					}
					else if (i == 5) {
						object->SetPosition({ -float(x),float(y),1 });
						object->SetRotation({ kNinetyRadian_,0,0 });//奥
					}
					object->SetScale({ kSize_ ,kSize_ ,kSize_ });
					tipObjects_.push_back(std::move(object));
				}
			}
		}
		//変更終了
		isRotationCube_ = false;
	}
}

void RubikCube::SelectAxisSetting(const TuboEngine::Math::Vector3& rowPoint, const TuboEngine::Math::Vector3& addRotationArrow) {
	selectAxisPosition_ = rowPoint;
	rotationArrow_ += addRotationArrow;

	//回転軸を表すマークの
	selectAxisObject_->SetPosition(selectAxisPosition_);
	selectAxisObject_->SetRotation(selectAxisRotate_);

	//回転方向を表す矢印を回転する
	rotateArrowObject_->SetPosition(selectAxisPosition_);
	rotateArrowObject_->SetRotation(rotationArrow_);
}



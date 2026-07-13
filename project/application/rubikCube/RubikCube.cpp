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

	if (isRotating_) {
		UpdateRotationAnimation();

	//回転方向の変更
	if (TuboEngine::Input::GetInstance()->TriggerKey(DIK_E)) {
		rotateDirection_ = 1;
	}
	else if (TuboEngine::Input::GetInstance()->TriggerKey(DIK_Q)) {
   		rotateDirection_ = 0;
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

		for (auto& object : objects_)object->Update();
		for (auto& tip : tips_)tip->Update();
		return;

	}

	// キーボードによる回転操作は廃止(操作はマウス側 StageScene::MouseCubeControl で行う)。
	// 回転は外部から RequestRotation(axis,row,dir) で依頼される。

	//回転軸の変更
	AxisChange();

	rubikCubeState_->Update(selectAxisPosition_, (float)row_, rotateDirection_, rotationArrow_);

	//回転軸を表すマークの
	selectAxisObject_->SetPosition(selectAxisPosition_);
	selectAxisObject_->SetRotation(selectAxisRotate_);

	//回転方向を表す矢印を回転する
	rotateArrowObject_->SetPosition(selectAxisPosition_);
	rotateArrowObject_->SetRotation(rotationArrow_);


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
		// 選択軸回転表示 / 矢印の位置リセット
		selectAxisRotate_ = { 0,0,0 };
		rotationArrow_ = { 0,0,0 };

		// 順番 x軸(0) → y軸(1) → z軸(2) → x軸(0) ...
		std::unique_ptr<BaseRubikCubeState> newState = rubikCubeState_->GetNextState();
		rubikCubeState_.reset();//リセット

		rubikCubeState_ = std::move(newState);

		
		if (rotateDirectionNum_ == 0) {
			selectAxisRotate_.z = kNinetyRadian_;// 軸回転
		}
		else if (rotateDirectionNum_ == 1) {
			selectAxisRotate_.y = kNinetyRadian_;// 軸回転
		}
		else if (rotateDirectionNum_ == 2) {
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

	const char* axisNames[] = { "X", "Y", "Z" };
	ImGui::Text("CurrentAxis: %s", axisNames[static_cast<int>(currentAxis_)]);
	ImGui::Text("Row (0-2): %d", row_);
	ImGui::Text("Rotation: %d", rotation_);
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
//回転アニメーション開始
bool RubikCube::RequestRotation(int axis, int row, int dir) {
	if (isRotating_) return false;

	// 回転軸と、アニメ終了時の論理更新に使う状態クラスを合わせて設定する
	rubikCubeState_.reset();
	if (axis == 0) {
		currentAxis_ = RotationAxis::X;
		rubikCubeState_ = std::make_unique<RotationXState>();
	}
	else if (axis == 1) {
		currentAxis_ = RotationAxis::Y;
		rubikCubeState_ = std::make_unique<RotationYState>();
	}
	else {
		currentAxis_ = RotationAxis::Z;
		rubikCubeState_ = std::make_unique<RotationZState>();
	}
	row_ = row;
	rotation_ = dir; // 反転は呼び出し側(StageScene)で必要に応じて行う
	StartRotationAnimation();
	return true;
}

void RubikCube::StartRotationAnimation() {

	rotatingObjects_.clear();
	rotatingInitialPos_.clear();

	//rowをワールド座標に変換
	float targetCoord = (float(row_) - 1.0f) * GetRowSign(currentAxis_);
	//回転対象のオブジェクトを収集
	auto collectTarget = [&](TuboEngine::Object3d* object) {
		TuboEngine::Math::Vector3 pos = object->GetPosition();

		float coord = 0.0f;
		if (currentAxis_ == RotationAxis::X) coord = pos.x;
		else if (currentAxis_ == RotationAxis::Y) coord = pos.y;
		else coord = pos.z;
		//回転対象のオブジェクトを収集
		if (std::abs(coord - targetCoord) < 0.5f) {
			rotatingObjects_.push_back(object);
			rotatingInitialPos_.push_back(pos);
			rotatingInitialRot_.push_back(object->GetRotation());
		}
		};
	//回転対象のオブジェクトを収集
	for (auto& object : objects_) {
		collectTarget(object.get());
	}
	//回転対象のオブジェクトを収集
	for (auto& tip : tips_) {
		collectTarget(tip.get());
	}

	isRotating_ = true;
	rotateAngle_ = 0.0f;
}
//回転アニメーション更新
void RubikCube::UpdateRotationAnimation() {

	float baseSign = (rotation_ == 1) ? 1.0f : -1.0f;
	float sign = baseSign * GetRotationSign(currentAxis_);
	rotateAngle_ += kRotateSpeedRad_;

	float angle = sign * rotateAngle_;
	//回転対象のオブジェクトを回転
	for (size_t i = 0; i < rotatingObjects_.size(); i++) {
		TuboEngine::Math::Vector3 newPos = RotateAroundAxis(rotatingInitialPos_[i], currentAxis_, angle);
		rotatingObjects_[i]->SetPosition(newPos);

		//回転も初期値から毎フレーム合成
		Mat3 initialMat = EulerToMat3(rotatingInitialRot_[i]);
		Mat3 axisMat = AxisRotationMat3(currentAxis_, angle);
		Mat3 newMat = Mat3Mul(axisMat, initialMat);
		rotatingObjects_[i]->SetRotation(Mat3ToEuler(newMat));
	}
	//回転が90度になったら状態を更新してアニメーションを終了
	if (rotateAngle_ >= float(M_PI) / 2.0f) {
		rubikCubeState_->Rotation(sixCube_, row_, rotation_);

		//位置・回転をスナップ
		for (auto* obj : rotatingObjects_) {
			TuboEngine::Math::Vector3 p = obj->GetPosition();
			p.x = std::round(p.x); p.y = std::round(p.y); p.z = std::round(p.z);
			obj->SetPosition(p);
		}

		isRotating_ = false;
		rotateAngle_ = 0.0f;
		rotatingObjects_.clear();
		rotatingInitialPos_.clear();
		rotatingInitialRot_.clear();
	}
}
//回転軸を中心に回転させる
TuboEngine::Math::Vector3 RubikCube::RotateAroundAxis(const TuboEngine::Math::Vector3& pos, RotationAxis axis, float angle) {
	float c = cosf(angle);
	float s = sinf(angle);
	TuboEngine::Math::Vector3 result = pos;

	switch (axis) {
	case RotationAxis::X:
		result.y = pos.y * c - pos.z * s;
		result.z = pos.y * s + pos.z * c;
		break;
	case RotationAxis::Y:
		result.x = pos.x * c + pos.z * s;
		result.z = -pos.x * s + pos.z * c;
		break;
	case RotationAxis::Z:
		result.x = pos.x * c - pos.y * s;
		result.y = pos.x * s + pos.y * c;
		break;
	}
	return result;
}
//回転軸と角度から3x3行列を作成
RubikCube::Mat3 RubikCube::AxisRotationMat3(RotationAxis axis, float angle) {
	float c = cosf(angle), s = sinf(angle);
	Mat3 r = {};
	switch (axis) {
	case RotationAxis::X:
		r.m[0][0] = 1; r.m[1][1] = c; r.m[1][2] = -s; r.m[2][1] = s; r.m[2][2] = c;
		break;
	case RotationAxis::Y:
		r.m[1][1] = 1; r.m[0][0] = c; r.m[0][2] = s; r.m[2][0] = -s; r.m[2][2] = c;
		break;
	case RotationAxis::Z:
		r.m[2][2] = 1; r.m[0][0] = c; r.m[0][1] = -s; r.m[1][0] = s; r.m[1][1] = c;
		break;
	}
	return r;
}
//3x3行列の掛け算
RubikCube::Mat3 RubikCube::Mat3Mul(const Mat3& a, const Mat3& b) {
	Mat3 r = {};
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			for (int k = 0; k < 3; k++)
				r.m[i][j] += a.m[i][k] * b.m[k][j];
	return r;
}
//オイラー角から3x3行列に変換
RubikCube::Mat3 RubikCube::EulerToMat3(const TuboEngine::Math::Vector3& euler) {
	Mat3 rx = AxisRotationMat3(RotationAxis::X, euler.x);
	Mat3 ry = AxisRotationMat3(RotationAxis::Y, euler.y);
	Mat3 rz = AxisRotationMat3(RotationAxis::Z, euler.z);
	return Mat3Mul(Mat3Mul(rz, ry), rx);
}
//3x3行列からオイラー角に変換
TuboEngine::Math::Vector3 RubikCube::Mat3ToEuler(const Mat3& m) {
	TuboEngine::Math::Vector3 e;
	e.y = asinf(-m.m[2][0]);
	if (cosf(e.y) > 0.0001f) {
		e.x = atan2f(m.m[2][1], m.m[2][2]);
		e.z = atan2f(m.m[1][0], m.m[0][0]);
	}
	else {
		e.x = atan2f(-m.m[1][2], m.m[1][1]);
		e.z = 0.0f;
	}
	return e;
}
//回転軸の符号を取得
float RubikCube::GetRowSign(RotationAxis axis) {
	switch (axis) {
	case RotationAxis::X: return 1.0f;
	case RotationAxis::Y: return -1.0f;
	case RotationAxis::Z: return -1.0f;
	}
	return 1.0f;
}
//回転方向の符号を取得
float RubikCube::GetRotationSign(RotationAxis axis) {
	switch (axis) {
	case RotationAxis::X: return 1.0f;
	case RotationAxis::Y: return -1.0f;
	case RotationAxis::Z: return -1.0f;
	}
	return 1.0f;
}


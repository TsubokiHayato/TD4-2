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
				if (x == 0 && y == 0 && z == 0) continue;//真ん中は見えないからいらない
				std::unique_ptr<TuboEngine::Object3d> object = std::make_unique<TuboEngine::Object3d>();
				object->Initialize("OneCube/OneCube.obj");
				object->SetCamera(camera);
				object->SetPosition({ float(x),float(y),float(z) });
				object->SetScale({ kSize_,kSize_,kSize_ });
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

	//選択した回転列

	selectAxisObject_[0] = std::make_unique<TuboEngine::Object3d>();
	selectAxisObject_[0]->Initialize("SelectAxis/SelectAxis.obj");
	selectAxisObject_[0]->SetCamera(camera);
	selectAxisObject_[0]->SetPosition({0,0,0});
	selectAxisObject_[0]->SetScale({kSize_,kSize_,kSize_});

	selectAxisObject_[1] = std::make_unique<TuboEngine::Object3d>();
	selectAxisObject_[1]->Initialize("SelectAxis/SelectAxis.obj");
	selectAxisObject_[1]->SetCamera(camera);
	selectAxisObject_[1]->SetPosition({ 0,0,0 });
	selectAxisObject_[1]->SetScale({ kSize_,kSize_,kSize_ });

	//回転方向の矢印
	rotateArrowObject_ = std::make_unique<TuboEngine::Object3d>();
	rotateArrowObject_->Initialize("RotateArrow/RotateArrow.obj");
	rotateArrowObject_->SetCamera(camera);
	rotateArrowObject_->SetPosition({ 0,0,0 });
	rotateArrowObject_->SetScale({ kSize_,kSize_,kSize_ });

}

void RubikCube::Update() {

	if (isRotating_) {
		UpdateRotationAnimation();

		for (auto& object : objects_)object->Update();
		for (auto& tip : tips_)tip->Update();

		selectAxisObject_[0]->SetPosition(selectAxisPosition_[0]);
		selectAxisObject_[0]->SetRotation(selectAxisRotate_[0]);
		selectAxisObject_[0]->Update();

		selectAxisObject_[1]->SetPosition(selectAxisPosition_[1]);
		selectAxisObject_[1]->SetRotation(selectAxisRotate_[1]);
		selectAxisObject_[1]->Update();

		rubikCubeState_->RotationDirection(rotation_, rotationArrow_);

		rotateArrowObject_->SetRotation(rotationArrow_);
		rotateArrowObject_->Update();
		return;
	}

	// キーボードによる回転操作は廃止(操作はマウス側 StageScene::MouseCubeControl で行う)。
	// 回転は外部から RequestRotation(axis,row,dir) で依頼される。

	tips_.clear();
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
						object->SetRotation({ 0,0,90.0f * (float(M_PI) / 180.0f) });//右
					}
					else if (i == 2) {
						object->SetPosition({ float(x),-float(y),-1 });
						object->SetRotation({ -90.0f * (float(M_PI) / 180.0f),0,0 });//手前
					}
					else if (i == 3) {
						object->SetPosition({ 1,-float(y),float(x) });
						object->SetRotation({ 0,0,-90.0f * (float(M_PI) / 180.0f) });//左
					}
					else if (i == 4) {
						object->SetPosition({ float(x),-1,float(y) });
						object->SetRotation({ 0,0,180.0f * (float(M_PI) / 180.0f) });//下
					}
					else if (i == 5) {
						object->SetPosition({ -float(x),float(y),1 });
						object->SetRotation({90.0f * (float(M_PI) / 180.0f),0,0 });//奥
					}

					object->SetScale({ 0.5f ,0.5f ,0.5f });
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
	selectAxisObject_[0]->SetPosition(selectAxisPosition_[0]);
	selectAxisObject_[0]->SetRotation(selectAxisRotate_[0]);
	selectAxisObject_[0]->Update();

	selectAxisObject_[1]->SetPosition(selectAxisPosition_[1]);
	selectAxisObject_[1]->SetRotation(selectAxisRotate_[1]);
	selectAxisObject_[1]->Update();

	rubikCubeState_->RotationDirection(rotation_, rotationArrow_);

	rotateArrowObject_->SetRotation(rotationArrow_);
	rotateArrowObject_->Update();
}

void RubikCube::Draw() {
	// --- キューブ本体＋先端の描画 ---
	if (wholeSpinYaw_ == 0.0f) {
		// 全体回転の指定が無ければ従来どおりそのまま描画（Title/Stage はこちら）。
		for (auto& object : objects_) {
			object->Draw();
		}
		for (auto& tip : tips_) {
			tip->Draw();
		}
	} else {
		// キューブ全体を Y 軸まわりに wholeSpinYaw_ だけ回した“見た目”で描画する。
		// 位置と向きを同じ軸・同じ角度で合成する規約は UpdateRotationAnimation と同一。
		// 論理座標(GetPosition/GetRotation が返す値)は最後に元へ戻すので、面回転判定は無傷。
		auto drawSpun = [&](TuboEngine::Object3d* o) {
			TuboEngine::Math::Vector3 pos = o->GetPosition();
			TuboEngine::Math::Vector3 rot = o->GetRotation();

			o->SetPosition(RotateAroundAxis(pos, RotationAxis::Y, wholeSpinYaw_));
			Mat3 spun = Mat3Mul(AxisRotationMat3(RotationAxis::Y, wholeSpinYaw_), EulerToMat3(rot));
			o->SetRotation(Mat3ToEuler(spun));

			o->Update(); // 回した変換で CBuffer を作り直してから描画
			o->Draw();

			o->SetPosition(pos); // 論理値を復元（次フレームの面回転判定のため）
			o->SetRotation(rot);
		};
		for (auto& object : objects_) {
			drawSpun(object.get());
		}
		for (auto& tip : tips_) {
			drawSpun(tip.get());
		}
	}

	// --- 操作補助UI(選択軸ガイド・方向矢印) ---
	// ゲーム操作用なので、表示フラグが立っているときだけ描く（既定 true = Stage）。
	// Title/Clear は SetGuideVisible(false) にしてあるので出ない。
	if (guideVisible_) {
		selectAxisObject_[0]->Draw();
		selectAxisObject_[1]->Draw();
		rotateArrowObject_->Draw();
	}
}

void RubikCube::Debug() {

		   #ifdef USE_IMGUI
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
	#endif

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
	rotateArrowObject_->SetPosition(positionArrow_);


	row_ = row;
	rotation_ = dir; // 反転は呼び出し側(StageScene)で必要に応じて行う
	StartRotationAnimation();
	return true;
}

void RubikCube::GuideRotationAxis(int axis[], int row[]) {
	if (isRotating_) return;

	selectAxisPosition_[0] = {0,0,0};
	selectAxisRotate_[0] = {0,0,0};

	selectAxisPosition_[1] = {0,0,0};
	selectAxisRotate_[1] = {0,0,0};

	//X軸
	if (axis[0] == row[0] && (axis[0] == 1 || axis[0] == -1)) {

		selectAxisPosition_[0].y = float(row[1]);
		selectAxisRotate_[0].z = kNinetyRadian_;// 軸回転

		selectAxisPosition_[1].z = float(row[2]);
		selectAxisRotate_[1].y = kNinetyRadian_;// 軸回転

		positionArrow_ = { float(row[0]),0.0f,0.0f };
	}
	//Y軸
	else if (axis[1] == row[1] && (axis[1] == 1 || axis[1] == -1)) {

		selectAxisPosition_[0].z = float(row[2]);
		selectAxisRotate_[0].y = kNinetyRadian_;// 軸回転

		selectAxisPosition_[1].x = float(row[0]);
		selectAxisRotate_[1].x = kNinetyRadian_;// 軸回転
		
		positionArrow_ = { 0.0f,float(row[1]),0.0f };

	}
	//Z軸
	else if (axis[2] == row[2] && (axis[2] == 1 || axis[2] == -1)) {

		selectAxisPosition_[0].x = float(row[0]);
		selectAxisRotate_[0].x = kNinetyRadian_;// 軸回転

		selectAxisPosition_[1].y = float(row[1]);
		selectAxisRotate_[1].z = kNinetyRadian_;// 軸回転
		
		positionArrow_ = { 0.0f,0.0f,float(row[2]) };

	}



	selectAxisObject_[0]->SetPosition(selectAxisPosition_[0]);
	selectAxisObject_[0]->SetRotation(selectAxisRotate_[0]);
	selectAxisObject_[0]->Update();

	selectAxisObject_[1]->SetPosition(selectAxisPosition_[1]);
	selectAxisObject_[1]->SetRotation(selectAxisRotate_[1]);
	selectAxisObject_[1]->Update();
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
#include "StageScene.h"
#include "GameScenes.h"
#include "SceneManager.h" // シーン遷移を使うとき用
#include "TextManager.h"

#include <CubeMapConverter.h>
#include <ImGuiManager.h>
#include <Input.h>
#include <Matrix.h>
#include <StageBuilder.h>
#include <StageLoader.h>
#include <Vector3.h>
#include <WinApp.h>
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <fstream>
#include <string>

using namespace TuboEngine;

void StageScene::Initialize() {
	// オービットカメラ(SatouScene移植: A/D回転 W/S上下 Q/Eズーム)
	camera_ = std::make_unique<TuboEngine::Camera>();
	camera_->setScale({1.0f, 1.0f, 1.0f});
	CameraRotation();

	background = std::make_unique<TuboEngine::Object3d>();
	background->Initialize("skyBox/skyBox.obj");
	background->SetScale({0.5f, 0.5f, 0.5f});

	// ポーズメニューキューブの初期化
	pauseMenuCube_ = std::make_unique<TuboEngine::Object3d>();
	pauseMenuCube_->Initialize("block/block.obj");
	pauseMenuCube_->SetCamera(camera_.get());
	pauseMenuCube_->SetPosition({0, 0, 0});
	pauseMenuCube_->SetScale({0, 0, 0});

	// Uiクラスの初期化
	ui_ = std::make_unique<Ui>();
	ui_->Initialize();

	// --- ステージ構築(StageLoader → CubeMapConverter → StageBuilder) ---
	StageLoader stageLoader;
	csvData_ = stageLoader.Load(stagePath_);
	cubeCsvData_ = stageLoader.Load(cubeStagePath_);
	RebuildStage();

	// ルービックキューブ(他者作成)。先端配置はキューブCSVから適用する。
	rubikCube_ = std::make_unique<RubikCube>();
	rubikCube_->Initialize(camera_.get());
	ApplyCubeCsv();

	TuboEngine::TextManager::GetInstance()->LoadTextLayout("Resources/Text/Stage.json");
}

void StageScene::Update() {
	// エディター等で要求された再構築は、描画中(コマンド記録中)に
	// GPUリソースを破棄しないよう、フレーム冒頭のここで実行する。
	if (rebuildRequested_) {
		RebuildStage();
		rebuildRequested_ = false;
	}
	if (applyCubeState_) {
		ApplyCubeCsv();
		applyCubeState_ = false;
	}

	// オービットカメラ(A/D回転・W/S上下・Q/Eズーム)
	CameraRotation();

	// マウスドラッグでキューブの面を回す
	MouseCubeControl();

	// パズル本体(キューブ操作＋クリア判定)
	rubikCube_->Update();

	// キューブ状態の変化を検知して操作カウンタを進める(操作が効いたか可視化)
	{
		const SixCube& now = rubikCube_->GetState();
		if (prevCubeStateValid_) {
			bool changed = false;
			for (int i = 0; i < 6 && !changed; i++)
				for (int r = 0; r < 3 && !changed; r++)
					for (int c = 0; c < 3 && !changed; c++)
						if (now.oneCube[i].cube[r][c] != prevCubeState_.oneCube[i].cube[r][c])
							changed = true;
			if (changed)
				moveCount_++;
		}
		prevCubeState_ = now;
		prevCubeStateValid_ = true;
	}

	CheckClear();

	// キューブの回転アニメーション
	CubeAnimation();

	// ポーズメニューキューブ
	pauseMenuCube_->Update();

	// Uiの更新
	ui_->Update();

	// ポーズメニューのスケールを取得
	cubeScale_ = ui_->GetPauseScale();

	pauseMenuCube_->SetScale({cubeBaseScale_ * cubeScale_, cubeBaseScale_ * cubeScale_, cubeBaseScale_ * cubeScale_});

	// ポーズメニューでのシーン切り替え
	ChangeSceneFromPause();


	//背景オブジェクトの回転
	static Vector3 backgroundRotate = {};
	backgroundRotate.x+=0.001f;
	backgroundRotate.z+=0.001f;
	background->SetRotation(backgroundRotate);
	//背景オブジェクトの更新
	background->SetCamera(camera_.get());
	background->Update();


	// TextManagerの更新
	TuboEngine::TextManager::GetInstance()->UpdateAll();

	// 別シーンへ遷移する例:  SceneManager::GetInstance()->ChangeScene(CLEAR);   // 次フレームで切り替わる
}

void StageScene::Finalize() {}

void StageScene::Object3DDraw() {

	background->Draw();

	// 壁(穴つき)とキューブ本体+先端の描画
	for (auto& wall : wallObjects_) {
		wall->Draw();
	}
	rubikCube_->Draw();

	// ポーズメニューキューブの描画
	if (cubeScale_ > 0.01f) {

		pauseMenuCube_->Draw();
	}
} // TODO: 3Dオブジェクト描画
void StageScene::SpriteDraw() {
	// UIの描画
	ui_->DrawStageScene();
	// TextManager
	TuboEngine::TextManager::GetInstance()->DrawAll();

} // TODO: 2Dスプライト描画
void StageScene::ImGuiDraw() {
	// マウスのスライス選択プレビュー＋回転フラッシュ
	DrawMouseGuide();
	// クリア状態・操作状況のHUD(最優先で分かりやすく)
	DrawHud();
	// UIクラスのデバッグ
	ui_->Debug();
	// ルービックキューブのデバッグ
	rubikCube_->Debug();
	// クリア判定(先端と必要位置の一致状況)のデバッグ
	StageClear::DrawDebug(rubikCube_->GetState(), required_);
	// レベルエディター(壁/穴)
	DrawEditor();
	// レベルエディター(キューブ先端)
	DrawCubeEditor();
	// ステージ(壁)サイズ調整。変更したら壁を作り直す。
	ImGui::Begin("Stage Settings");
	bool changed = false;
	// キューブと壁の距離(これを上げると壁がキューブから離れる)
	changed |= ImGui::DragFloat("Wall Distance from Cube", &cubeMargin_, 0.05f, -1.5f, 20.0f);
	changed |= ImGui::DragFloat("Cell Size (grid interval)", &stageCellSize_, 0.05f, 0.2f, 5.0f);
	changed |= ImGui::DragFloat("Wall Scale (tile size)", &wallScale_, 0.02f, 0.05f, 3.0f);
	changed |= ImGui::DragFloat("Wall Thickness", &wallThickness_, 0.01f, 0.01f, 3.0f);
	if (changed) {
		rebuildRequested_ = true; // 実際の再構築は次フレーム冒頭
	}
	ImGui::End();

	// カメラ調整(A/D=回転 W/S=上下 Q/E=ズーム)
	ImGui::Begin("Camera");
	ImGui::Text("A/D: rotate  W/S: up-down  Q/E: zoom");
	ImGui::SliderFloat("Yaw", &yaw_, -3.14159f, 3.14159f);
	ImGui::SliderFloat("Pitch", &pitch_, -1.4f, 1.4f);
	ImGui::DragFloat("Radius(zoom)", &targetRadius_, 0.1f, 3.0f, 50.0f);
	ImGui::DragFloat3("Target", &target_.x, 0.1f);
	ImGui::End();
	// TextManager
	TuboEngine::TextManager::GetInstance()->DrawImGui();
} // TODO: ImGui描画
void StageScene::ParticleDraw() {} // TODO: パーティクル描画
// キューブの回転アニメーション
void StageScene::CubeAnimation() {

	bool rotating = ui_->IsRotating();

	const float PI = 3.1415926f;
	const float DEG90 = PI * 0.5f;

	// 回転終了時に確定
	if (prevRotating_ && !rotating) {

		basecubeAngle_ += ui_->GetRotateDir() * DEG90;

		// 正規化（-π〜πでもOK）
		if (basecubeAngle_ >= PI * 2.0f)
			basecubeAngle_ -= PI * 2.0f;
		if (basecubeAngle_ < 0.0f)
			basecubeAngle_ += PI * 2.0f;
	}

	float drawAngle = basecubeAngle_;

	if (rotating) {

		float t = ui_->GetRotateTimer();

		// ease
		t = (t < 0.5f) ? 2.0f * t * t : 1.0f - std::pow(-2.0f * t + 2.0f, 2.0f) / 2.0f;

		drawAngle += ui_->GetRotateDir() * DEG90 * t;
	}

	pauseMenuCube_->SetRotation({0.0f, drawAngle, 0.0f});

	prevRotating_ = rotating;
}
// ポーズメニューでのシーン切り替え
void StageScene::ChangeSceneFromPause() {
	switch (ui_->GetPauseMenu()) {
	case Ui::PauseMenuType::Retry:
		SceneManager::GetInstance()->ChangeScene(STAGE);
		ui_->SetPauseMenu(Ui::PauseMenuType::None);
		break;
	case Ui::PauseMenuType::ToTitle:
		SceneManager::GetInstance()->ChangeScene(TITLE);
		ui_->SetPauseMenu(Ui::PauseMenuType::None);
		break;
	case Ui::PauseMenuType::ToSelect:

		ui_->SetPauseMenu(Ui::PauseMenuType::None);
		break;
	default:
		break;
	}
}

// 壁(穴つき)オブジェクトを WallData から生成
void StageScene::BuildWalls() {
	wallObjects_.clear();

	for (const auto& wall : wallData_) {
		if (!wall.isActive)
			continue;

		auto obj = std::make_unique<TuboEngine::Object3d>();

		// 穴の形状に応じてモデルを切り替える(穴なしは壁モデル)
		switch (wall.holeType) {
		case HoleType::Cone:
			obj->Initialize("cone/cone.obj");
			break;
		case HoleType::Square:
			obj->Initialize("square/square.obj");
			break;
		default:
			obj->Initialize("wall/wall.obj");
			break;
		}

		obj->SetCamera(camera_.get());
		obj->SetPosition(wall.worldPos);
		obj->SetRotation(wall.rotation);
		// グリッド間隔(cellSize)と見た目の大きさ(wallScale_)は分離する。
		// 壁/穴モデルは実寸2ユニットなので 1マス=1ユニットにするには 0.5 前後。
		obj->SetScale({wallScale_, wallScale_, wallScale_});
		obj->Update();

		wallObjects_.push_back(std::move(obj));
	}
}

// セルサイズ等の変更時に壁データと描画を作り直す(csvData_ を元に再構築)
void StageScene::RebuildStage() {
	// 穴データ(展開図→各面)。CSVの寸法検証も兼ねる。
	CubeMapConverter converter(kCubeSize);
	std::vector<HoleData> holeData = converter.Convert(csvData_);
	(void)holeData; // 穴一覧(必要に応じて可視化/検証に利用)

	// 壁配置(描画とクリア判定の共通ソース)
	StageBuilder builder(kCubeSize, stageCellSize_);
	builder.SetWallThickness(wallThickness_);
	builder.SetCubeMargin(cubeMargin_);
	wallData_ = builder.Build(csvData_);

	// 壁(穴つき)の描画オブジェクトと、先端が必要な位置を生成
	BuildWalls();
	required_ = StageClear::BuildRequired(wallData_, kCubeSize);
}

// 展開図(十字)上の有効セルか(6面のいずれかに属するか)
bool StageScene::IsValidCell(int x, int y) const {
	const int n = kCubeSize;
	// 十字ネット: 中央の横帯(y∈[n,2n)) 全域、または中央の縦帯(x∈[n,2n)) 全域
	bool horizontalBand = (y >= n && y < 2 * n && x >= 0 && x < 4 * n);
	bool verticalBand = (x >= n && x < 2 * n && y >= 0 && y < 3 * n);
	return horizontalBand || verticalBand;
}

// 現在の csvData_ を stagePath_ に保存(カンマ区切り)
void StageScene::SaveStage() {
	std::ofstream ofs(stagePath_);
	if (!ofs)
		return;
	for (const auto& row : csvData_) {
		for (size_t x = 0; x < row.size(); ++x) {
			ofs << row[x];
			if (x + 1 < row.size())
				ofs << ",";
		}
		ofs << "\n";
	}
}

// 現在の cubeCsvData_ を cubeStagePath_ に保存(カンマ区切り)
void StageScene::SaveCubeStage() {
	std::ofstream ofs(cubeStagePath_);
	if (!ofs)
		return;
	for (const auto& row : cubeCsvData_) {
		for (size_t x = 0; x < row.size(); ++x) {
			ofs << row[x];
			if (x + 1 < row.size())
				ofs << ",";
		}
		ofs << "\n";
	}
}

// 指定番号のステージ(壁+キューブCSV)を読み込む。
// GPUリソースの再生成はフラグ経由で次フレーム冒頭に行う(描画中破棄の回避)。
void StageScene::LoadStage(int index) {
	if (index < 1)
		index = 1;
	if (index > kStageCount)
		index = kStageCount;
	stageIndex_ = index;
	stagePath_ = "Resources/4209_stages/stage" + std::to_string(index) + ".csv";
	cubeStagePath_ = "Resources/4209_stages/cube" + std::to_string(index) + ".csv";

	StageLoader loader;
	csvData_ = loader.Load(stagePath_);
	cubeCsvData_ = loader.Load(cubeStagePath_);

	cleared_ = false;
	rebuildRequested_ = true;
	applyCubeState_ = true;
}

// cubeCsvData_ をキューブの先端配置として適用する
void StageScene::ApplyCubeCsv() {
	if (cubeCsvData_.empty() || !rubikCube_)
		return;

	// 壁と同じ StageBuilder の面規約で展開図セル→面/ローカル座標に変換し、
	// 有効セルすべてを先端としてキューブ状態を作る。
	StageBuilder builder(kCubeSize, stageCellSize_);
	std::vector<WallData> cells = builder.Build(cubeCsvData_);
	rubikCube_->SetState(StageClear::BuildCubeState(cells, kCubeSize));
}

// クリア判定とクリア時のシーン遷移
void StageScene::CheckClear() {
	// 壁トランスフォームは固定なので更新するだけ
	for (auto& wall : wallObjects_) {
		wall->Update();
	}

	// 編集モード中はクリア遷移しない(レベルデザインに集中するため)
	if (cleared_ || editorEnabled_)
		return;

	if (StageClear::IsClear(rubikCube_->GetState(), required_)) {
		// 最終ステージをクリアしたら CLEAR シーンへ。
		// それ以外は自動遷移せず、HUD の「Next Stage」で次へ進む。
		if (stageIndex_ >= kStageCount) {
			cleared_ = true;
			SceneManager::GetInstance()->ChangeScene(CLEAR);
		}
	}
}

// 注視点(キューブ中心)を回るオービットカメラの更新
// カメラ回転(SatouScene から移植: A/D=ヨー W/S=ピッチ Q/E=ズーム、球面座標)
void StageScene::CameraRotation() {
	Input* input = Input::GetInstance();

	const float speed = 0.02f;
	if (input->PushKey(DIK_A))
		yaw_ += speed;
	if (input->PushKey(DIK_D))
		yaw_ -= speed;
	if (input->PushKey(DIK_W))
		pitch_ += speed;
	if (input->PushKey(DIK_S))
		pitch_ -= speed;
	pitch_ = std::clamp(pitch_, -1.4f, 1.4f);

	const float zoomSpeed = 0.3f;
	if (input->PushKey(DIK_Q))
		targetRadius_ += zoomSpeed;
	if (input->PushKey(DIK_E))
		targetRadius_ -= zoomSpeed;
	targetRadius_ = std::clamp(targetRadius_, 3.0f, 50.0f);

	// 球面座標からカメラ位置を計算
	TuboEngine::Math::Vector3 pos;
	pos.x = target_.x + targetRadius_ * std::cos(pitch_) * std::sin(yaw_);
	pos.y = target_.y + targetRadius_ * std::sin(pitch_);
	pos.z = target_.z + targetRadius_ * std::cos(pitch_) * std::cos(yaw_);

	camera_->SetTranslate(pos);
	camera_->setRotation({pitch_, yaw_ + 3.141592f, 0.0f});
	camera_->Update();
}

namespace {
// Vector3 の成分をインデックス(0=x,1=y,2=z)で取得
float GetAxis(const TuboEngine::Math::Vector3& v, int a) { return (a == 0) ? v.x : (a == 1) ? v.y : v.z; }
// RubikCube::GetRotationSign と同じ(X:+1 / Y,Z:-1)
float RotationSignOf(int axis) { return (axis == 0) ? 1.0f : -1.0f; }
// RubikCube::RotateAroundAxis と同じ回転(点posをaxis周りにangle回す)
TuboEngine::Math::Vector3 RotateAroundAxisLocal(const TuboEngine::Math::Vector3& pos, int axis, float angle) {
	float c = std::cos(angle), s = std::sin(angle);
	TuboEngine::Math::Vector3 r = pos;
	if (axis == 0) { // X
		r.y = pos.y * c - pos.z * s;
		r.z = pos.y * s + pos.z * c;
	} else if (axis == 1) { // Y
		r.x = pos.x * c + pos.z * s;
		r.z = -pos.x * s + pos.z * c;
	} else { // Z
		r.x = pos.x * c - pos.y * s;
		r.y = pos.x * s + pos.y * c;
	}
	return r;
}
} // namespace

// カーソルに最も近い「前向きの面のセル」を選ぶ。
// 逆投影を使わず、各セルを前方投影(描画と同じVP)して最近傍を取るのでどのカメラ角度でも一致する。
bool StageScene::PickBlock(float mx, float my, int cell[3], int normal[3]) {
	using namespace TuboEngine::Math;
	float w = static_cast<float>(TuboEngine::WinApp::GetInstance()->GetClientWidth());
	float h = static_cast<float>(TuboEngine::WinApp::GetInstance()->GetClientHeight());

	Matrix4x4 vp = camera_->GetViewProjectionMatrix();
	auto project = [&](Vector3 wp, ImVec2& out) -> bool {
		Vector3 c = TransformCoord(wp, vp);
		// カメラ背後(w<0でTransformCoordが破綻)を弾くため、ビュー空間zで前方判定
		Vector3 vpos = TransformCoord(wp, camera_->GetViewMatrix());
		if (vpos.z <= 0.0f)
			return false; // カメラの後ろ
		out = ImVec2((c.x * 0.5f + 0.5f) * w, (1.0f - (c.y * 0.5f + 0.5f)) * h);
		return true;
	};

	Vector3 eye = camera_->GetTranslate();
	float best = 1e30f;
	bool found = false;

	// 6面 × 面内3x3 のサーフェスセルを走査
	for (int fa = 0; fa < 3; fa++) {
		for (int s = -1; s <= 1; s += 2) {
			// 前向きの面だけ対象(カメラがその面の外側にある)
			if (s * GetAxis(eye, fa) <= 1.0f)
				continue;

			int inplane[2], k = 0;
			for (int ax = 0; ax < 3; ax++)
				if (ax != fa)
					inplane[k++] = ax;

			for (int u = -1; u <= 1; u++) {
				for (int v = -1; v <= 1; v++) {
					float comp[3];
					comp[fa] = static_cast<float>(s);
					comp[inplane[0]] = static_cast<float>(u);
					comp[inplane[1]] = static_cast<float>(v);
					ImVec2 sp;
					if (!project(Vector3{comp[0], comp[1], comp[2]}, sp))
						continue;
					float dx = sp.x - mx, dy = sp.y - my;
					float dist = dx * dx + dy * dy;
					if (dist < best) {
						best = dist;
						found = true;
						cell[fa] = s;
						cell[inplane[0]] = u;
						cell[inplane[1]] = v;
						normal[0] = normal[1] = normal[2] = 0;
						normal[fa] = s;
					}
				}
			}
		}
	}

	// カーソルからの許容半径(px)。これ以上遠ければキューブ外とみなす。
	const float kPickRadius = 90.0f;
	if (!found || best > kPickRadius * kPickRadius)
		return false;
	return true;
}

// マウスで指したブロックのスライスをドラッグで回す(RubikCubeは改変せずSetStateで反映)。
void StageScene::MouseCubeControl() {
	Input* input = Input::GetInstance();
	float mx = input->GetMousePosition().x;
	float my = input->GetMousePosition().y;

	// ホバー中のブロックを毎フレーム更新(ハイライト用)
	pickValid_ = false;
	if (!ImGui::GetIO().WantCaptureMouse) {
		int cell[3], nrm[3];
		if (PickBlock(mx, my, cell, nrm)) {
			pickValid_ = true;
			for (int i = 0; i < 3; i++) {
				pickCell_[i] = cell[i];
				pickNormal_[i] = nrm[i];
			}
		}
	}

	if (ImGui::GetIO().WantCaptureMouse) {
		dragging_ = false;
		return;
	}

	// 左押下: ブロックを指していたら掴む(押した瞬間のみ)
	if (input->IsTriggerMouse(0) && pickValid_) {
		dragging_ = true;
		dragAccumX_ = 0.0f;
		dragAccumY_ = 0.0f;
		for (int i = 0; i < 3; i++) {
			dragPickCell_[i] = pickCell_[i];
			dragPickNormal_[i] = pickNormal_[i];
		}
	}
	if (!input->IsPressMouse(0))
		dragging_ = false; // ボタンを離したら終了
	if (!dragging_)
		return;

	// 掴んでいる間ドラッグ量を累積
	Input::MouseMove mv = input->GetMouseMove();
	dragAccumX_ += static_cast<float>(mv.lX);
	dragAccumY_ += static_cast<float>(mv.lY);

	// アニメ中は回転を発行せず「待つ」(ドラッグは維持したまま)。
	// これで連続ドラッグ中もアニメ後に続けて回せる。
	if (rubikCube_->IsRotating())
		return;

	const float kThreshold = 45.0f;
	if (std::abs(dragAccumX_) < kThreshold && std::abs(dragAccumY_) < kThreshold)
		return;

	// 基準は「掴んだ瞬間のブロック」(dragPickCell_)に固定する。
	// ここでカーソル位置に更新すると、ドラッグでずれた別ブロックが基準になってしまう。

	// 掴んだ面の面内2軸(=回転軸の候補)を列挙
	int fa = (dragPickNormal_[0] != 0) ? 0 : (dragPickNormal_[1] != 0) ? 1 : 2;
	int cand[2], k = 0;
	for (int ax = 0; ax < 3; ax++)
		if (ax != fa)
			cand[k++] = ax;

	using namespace TuboEngine::Math;
	float w = static_cast<float>(TuboEngine::WinApp::GetInstance()->GetClientWidth());
	float h = static_cast<float>(TuboEngine::WinApp::GetInstance()->GetClientHeight());
	Matrix4x4 vp = camera_->GetViewProjectionMatrix();
	auto project = [&](Vector3 wpt) -> ImVec2 {
		Vector3 c = TransformCoord(wpt, vp);
		return ImVec2((c.x * 0.5f + 0.5f) * w, (1.0f - (c.y * 0.5f + 0.5f)) * h);
	};

	// 掴んだブロックの中心と、そのスクリーン位置
	Vector3 center = {(float)dragPickCell_[0], (float)dragPickCell_[1], (float)dragPickCell_[2]};
	ImVec2 sc = project(center);

	// 「回転軸×向き」の各候補で、実際にエンジンが回したときブロックが画面上で
	// どちらへ動くかをシミュレートし、ドラッグ方向に最も一致する候補を選ぶ。
	// これによりカメラの向き・掴んだ面に関係なく見た目通りに回る。
	const float kEps = 0.15f;
	int rotAxis = cand[0], dir = 0;
	float best = -1e30f;
	for (int ci = 0; ci < 2; ci++) {
		int axis = cand[ci];
		for (int d = 0; d < 2; d++) {
			// エンジンの回転角の符号 (UpdateRotationAnimation と同じ規約)
			float engineSign = ((d == 1) ? 1.0f : -1.0f) * RotationSignOf(axis);
			Vector3 moved = RotateAroundAxisLocal(center, axis, kEps * engineSign);
			ImVec2 sm = project(moved);
			ImVec2 dirScreen = {sm.x - sc.x, sm.y - sc.y};
			float score = dragAccumX_ * dirScreen.x + dragAccumY_ * dirScreen.y;
			if (score > best) {
				best = score;
				rotAxis = axis;
				dir = d;
			}
		}
	}
	int layer = dragPickCell_[rotAxis];

	// world層 → RubikCube の row_ に変換 (GetRowSign: X=+1, Y/Z=-1)
	int row = (rotAxis == 0) ? (layer + 1) : (1 - layer);

	// RubikCube の回転アニメを起動(論理更新もアニメ側で行われる)。
	// 1ドラッグ=1回転。基準ブロックが変わらないよう、回したら一旦掴みを解除する
	// (次の回転はもう一度ブロックを掴み直す)。
	if (rubikCube_->RequestRotation(rotAxis, row, dir)) {
		rotateFlash_ = 40.0f;
	}
	dragging_ = false;
	dragAccumX_ = 0.0f;
	dragAccumY_ = 0.0f;
}

// 指しているブロックの面をハイライトし、回転フラッシュを描く
void StageScene::DrawMouseGuide() {
	using namespace TuboEngine::Math;
	ImGuiIO& io = ImGui::GetIO();
	float w = io.DisplaySize.x;
	float h = io.DisplaySize.y;
	ImDrawList* dl = ImGui::GetForegroundDrawList();

	Matrix4x4 vp = camera_->GetViewProjectionMatrix();
	auto project = [&](Vector3 wp) -> ImVec2 {
		Vector3 c = TransformCoord(wp, vp);
		return ImVec2((c.x * 0.5f + 0.5f) * w, (1.0f - (c.y * 0.5f + 0.5f)) * h);
	};

	// 診断表示(ピック状況・カメラ角)。原因切り分け用。
	{
		char dbg[192];
		snprintf(
		    dbg, sizeof(dbg), "pick=%d cell(%d,%d,%d) nrm(%d,%d,%d) drag=%d yaw=%.2f pitch=%.2f", pickValid_ ? 1 : 0, pickCell_[0], pickCell_[1], pickCell_[2], pickNormal_[0], pickNormal_[1],
		    pickNormal_[2], dragging_ ? 1 : 0, yaw_, pitch_);
		dl->AddText(ImVec2(12.0f, 34.0f), IM_COL32(255, 240, 0, 255), dbg);
	}

	// ホバー中(またはドラッグ中)のブロックの面をハイライト
	const int* cellSrc = dragging_ ? dragPickCell_ : pickCell_;
	const int* nrmSrc = dragging_ ? dragPickNormal_ : pickNormal_;
	if (dragging_ || pickValid_) {
		int fa = (nrmSrc[0] != 0) ? 0 : (nrmSrc[1] != 0) ? 1 : 2;
		int sign = nrmSrc[fa];
		int inplane[2], k = 0;
		for (int ax = 0; ax < 3; ax++)
			if (ax != fa)
				inplane[k++] = ax;

		// 面の外側(±1.5)にセルの四隅を作りスクリーンへ投影
		const int su[4] = {-1, 1, 1, -1};
		const int sv[4] = {-1, -1, 1, 1};
		ImVec2 pc[4];
		for (int i = 0; i < 4; i++) {
			float comp[3];
			comp[fa] = sign * 1.5f;
			comp[inplane[0]] = static_cast<float>(cellSrc[inplane[0]]) + 0.5f * su[i];
			comp[inplane[1]] = static_cast<float>(cellSrc[inplane[1]]) + 0.5f * sv[i];
			pc[i] = project(Vector3{comp[0], comp[1], comp[2]});
		}
		ImU32 fill = dragging_ ? IM_COL32(120, 255, 120, 60) : IM_COL32(80, 180, 255, 55);
		ImU32 edge = dragging_ ? IM_COL32(120, 255, 120, 230) : IM_COL32(140, 200, 255, 230);
		dl->AddQuadFilled(pc[0], pc[1], pc[2], pc[3], fill);
		dl->AddQuad(pc[0], pc[1], pc[2], pc[3], edge, 2.5f);
	}

	// 回転した瞬間のフラッシュ表示
	if (rotateFlash_ > 0.0f) {
		rotateFlash_ -= 1.0f;
		const char* t = "ROTATE!";
		float scale = 3.0f;
		float fs = ImGui::GetFontSize() * scale;
		ImVec2 ts = ImGui::CalcTextSize(t);
		dl->AddText(ImGui::GetFont(), fs, ImVec2(w * 0.5f - ts.x * scale * 0.5f, h * 0.10f), IM_COL32(120, 255, 120, 255), t);
	}
}

// クリア状態・操作状況を分かりやすく表示するHUD
void StageScene::DrawHud() {
	const SixCube& cur = rubikCube_->GetState();

	// 形状まで一致していない穴の数(位置+形状)
	int remain = 0;
	for (int i = 0; i < 6; i++)
		for (int r = 0; r < 3; r++)
			for (int c = 0; c < 3; c++)
				if (required_.oneCube[i].cube[r][c] >= 1 && cur.oneCube[i].cube[r][c] != required_.oneCube[i].cube[r][c])
					remain++;
	bool clear = (remain == 0);

	ImGui::Begin("HUD");
	ImGui::Text("Stage: %d / %d", stageIndex_, kStageCount);
	if (ImGui::Button("< Prev")) {
		LoadStage(stageIndex_ - 1);
	}
	ImGui::SameLine();
	if (ImGui::Button("Next >")) {
		LoadStage(stageIndex_ + 1);
	}
	ImGui::Separator();
	if (clear) {
		ImGui::TextColored(ImVec4(0.3f, 1.0f, 0.3f, 1), "STATE: CLEAR!");
		if (stageIndex_ < kStageCount) {
			if (ImGui::Button("Next Stage >>")) {
				LoadStage(stageIndex_ + 1);
			}
		} else {
			ImGui::TextDisabled("(final stage - turn off Editor Mode to finish)");
		}
	} else {
		ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.2f, 1), "STATE: not clear");
	}
	ImGui::Text("Unmatched holes : %d", remain);
	ImGui::Text("Cube moves      : %d", moveCount_);
	ImGui::Separator();
	ImGui::Text("[Camera] A/D:rotate  W/S:up-down  Q/E:zoom");
	ImGui::Separator();
	ImGui::Text("[Cube controls]");
	ImGui::BulletText("R : change rotation axis (X/Y/Z)");
	ImGui::BulletText("LEFT / RIGHT : change row (0-2)");
	ImGui::BulletText("UP / DOWN : direction (1 / 0)");
	ImGui::BulletText("SPACE : rotate selected slice");
	ImGui::BulletText("Mouse: point a block (highlighted) & drag to rotate");
	ImGui::TextDisabled("(current axis/row: see 'Rubik Cube' window)");
	ImGui::Separator();
	ImGui::Checkbox("Editor Mode (block clear transition)", &editorEnabled_);
	ImGui::End();

	// クリア時は画面中央に大きく表示
	if (clear) {
		ImGuiIO& io = ImGui::GetIO();
		ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.30f), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
		ImGuiWindowFlags flags =
		    ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_AlwaysAutoResize;
		ImGui::Begin("ClearOverlay", nullptr, flags);
		ImGui::SetWindowFontScale(3.0f);
		ImGui::TextColored(ImVec4(0.3f, 1.0f, 0.3f, 1), "*** CLEAR! ***");
		ImGui::SetWindowFontScale(1.0f);
		ImGui::End();
	}
}

// レベルエディター(ImGui でCSVを直接編集)
void StageScene::DrawEditor() {
	ImGui::Begin("Level Editor");

	ImGui::Checkbox("Editor Mode (stop clear transition)", &editorEnabled_);
	ImGui::TextWrapped("Click a cell to cycle: Empty -> Wall -> Cone -> Square");
	ImGui::Text("File: %s", stagePath_.c_str());

	if (ImGui::Button("Save CSV")) {
		SaveStage();
	}
	ImGui::SameLine();
	if (ImGui::Button("Reload")) {
		StageLoader loader;
		csvData_ = loader.Load(stagePath_);
		rebuildRequested_ = true; // 実際の再構築は次フレーム冒頭
	}

	ImGui::Separator();
	// 凡例
	ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1), "1:Wall");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(0.2f, 0.8f, 0.2f, 1), "2:Cone(C)");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(0.2f, 0.4f, 1.0f, 1), "3:Square(S)");
	ImGui::Separator();

	const float cell = 26.0f;
	bool changed = false;

	for (int y = 0; y < static_cast<int>(csvData_.size()); ++y) {
		for (int x = 0; x < static_cast<int>(csvData_[y].size()); ++x) {

			// 面に属さない無効セルは空白(編集不可)。ここを非0にすると
			// StageBuilder/CubeMapConverter が例外を投げるため触らせない。
			if (!IsValidCell(x, y)) {
				ImGui::Dummy(ImVec2(cell, cell));
				if (x < static_cast<int>(csvData_[y].size()) - 1)
					ImGui::SameLine();
				continue;
			}

			int v = csvData_[y][x];
			ImVec4 col;
			switch (v) {
			case 1:
				col = ImVec4(0.5f, 0.5f, 0.5f, 1);
				break; // 壁
			case 2:
				col = ImVec4(0.2f, 0.8f, 0.2f, 1);
				break; // Cone
			case 3:
				col = ImVec4(0.2f, 0.4f, 1.0f, 1);
				break; // Square
			default:
				col = ImVec4(0.15f, 0.15f, 0.15f, 1);
				break; // 0:空
			}

			ImGui::PushStyleColor(ImGuiCol_Button, col);
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, col);
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, col);

			std::string label = (v == 2) ? "C" : (v == 3) ? "S" : "";
			label += "##" + std::to_string(y) + "_" + std::to_string(x);

			if (ImGui::Button(label.c_str(), ImVec2(cell, cell))) {
				csvData_[y][x] = (v + 1) % 4;
				changed = true;
			}
			ImGui::PopStyleColor(3);

			if (x < static_cast<int>(csvData_[y].size()) - 1)
				ImGui::SameLine();
		}
	}

	if (changed) {
		rebuildRequested_ = true; // 実際の再構築は次フレーム冒頭
	}

	ImGui::End();
}

// キューブ先端エディター(ImGui でキューブCSVを直接編集)
void StageScene::DrawCubeEditor() {
	ImGui::Begin("Cube Tip Editor");

	ImGui::TextWrapped("Click a cell to cycle tip: Empty -> Cone -> Square");
	ImGui::Text("File: %s", cubeStagePath_.c_str());

	if (ImGui::Button("Save Cube CSV")) {
		SaveCubeStage();
	}
	ImGui::SameLine();
	if (ImGui::Button("Reload Cube")) {
		StageLoader loader;
		cubeCsvData_ = loader.Load(cubeStagePath_);
		applyCubeState_ = true; // 次フレームで適用
	}

	ImGui::Separator();
	ImGui::TextColored(ImVec4(0.2f, 0.8f, 0.2f, 1), "2:Cone(C)");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(0.2f, 0.4f, 1.0f, 1), "3:Square(S)");
	ImGui::Separator();

	const float cell = 26.0f;
	bool changed = false;

	for (int y = 0; y < static_cast<int>(cubeCsvData_.size()); ++y) {
		for (int x = 0; x < static_cast<int>(cubeCsvData_[y].size()); ++x) {

			if (!IsValidCell(x, y)) {
				ImGui::Dummy(ImVec2(cell, cell));
				if (x < static_cast<int>(cubeCsvData_[y].size()) - 1)
					ImGui::SameLine();
				continue;
			}

			int v = cubeCsvData_[y][x];
			ImVec4 col;
			switch (v) {
			case 2:
				col = ImVec4(0.2f, 0.8f, 0.2f, 1);
				break; // Cone
			case 3:
				col = ImVec4(0.2f, 0.4f, 1.0f, 1);
				break; // Square
			default:
				col = ImVec4(0.15f, 0.15f, 0.15f, 1);
				break; // 空
			}

			ImGui::PushStyleColor(ImGuiCol_Button, col);
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, col);
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, col);

			std::string label = (v == 2) ? "C" : (v == 3) ? "S" : "";
			label += "##cube" + std::to_string(y) + "_" + std::to_string(x);

			if (ImGui::Button(label.c_str(), ImVec2(cell, cell))) {
				// 空 -> Cone(2) -> Square(3) -> 空 …(壁CSVと同じ形状エンコード)
				if (v == 2)
					cubeCsvData_[y][x] = 3;
				else if (v == 3)
					cubeCsvData_[y][x] = 0;
				else
					cubeCsvData_[y][x] = 2;
				changed = true;
			}
			ImGui::PopStyleColor(3);

			if (x < static_cast<int>(cubeCsvData_[y].size()) - 1)
				ImGui::SameLine();
		}
	}

	if (changed) {
		applyCubeState_ = true; // 実際の適用は次フレーム冒頭
	}

	ImGui::End();
}
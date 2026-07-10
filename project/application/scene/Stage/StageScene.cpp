#include "StageScene.h"
#include "GameScenes.h"
#include "TextManager.h"
#include "SceneManager.h" // シーン遷移を使うとき用

#include <StageLoader.h>
#include <CubeMapConverter.h>
#include <StageBuilder.h>
#include <Input.h>
#include <ImGuiManager.h>
#include <cmath>
#include <fstream>
#include <string>

using namespace TuboEngine;

void StageScene::Initialize() {
	// オービットカメラ(キューブ中心を斜め上から見る)
	camera_ = std::make_unique<TuboEngine::Camera>();
	camera_->setScale({ 1.0f, 1.0f, 1.0f });
	UpdateCamera();

	//ポーズメニューキューブの初期化
	pauseMenuCube_ = std::make_unique<TuboEngine::Object3d>();
	pauseMenuCube_->Initialize("block/block.obj");
	pauseMenuCube_->SetCamera(camera_.get());
	pauseMenuCube_->SetPosition({ 0,0,0 });
	pauseMenuCube_->SetScale({ 0,0,0 });

	//Uiクラスの初期化
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

	// オービットカメラ(矢印キーで注視点まわりに回転)
	UpdateCamera();

	// パズル本体(キューブ操作＋クリア判定)
	rubikCube_->Update();
	CheckClear();

		//キューブの回転アニメーション
	CubeAnimation();

	//ポーズメニューキューブ
	pauseMenuCube_->Update();

	//Uiの更新
	ui_->Update();

	//ポーズメニューのスケールを取得
	cubeScale_ = ui_->GetPauseScale();

	pauseMenuCube_->SetScale({
		cubeBaseScale_ * cubeScale_,
		cubeBaseScale_ * cubeScale_,
		cubeBaseScale_ * cubeScale_
		});

	//ポーズメニューでのシーン切り替え
	ChangeSceneFromPause();
	//TextManagerの更新
	TuboEngine::TextManager::GetInstance()->UpdateAll();

	// 別シーンへ遷移する例:  SceneManager::GetInstance()->ChangeScene(CLEAR);   // 次フレームで切り替わる
}

void StageScene::Finalize() {}

void StageScene::Object3DDraw() {

	// 壁(穴つき)とキューブ本体+先端の描画
	for (auto& wall : wallObjects_) {
		wall->Draw();
	}
	rubikCube_->Draw();

	//ポーズメニューキューブの描画
	if (cubeScale_ > 0.01f) {

		pauseMenuCube_->Draw();
	}
}   // TODO: 3Dオブジェクト描画
void StageScene::SpriteDraw() {
	//UIの描画
	ui_->DrawStageScene();
	//TextManager
	TuboEngine::TextManager::GetInstance()->DrawAll();

}    // TODO: 2Dスプライト描画
void StageScene::ImGuiDraw() {
	//UIクラスのデバッグ
	ui_->Debug();
	//ルービックキューブのデバッグ
	rubikCube_->Debug();
	//クリア判定(先端と必要位置の一致状況)のデバッグ
	StageClear::DrawDebug(rubikCube_->GetState(), required_);
	//レベルエディター(壁/穴)
	DrawEditor();
	//レベルエディター(キューブ先端)
	DrawCubeEditor();
	//ステージ(壁)サイズ調整。変更したら壁を作り直す。
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

	//カメラ調整
	ImGui::Begin("Camera");
	ImGui::Text("Arrow keys: orbit");
	ImGui::SliderFloat("Yaw", &camYaw_, -3.14159f, 3.14159f);
	ImGui::SliderFloat("Pitch", &camPitch_, -1.4f, 1.4f);
	ImGui::DragFloat("Distance", &camDistance_, 0.1f, 1.0f, 60.0f);
	ImGui::DragFloat3("Target", &camTarget_.x, 0.1f);
	ImGui::End();
	//TextManager
	TuboEngine::TextManager::GetInstance()->DrawImGui();
}    // TODO: ImGui描画
void StageScene::ParticleDraw() {}    // TODO: パーティクル描画
//キューブの回転アニメーション
void StageScene::CubeAnimation() {

	bool rotating = ui_->IsRotating();

	const float PI = 3.1415926f;
	const float DEG90 = PI * 0.5f;

	// 回転終了時に確定
	if (prevRotating_ && !rotating) {

		basecubeAngle_ += ui_->GetRotateDir() * DEG90;

		// 正規化（-π〜πでもOK）
		if (basecubeAngle_ >= PI * 2.0f) basecubeAngle_ -= PI * 2.0f;
		if (basecubeAngle_ < 0.0f) basecubeAngle_ += PI * 2.0f;
	}

	float drawAngle = basecubeAngle_;

	if (rotating) {

		float t = ui_->GetRotateTimer();

		// ease
		t = (t < 0.5f)
			? 2.0f * t * t
			: 1.0f - std::pow(-2.0f * t + 2.0f, 2.0f) / 2.0f;

		drawAngle += ui_->GetRotateDir() * DEG90 * t;
	}

	pauseMenuCube_->SetRotation({
		0.0f,
		drawAngle,
		0.0f
		});

	prevRotating_ = rotating;
}
//ポーズメニューでのシーン切り替え
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

//壁(穴つき)オブジェクトを WallData から生成
void StageScene::BuildWalls() {
	wallObjects_.clear();

	for (const auto& wall : wallData_) {
		if (!wall.isActive) continue;

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
		obj->SetScale({ wallScale_, wallScale_, wallScale_ });
		obj->Update();

		wallObjects_.push_back(std::move(obj));
	}
}

//セルサイズ等の変更時に壁データと描画を作り直す(csvData_ を元に再構築)
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

//展開図(十字)上の有効セルか(6面のいずれかに属するか)
bool StageScene::IsValidCell(int x, int y) const {
	const int n = kCubeSize;
	// 十字ネット: 中央の横帯(y∈[n,2n)) 全域、または中央の縦帯(x∈[n,2n)) 全域
	bool horizontalBand = (y >= n && y < 2 * n && x >= 0 && x < 4 * n);
	bool verticalBand = (x >= n && x < 2 * n && y >= 0 && y < 3 * n);
	return horizontalBand || verticalBand;
}

//現在の csvData_ を stagePath_ に保存(カンマ区切り)
void StageScene::SaveStage() {
	std::ofstream ofs(stagePath_);
	if (!ofs) return;
	for (const auto& row : csvData_) {
		for (size_t x = 0; x < row.size(); ++x) {
			ofs << row[x];
			if (x + 1 < row.size()) ofs << ",";
		}
		ofs << "\n";
	}
}

//現在の cubeCsvData_ を cubeStagePath_ に保存(カンマ区切り)
void StageScene::SaveCubeStage() {
	std::ofstream ofs(cubeStagePath_);
	if (!ofs) return;
	for (const auto& row : cubeCsvData_) {
		for (size_t x = 0; x < row.size(); ++x) {
			ofs << row[x];
			if (x + 1 < row.size()) ofs << ",";
		}
		ofs << "\n";
	}
}

//cubeCsvData_ をキューブの先端配置として適用する
void StageScene::ApplyCubeCsv() {
	if (cubeCsvData_.empty() || !rubikCube_) return;

	// 壁と同じ StageBuilder の面規約で展開図セル→面/ローカル座標に変換し、
	// 有効セルすべてを先端としてキューブ状態を作る。
	StageBuilder builder(kCubeSize, stageCellSize_);
	std::vector<WallData> cells = builder.Build(cubeCsvData_);
	rubikCube_->SetState(StageClear::BuildCubeState(cells, kCubeSize));
}

//クリア判定とクリア時のシーン遷移
void StageScene::CheckClear() {
	// 壁トランスフォームは固定なので更新するだけ
	for (auto& wall : wallObjects_) {
		wall->Update();
	}

	// 編集モード中はクリア遷移しない(レベルデザインに集中するため)
	if (cleared_ || editorEnabled_) return;

	if (StageClear::IsClear(rubikCube_->GetState(), required_)) {
		cleared_ = true;
		SceneManager::GetInstance()->ChangeScene(CLEAR);
	}
}

//注視点(キューブ中心)を回るオービットカメラの更新
void StageScene::UpdateCamera() {
	Input* input = Input::GetInstance();

	// 矢印キーで注視点まわりに回転(WASDはキューブ操作が使用中のため矢印に割当)
	if (input->PushKey(DIK_LEFT))  camYaw_ -= camRotateSpeed_;
	if (input->PushKey(DIK_RIGHT)) camYaw_ += camRotateSpeed_;
	if (input->PushKey(DIK_UP))    camPitch_ += camRotateSpeed_;
	if (input->PushKey(DIK_DOWN))  camPitch_ -= camRotateSpeed_;

	// 真上/真下でのフリップを防ぐ
	const float kPitchLimit = 1.4f;
	if (camPitch_ > kPitchLimit)  camPitch_ = kPitchLimit;
	if (camPitch_ < -kPitchLimit) camPitch_ = -kPitchLimit;

	// 前方 = ローカル+Z を Rx(pitch)*Ry(yaw) で回したもの。
	// 注視点の後方 distance にカメラを置く。
	float cp = std::cos(camPitch_);
	float sp = std::sin(camPitch_);
	float cy = std::cos(camYaw_);
	float sy = std::sin(camYaw_);
	TuboEngine::Math::Vector3 forward = { cp * sy, -sp, cp * cy };

	TuboEngine::Math::Vector3 pos = {
		camTarget_.x - forward.x * camDistance_,
		camTarget_.y - forward.y * camDistance_,
		camTarget_.z - forward.z * camDistance_,
	};

	camera_->SetTranslate(pos);
	camera_->setRotation({ camPitch_, camYaw_, 0.0f });
	camera_->Update();
}

//レベルエディター(ImGui でCSVを直接編集)
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
	ImGui::SameLine(); ImGui::TextColored(ImVec4(0.2f, 0.8f, 0.2f, 1), "2:Cone(C)");
	ImGui::SameLine(); ImGui::TextColored(ImVec4(0.2f, 0.4f, 1.0f, 1), "3:Square(S)");
	ImGui::Separator();

	const float cell = 26.0f;
	bool changed = false;

	for (int y = 0; y < static_cast<int>(csvData_.size()); ++y) {
		for (int x = 0; x < static_cast<int>(csvData_[y].size()); ++x) {

			// 面に属さない無効セルは空白(編集不可)。ここを非0にすると
			// StageBuilder/CubeMapConverter が例外を投げるため触らせない。
			if (!IsValidCell(x, y)) {
				ImGui::Dummy(ImVec2(cell, cell));
				if (x < static_cast<int>(csvData_[y].size()) - 1) ImGui::SameLine();
				continue;
			}

			int v = csvData_[y][x];
			ImVec4 col;
			switch (v) {
			case 1:  col = ImVec4(0.5f, 0.5f, 0.5f, 1); break; // 壁
			case 2:  col = ImVec4(0.2f, 0.8f, 0.2f, 1); break; // Cone
			case 3:  col = ImVec4(0.2f, 0.4f, 1.0f, 1); break; // Square
			default: col = ImVec4(0.15f, 0.15f, 0.15f, 1); break; // 0:空
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

			if (x < static_cast<int>(csvData_[y].size()) - 1) ImGui::SameLine();
		}
	}

	if (changed) {
		rebuildRequested_ = true; // 実際の再構築は次フレーム冒頭
	}

	ImGui::End();
}

//キューブ先端エディター(ImGui でキューブCSVを直接編集)
void StageScene::DrawCubeEditor() {
	ImGui::Begin("Cube Tip Editor");

	ImGui::TextWrapped("Click a cell to toggle a tip (Empty <-> Tip)");
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
	ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.2f, 1), "1:Tip");
	ImGui::Separator();

	const float cell = 26.0f;
	bool changed = false;

	for (int y = 0; y < static_cast<int>(cubeCsvData_.size()); ++y) {
		for (int x = 0; x < static_cast<int>(cubeCsvData_[y].size()); ++x) {

			if (!IsValidCell(x, y)) {
				ImGui::Dummy(ImVec2(cell, cell));
				if (x < static_cast<int>(cubeCsvData_[y].size()) - 1) ImGui::SameLine();
				continue;
			}

			int v = cubeCsvData_[y][x];
			ImVec4 col = (v >= 1) ? ImVec4(1.0f, 0.8f, 0.2f, 1)   // 先端あり
			                      : ImVec4(0.15f, 0.15f, 0.15f, 1); // 空

			ImGui::PushStyleColor(ImGuiCol_Button, col);
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, col);
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, col);

			std::string label = (v >= 1) ? "T" : "";
			label += "##cube" + std::to_string(y) + "_" + std::to_string(x);

			if (ImGui::Button(label.c_str(), ImVec2(cell, cell))) {
				cubeCsvData_[y][x] = (v >= 1) ? 0 : 1; // 先端の有無をトグル
				changed = true;
			}
			ImGui::PopStyleColor(3);

			if (x < static_cast<int>(cubeCsvData_[y].size()) - 1) ImGui::SameLine();
		}
	}

	if (changed) {
		applyCubeState_ = true; // 実際の適用は次フレーム冒頭
	}

	ImGui::End();
}
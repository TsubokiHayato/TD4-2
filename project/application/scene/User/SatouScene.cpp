#include "SatouScene.h"
#include "GameScenes.h"
#include "SceneManager.h" // シーン遷移を使うとき用
#include <StageBuilder.h>

void SatouScene::Initialize() {
	// 最低限のカメラ
	camera_ = std::make_unique<TuboEngine::Camera>();
	camera_->SetTranslate({ 0.0f, 0.0f, -15.0f });
	camera_->setRotation({ 0.0f, 0.0f, 0.0f });
	camera_->setScale({ 1.0f, 1.0f, 1.0f });
	camera_->Update();

    // 開発・デバッグ用のカメラ初期化
    debugCamera_ = std::make_unique<DebugCamera>();

	StageLoader stageLoader;
    CubeMapConverter converter(kCubeSize);

	// CSVファイルからステージの構成データを読み込む
	csvData_ = stageLoader.Load("Resources/4209_stages/stage1.csv");
	// 読み込んだデータを展開図・各面データに変換する
	holeData_ = converter.Convert(csvData_);

    // 変換されたデータから壁の配置などを構築する
    StageBuilder builder(kCubeSize, kCellSize);
    wallData_ = builder.Build(csvData_);

    // 各セル座標に応じた3Dオブジェクトを生成・初期配置する
    CreateWalls();
}

void SatouScene::Update() {
	// カメラの更新
	camera_->Update();
    debugCamera_->Update(camera_.get());

    // 各セル(壁・コーン・四角)のトランスフォーム等の更新
    for (auto& cell : cellObjects_) {
        cell.wall->Update();
        cell.cone->Update();
        cell.square->Update();
    }
	// TODO: ここに更新処理（入力・ゲームロジック）を書く
	// 別シーンへ遷移する例:  SceneManager::GetInstance()->ChangeScene(STAGE);   // 次フレームで切り替わる
}

void SatouScene::Finalize() {}

void SatouScene::Object3DDraw() {
    // 設定されたタイプ(1:壁, 2:コーン, 3:四角)に従って3Dモデルを描画する
    for (auto& cell : cellObjects_) {
        if (!cell.isActive) continue;

        switch (cell.type) {
        case 1:
            cell.wall->Draw();
            break;
        case 2:
            cell.cone->Draw();
            break;
        case 3:
            cell.square->Draw();
            break;
        }
    }
}   // TODO: 3Dオブジェクト描画
void SatouScene::SpriteDraw()  {}    // TODO: 2Dスプライト描画
void SatouScene::ImGuiDraw()   {
    // デバッグカメラ用UIの描画
    debugCamera_->DrawImGui();
    
    // --- ステージ構造の可視化とエディタ ---
    ImGui::Begin("Stage Map");

    const float cellSize = 30.0f;

    for (int y = 0; y < csvData_.size(); y++) {
        for (int x = 0; x < csvData_[y].size(); x++) {

            int value = csvData_[y][x];

            ImVec4 color;

            switch (value) {
                ImGui::Dummy(ImVec2(cellSize, cellSize));
                if (x < csvData_[y].size() - 1)
                    ImGui::SameLine();
                continue;

            case 1:
                color = ImVec4(0.5f, 0.5f, 0.5f, 1.0f); // 壁
                break;

            case 2:
                color = ImVec4(0.2f, 0.8f, 0.2f, 1.0f); // Cone
                break;

            case 3:
                color = ImVec4(0.2f, 0.4f, 1.0f, 1.0f); // Square
                break;

            default:
                color = ImVec4(1, 0, 0, 1);
                break;
            }

            ImGui::PushStyleColor(ImGuiCol_Button, color);
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, color);
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, color);

            std::string label;

            if (value == 2) label = "C";
            else if (value == 3) label = "S";
            else label = "";

            label += "##" + std::to_string(y) + "_" + std::to_string(x);

            if (ImGui::Button(label.c_str(), ImVec2(cellSize, cellSize))) {
                csvData_[y][x] = (csvData_[y][x] + 1) % 4;

                int idx = Index(x, y);
                cellObjects_[idx].type = csvData_[y][x];
                cellObjects_[idx].isActive = (csvData_[y][x] != 0);
            }
            ImGui::PopStyleColor(3);

            if (x < csvData_[y].size() - 1) {
                ImGui::SameLine();
            }
        }
    }

    ImGui::End();
    
    // --- ステージ生成パラメータの調整 ---
    ImGui::Begin("Stage Settings");

    bool changed = false;

    changed |= ImGui::DragFloat("Cell Size", &stageCellSize_, 0.1f, 0.5f, 10.0f);
    changed |= ImGui::DragFloat("Wall Thickness", &wallThickness_, 0.01f, 0.01f, 5.0f);
    changed |= ImGui::DragFloat("Cube Margin", &cubeMargin_, 0.1f, 0.0f, 20.0f);

    // 値が変更されたら壁のトランスフォームを再計算して反映する
    if (changed) {
        RebuildWalls();
    }

    ImGui::End();
    
    // --- 生成された壁データの確認 ---
	ImGui::Begin("Wall Data");
    for (auto& wall : wallData_) {
        ImGui::Text("Face Type: %d", (int)wall.face);
		ImGui::Text("WallType: %d", (int)wall.holeType);
		ImGui::Text("world Pos: (%f, %f)", wall.worldPos.x, wall.worldPos.y);
		ImGui::Text("Rotation: (%f, %f, %f)", wall.rotation.x, wall.rotation.y, wall.rotation.z);
		ImGui::Text("Scale: (%f, %f, %f)", wall.scale.x, wall.scale.y, wall.scale.z);
    }
    ImGui::End();
	

}    // TODO: ImGui描画
void SatouScene::ParticleDraw(){}    // TODO: パーティクル描画

/// <summary>
/// ステージ全体の描画オブジェクトを生成する。
/// 各セルごとに Wall / Cone / Square を事前生成し、
/// 描画時に type で切り替える方式を採用している。
/// </summary>
void SatouScene::CreateWalls()
{
    int height = static_cast<int>(csvData_.size());
    int width = static_cast<int>(csvData_[0].size());

    cellObjects_.resize(width * height);

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {

            int idx = Index(x, y);
            auto& cell = cellObjects_[idx];

            SetupObject(cell.wall, "wall/wall.obj");
            SetupObject(cell.cone, "cone/cone.obj");
            SetupObject(cell.square, "square/square.obj");

            cell.type = csvData_[y][x];
            cell.isActive = (cell.type != 0);
        }
    }

    RebuildWalls();
}

// モデル初期化のヘルパー関数
void SatouScene::SetupObject(std::unique_ptr<Object3d>& obj, const std::string& path)
{
    obj = std::make_unique<Object3d>();
    obj->Initialize(path);
    obj->SetCamera(camera_.get());
}

/// <summary>
/// CSVデータから壁配置情報を再生成する。
/// セルサイズ・壁厚・マージン変更時に使用。
/// </summary>
void SatouScene::RebuildWalls()
{
    StageBuilder builder(kCubeSize, stageCellSize_);
    builder.SetWallThickness(wallThickness_);
    builder.SetCubeMargin(cubeMargin_);

    wallData_ = builder.Build(csvData_);
    UpdateWallTransform();
}
// <summary>
/// 各セルの位置・回転を再計算して反映する。
/// Cube展開図上の座標を3D空間へ変換する。
/// </summary>
void SatouScene::UpdateWallTransform()
{
    StageBuilder builder(kCubeSize, stageCellSize_);
    builder.SetWallThickness(wallThickness_);
    builder.SetCubeMargin(cubeMargin_);

    int height = static_cast<int>(csvData_.size());
    int width = static_cast<int>(csvData_[0].size());

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {

            if (csvData_[y][x] == 0) continue;

            int idx = Index(x, y);

            FaceType face = builder.GetFace(x, y);
            GridPos local = builder.GetLocalPos(x, y, face);

            auto& cell = cellObjects_[idx];

            Vector3 pos = builder.GetWorldPos(face, local);
            Vector3 rot = builder.GetRotation(face);


            cell.wall->SetPosition(pos);
            cell.wall->SetRotation(rot);

            cell.cone->SetPosition(pos);
            cell.cone->SetRotation(rot);

            cell.square->SetPosition(pos);
            cell.square->SetRotation(rot);
        }
    }
}

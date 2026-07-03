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

    debugCamera_ = std::make_unique<DebugCamera>();

	StageLoader stageLoader;
    CubeMapConverter converter(kCubeSize);

	csvData_ = stageLoader.Load("Resources/4209_stages/stage1.csv");
	holeData_ = converter.Convert(csvData_);

    StageBuilder builder(kCubeSize, kCellSize);
    wallData_ = builder.Build(csvData_);

    CreateWalls();
}

void SatouScene::Update() {
	camera_->Update();
    debugCamera_->Update(camera_.get());

    for (auto& wall : wallObjects_) {
        wall->Update();
    }
	// TODO: ここに更新処理（入力・ゲームロジック）を書く
	// 別シーンへ遷移する例:  SceneManager::GetInstance()->ChangeScene(STAGE);   // 次フレームで切り替わる
}

void SatouScene::Finalize() {}

void SatouScene::Object3DDraw() {
    for (auto& wall : wallObjects_) {
        wall->Draw();
    }
}   // TODO: 3Dオブジェクト描画
void SatouScene::SpriteDraw()  {}    // TODO: 2Dスプライト描画
void SatouScene::ImGuiDraw()   {
    debugCamera_->DrawImGui();
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
                RebuildWalls();
            }
            ImGui::PopStyleColor(3);

            if (x < csvData_[y].size() - 1) {
                ImGui::SameLine();
            }
        }
    }

    ImGui::End();
    ImGui::Begin("Stage Settings");

    bool changed = false;

    changed |= ImGui::DragFloat("Cell Size", &stageCellSize_, 0.1f, 0.5f, 10.0f);
    changed |= ImGui::DragFloat("Wall Thickness", &wallThickness_, 0.01f, 0.01f, 5.0f);
    changed |= ImGui::DragFloat("Cube Margin", &cubeMargin_, 0.1f, 0.0f, 20.0f);

    if (changed) {
        RebuildWalls();
    }

    ImGui::End();
	ImGui::Begin("Wall Data");
    for (auto& wall : wallData_) {
        ImGui::Text("Face Type: %d", (int)wall.face);
		ImGui::Text("WallType: %d", (int)wall.holeType);
		ImGui::Text("world Pos: (%f, %f)", wall.worldPos.x, wall.worldPos.y);
		ImGui::Text("Rotation: (%f, %f, %f)", wall.rotation.x, wall.rotation.y, wall.rotation.z);
    }
    ImGui::End();
	

}    // TODO: ImGui描画
void SatouScene::ParticleDraw(){}    // TODO: パーティクル描画

void SatouScene::CreateWalls()
{
    wallObjects_.clear();

    for (const auto& wall : wallData_) {
        auto obj = std::make_unique<Object3d>();

        if (wall.holeType == HoleType::None) {
            obj->Initialize("wall/wall.obj");
        }
        else if (wall.holeType == HoleType::Cone) {
            obj->Initialize("cone/cone.obj");
		}
		else if (wall.holeType == HoleType::Square) {
			obj->Initialize("square/square.obj");
		}

        obj->SetPosition(wall.worldPos);
        obj->SetRotation(wall.rotation);
        obj->SetScale({ 1,1,1 });
        obj->SetCamera(camera_.get());

        wallObjects_.push_back(std::move(obj));
    }
}

void SatouScene::RebuildWalls()
{
    StageBuilder builder(kCubeSize, stageCellSize_);
    builder.SetWallThickness(wallThickness_);
    builder.SetCubeMargin(cubeMargin_);

    wallData_ = builder.Build(csvData_);
    UpdateWallTransform();
}

void SatouScene::UpdateWallTransform()
{
    for (size_t i = 0; i < wallObjects_.size(); i++) {
        wallObjects_[i]->SetPosition(wallData_[i].worldPos);
        wallObjects_[i]->SetRotation(wallData_[i].rotation);
        wallObjects_[i]->SetScale({ 1,1,1 });
    }
}

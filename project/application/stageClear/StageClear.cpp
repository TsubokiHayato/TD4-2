#include "StageClear.h"
#include <ImGuiManager.h>

int StageClear::FaceToCubeIndex(FaceType face) {
    // 面法線が一致するルービック面インデックスへ対応付ける。
    //   RubikCube 面: 0=+Y / 1=-X / 2=-Z / 3=+X / 4=-Y / 5=+Z
    switch (face) {
    case FaceType::Top:    return 0; // +Y
    case FaceType::Left:   return 1; // -X
    case FaceType::Back:   return 2; // -Z
    case FaceType::Right:  return 3; // +X
    case FaceType::Bottom: return 4; // -Y
    case FaceType::Front:  return 5; // +Z
    }
    return 0;
}

StageClear::Coord StageClear::CubeCellToCoord(int face, int row, int col) {
    // RubikCube::Update の先端配置と厳密に一致させること。
    // x = col-1, y = row-1
    int x = col - 1;
    int y = row - 1;
    switch (face) {
    case 0: return { x,  1, -y }; // Top(+Y)
    case 1: return { -1, -y, -x }; // Right(-X面)
    case 2: return { x, -y, -1 }; // Front(-Z面)
    case 3: return { 1, -y,  x }; // Left(+X面)
    case 4: return { x, -1,  y }; // Bottom(-Y)
    case 5: return { -x,  y,  1 }; // Back(+Z面)
    }
    return { 0, 0, 0 };
}

StageClear::Coord StageClear::HoleToCoord(FaceType face, GridPos local, int cubeSize) {
    // StageBuilder::GetWorldPos を cellSize=1・margin=0 相当に正規化した整数座標。
    //   offset = (cubeSize-1)/2, l = offset - localPos
    int offset = (cubeSize - 1) / 2;
    int lx = offset - local.x;
    int ly = offset - local.y;
    switch (face) {
    case FaceType::Front:  return { lx,  ly,  1 };
    case FaceType::Back:   return { -lx,  ly, -1 };
    case FaceType::Right:  return { 1,  ly, -lx };
    case FaceType::Left:   return { -1,  ly,  lx };
    case FaceType::Top:    return { -lx,  1,  ly };
    case FaceType::Bottom: return { -lx, -1, -ly };
    }
    return { 0, 0, 0 };
}

SixCube StageClear::BuildRequired(const std::vector<WallData>& walls, int cubeSize) {
    SixCube required{};
    for (int i = 0; i < 6; i++) {
        for (int row = 0; row < 3; row++) {
            for (int col = 0; col < 3; col++) {
                required.oneCube[i].cube[row][col] = 0;
            }
        }
    }

    for (const auto& wall : walls) {
        if (wall.holeType == HoleType::None) {
            continue; // 穴なしの壁は先端を要求しない
        }

        int face = FaceToCubeIndex(wall.face);
        Coord target = HoleToCoord(wall.face, wall.localPos, cubeSize);

        // 対応面内で座標が一致するセルに「先端が必要」を立てる
        for (int row = 0; row < 3; row++) {
            for (int col = 0; col < 3; col++) {
                Coord c = CubeCellToCoord(face, row, col);
                if (c.x == target.x && c.y == target.y && c.z == target.z) {
                    required.oneCube[face].cube[row][col] = 1;
                }
            }
        }
    }

    return required;
}

SixCube StageClear::BuildCubeState(const std::vector<WallData>& cells, int cubeSize) {
    SixCube state{};
    for (int i = 0; i < 6; i++) {
        for (int row = 0; row < 3; row++) {
            for (int col = 0; col < 3; col++) {
                state.oneCube[i].cube[row][col] = 0;
            }
        }
    }

    for (const auto& cell : cells) {
        if (!cell.isActive) {
            continue;
        }

        int face = FaceToCubeIndex(cell.face);
        Coord target = HoleToCoord(cell.face, cell.localPos, cubeSize);

        for (int row = 0; row < 3; row++) {
            for (int col = 0; col < 3; col++) {
                Coord c = CubeCellToCoord(face, row, col);
                if (c.x == target.x && c.y == target.y && c.z == target.z) {
                    state.oneCube[face].cube[row][col] = 1; // 先端あり
                }
            }
        }
    }

    return state;
}

bool StageClear::IsClear(const SixCube& current, const SixCube& required) {
    for (int i = 0; i < 6; i++) {
        for (int row = 0; row < 3; row++) {
            for (int col = 0; col < 3; col++) {
                if (required.oneCube[i].cube[row][col] >= 1 &&
                    current.oneCube[i].cube[row][col] == 0) {
                    return false; // 塞がれていない穴がある
                }
            }
        }
    }
    return true;
}

void StageClear::DrawDebug(const SixCube& current, const SixCube& required) {
    ImGui::Begin("Stage Clear");

    ImGui::Text("State: %s", IsClear(current, required) ? "CLEAR!" : "not yet");

    int remain = 0;
    for (int i = 0; i < 6; i++) {
        for (int row = 0; row < 3; row++) {
            for (int col = 0; col < 3; col++) {
                if (required.oneCube[i].cube[row][col] >= 1 &&
                    current.oneCube[i].cube[row][col] == 0) {
                    remain++;
                }
            }
        }
    }
    ImGui::Text("Unfilled holes: %d", remain);
    ImGui::Separator();

    // 面ごとに [先端有無 / 穴要求] を並べ、未達の穴に印を付ける
    for (int i = 0; i < 6; i++) {
        ImGui::Text("Face %d  (tip / need)", i);
        for (int row = 0; row < 3; row++) {
            for (int col = 0; col < 3; col++) {
                uint32_t cur = current.oneCube[i].cube[row][col];
                uint32_t need = required.oneCube[i].cube[row][col];
                bool ng = (need >= 1 && cur == 0);
                ImGui::TextColored(
                    ng ? ImVec4(1, 0.3f, 0.3f, 1) : ImVec4(0.6f, 1, 0.6f, 1),
                    "%u/%u%s", cur, need, ng ? "x" : " ");
                if (col < 2) ImGui::SameLine();
            }
        }
        ImGui::Separator();
    }

    ImGui::End();
}

#include "StageBuilder.h"
#include <iostream>

StageBuilder::StageBuilder(int cubeSize, float cellSize)
    : cubeSize_(cubeSize),
    cellSize_(cellSize) {
}

std::vector<WallData> StageBuilder::Build(const std::vector<std::vector<int>>& csvData)
{
    std::vector<WallData> walls;

    for (int y = 0; y < csvData.size(); ++y) {
        for (int x = 0; x < csvData[y].size(); ++x) {

            int value = csvData[y][x];

            // 無効領域
            if (value == 0) {
                continue;
            }

            FaceType face = GetFace(x, y);
            GridPos localPos = GetLocalPos(x, y, face);

            walls.push_back({
                face,
                localPos,
                GetWorldPos(face, localPos),
                GetRotation(face),
                { cellSize_, cellSize_, cellSize_ },
                GetHoleType(value)
                });
        }
    }

    return walls;
}

FaceType StageBuilder::GetFace(int x, int y) const
{
    // Top
    if (x >= cubeSize_ && x < cubeSize_ * 2 &&
        y >= 0 && y < cubeSize_) {
        return FaceType::Top;
    }

    // Left
    if (x >= 0 && x < cubeSize_ &&
        y >= cubeSize_ && y < cubeSize_ * 2) {
        return FaceType::Left;
    }

    // Front
    if (x >= cubeSize_ && x < cubeSize_ * 2 &&
        y >= cubeSize_ && y < cubeSize_ * 2) {
        return FaceType::Front;
    }

    // Right
    if (x >= cubeSize_ * 2 && x < cubeSize_ * 3 &&
        y >= cubeSize_ && y < cubeSize_ * 2) {
        return FaceType::Right;
    }

    // Back
    if (x >= cubeSize_ * 3 && x < cubeSize_ * 4 &&
        y >= cubeSize_ && y < cubeSize_ * 2) {
        return FaceType::Back;
    }

    // Bottom
    if (x >= cubeSize_ && x < cubeSize_ * 2 &&
        y >= cubeSize_ * 2 && y < cubeSize_ * 3) {
        return FaceType::Bottom;
    }

    throw std::runtime_error("無効な座標です");
}

GridPos StageBuilder::GetLocalPos(int x, int y, FaceType face) const
{
    switch (face) {

        // Top面内座標
    case FaceType::Top:
        return { x - cubeSize_, y };

        // Left面内座標
    case FaceType::Left:
        return { x, y - cubeSize_ };

        // Front面内座標
    case FaceType::Front:
        return { x - cubeSize_, y - cubeSize_ };

        // Right面内座標
    case FaceType::Right:
        return { x - cubeSize_ * 2, y - cubeSize_ };

        // Back面内座標
    case FaceType::Back:
        return { x - cubeSize_ * 3, y - cubeSize_ };

        // Bottom面内座標
    case FaceType::Bottom:
        return { x - cubeSize_, y - cubeSize_ * 2 };
    }

    throw std::runtime_error("無効な座標です");
}

HoleType StageBuilder::GetHoleType(int value) const
{
    switch (value) {
    case 2: return HoleType::Cone;
    case 3: return HoleType::Square;
    default: return HoleType::None;
    }
}

Vector3 StageBuilder::GetWorldPos(FaceType face, GridPos pos) const
{
    float half = cubeSize_ * cellSize_ * 0.5f;
    float surfaceOffset = half + cubeMargin_ + wallThickness_ * 0.5f;
    float offset = (cubeSize_ - 1) * 0.5f;

    float localX = (pos.x - offset) * cellSize_;
    float localY = -(pos.y - offset) * cellSize_;

    switch (face) {
    case FaceType::Front:
        return { localX, localY, surfaceOffset };

    case FaceType::Back:
        return { -localX, localY, -surfaceOffset };

    case FaceType::Right:
        return { surfaceOffset, localY, -localX };

    case FaceType::Left:
        return { -surfaceOffset, localY, localX };

    case FaceType::Top:
        return { localX, surfaceOffset, -localY };

    case FaceType::Bottom:
        return { localX, -surfaceOffset, localY };
    }

    return { 0,0,0 };
}

Vector3 StageBuilder::GetRotation(FaceType face) const
{
    switch (face) {
    case FaceType::Front:
        return { 1.5708f,0,0 };

    case FaceType::Back:
        return { -1.5708f,0,0 };

    case FaceType::Right:
        return { 1.5708f,1.5708f,0 };

    case FaceType::Left:
        return { 1.5708f,-1.5708f,0 };

    case FaceType::Top:
        return { 0,0,0 };

    case FaceType::Bottom:
        return { 3.14159f,0,0 };
    }
    throw std::runtime_error("Invalid FaceType");
}
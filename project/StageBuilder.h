#pragma once
#include <vector>
#include "WallData.h"

using namespace TuboEngine::Math;

class StageBuilder {
public:
    StageBuilder(int cubeSize, float cellSize);

    std::vector<WallData> Build(
        const std::vector<std::vector<int>>& csvData
    );

    void SetWallThickness(float thickness) { wallThickness_ = thickness; }

    void SetCubeMargin(float margin) { cubeMargin_ = margin; }
private:
    FaceType GetFace(int x, int y) const;
    GridPos GetLocalPos(int x, int y, FaceType face) const;
    HoleType GetHoleType(int value) const;

    Vector3 GetWorldPos(FaceType face, GridPos pos) const;
    Vector3 GetRotation(FaceType face) const;

	

    int cubeSize_;
    float cellSize_;
	float wallThickness_ = 0.2f; // 壁の厚み
    // 中央からの距離
    float cubeMargin_ = 5.0f;
};
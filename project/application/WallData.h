#pragma once
#include <FaceType.h>
#include <HoleType.h>
#include <GridPos.h>
#include <Vector3.h>
struct WallData {
    FaceType face;
    GridPos localPos;
    TuboEngine::Math::Vector3 worldPos;
    TuboEngine::Math::Vector3 rotation;
    TuboEngine::Math::Vector3 scale;
    HoleType holeType;
    bool isActive = true;
};
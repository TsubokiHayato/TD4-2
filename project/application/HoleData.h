#pragma once
#include "Vector2.h"
#include <MapChip.h>
#include <FaceType.h>
#include <HoleType.h>
#include <GridPos.h>

struct HoleData {
    FaceType face;
    GridPos localPos;
    HoleType holeType;
};
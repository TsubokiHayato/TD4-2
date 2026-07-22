#pragma once
#include <vector>
#include <HoleData.h>

// CSV形式のステージデータを読み取り、
// キューブの各面の穴情報(HoleData)に変換するクラス
class CubeMapConverter {
public:
    // cubeSize:
    // 1面あたりのマス数（3なら3x3）
    CubeMapConverter(int cubeSize);

    // CSVデータを HoleData の配列に変換する
    std::vector<HoleData> Convert(
        const std::vector<std::vector<int>>& csvData
    );

    // ImGui
	void DrawFaceDebug(const std::vector<HoleData>& holes, FaceType face, int cubeSize);

private:
    // 展開図上の座標(x, y)からどの面かを判定
    FaceType GetFace(int x, int y) const;

    // 展開図上の座標(x, y)を、
    // 面内ローカル座標(0 ~ cubeSize_-1)に変換
    GridPos GetLocalPos(int x, int y, FaceType face) const;

    // CSVの値を HoleType に変換
    HoleType GetHoleType(int value) const;

    // キューブ1面のサイズ
    int cubeSize_;
};
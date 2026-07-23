#include "CubeMapConverter.h"
#include <iostream>
#include <ImGuiManager.h>
/*
CSV定義
0 : 無効領域
1 : 壁（穴なし）
2 : Cone穴
3 : Square穴
*/

CubeMapConverter::CubeMapConverter(int cubeSize)
	: cubeSize_(cubeSize) {
}

std::vector<HoleData> CubeMapConverter::Convert(
	const std::vector<std::vector<int>>& csvData)
{
	// 展開図のサイズ
	// 横: 4面分
	// 縦: 3面分
	int expectedWidth = cubeSize_ * 4;
	int expectedHeight = cubeSize_ * 3;

	// 行数チェック
	if (csvData.size() != expectedHeight) {
		throw std::runtime_error("CSVの行数が期待値と異なります。");
	}

	// 列数チェック
	for (const auto& row : csvData) {
		if (row.size() != expectedWidth) {
			throw std::runtime_error("CSVの列数が期待値と異なります。");
		}
	}

	std::vector<HoleData> holeDataList;

	// CSV全体を走査
	for (int y = 0; y < csvData.size(); ++y) {
		for (int x = 0; x < csvData[y].size(); ++x) {

			int value = csvData[y][x];

			// 0 = 無効領域
			// 1 = 壁（穴なし）
			if (value <= 1) {
				continue;
			}

			// どの面か判定
			FaceType face = GetFace(x, y);

			// 穴情報を登録
			holeDataList.push_back({
				face,
				GetLocalPos(x, y, face),
				GetHoleType(value)
				});
		}
	}

	return holeDataList;
}

void CubeMapConverter::DrawFaceDebug(const std::vector<HoleData>& holes, FaceType face, int cubeSize)
{
#ifdef USE_IMGUI
	if (ImGui::TreeNode("Face")) {

		for (int y = 0; y < cubeSize; y++) {
			for (int x = 0; x < cubeSize; x++) {

				int value = 1; // 壁

				for (const auto& hole : holes) {
					if (hole.face == face &&
						hole.localPos.x == x &&
						hole.localPos.y == y) {
						value = static_cast<int>(hole.holeType) + 1;
						break;
					}
				}

				ImGui::Text("%d", value);
				ImGui::SameLine();
			}
			ImGui::NewLine();
		}

		ImGui::TreePop();
	}
#else
	(void)holes; (void)face; (void)cubeSize;
#endif
}

FaceType CubeMapConverter::GetFace(int x, int y) const
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
GridPos CubeMapConverter::GetLocalPos(int x, int y, FaceType face) const
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

HoleType CubeMapConverter::GetHoleType(int value) const
{
	switch (value) {
	case 2: return HoleType::Cone;
	case 3: return HoleType::Square;
	default: return HoleType::None;
	}
}

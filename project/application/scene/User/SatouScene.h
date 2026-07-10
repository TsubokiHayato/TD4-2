#pragma once
#include "IScene.h"
#include "Camera.h"
#include <memory>
#include <StageLoader.h>
#include <CubeMapConverter.h>
#include <Object3d.h>
#include <WallData.h>
#include <DebugCamera.h>
#include <Vector3.h>
using namespace TuboEngine::Math;

using namespace TuboEngine;

/// <summary>
/// ステージ上の1マスに対応する描画オブジェクト群。
/// 各マスごとに Wall / Cone / Square の3種類を保持し、
/// type に応じて描画対象を切り替える。
/// 再生成せずに表示切替できるようにするための構造体。
/// </summary>
struct CellObject {
	std::unique_ptr<Object3d> wall;
	std::unique_ptr<Object3d> cone;
	std::unique_ptr<Object3d> square;

	bool isActive = false; // このマスが有効か
	int type = 0;          // 0=None, 1=Wall, 2=Cone, 3=Square
};

/// <summary>
/// 佐藤用ステージ編集シーン。
/// 
/// CSVからステージデータを読み込み、
/// キューブ展開図形式で各面にオブジェクトを配置する。
/// ImGuiからリアルタイムでステージ編集が可能。
/// </summary>
class SatouScene : public IScene {
public:
	void Initialize() override;
	void Update() override;
	void Finalize() override;
	void Object3DDraw() override;
	void SpriteDraw() override;
	void ImGuiDraw() override;
	void ParticleDraw() override;

	TuboEngine::Camera* GetMainCamera() const override { return camera_.get(); }

private:
	/// <summary>
	/// 2次元座標を1次元配列インデックスへ変換
	/// </summary>
	int Index(int x, int y) {
		int width = static_cast<int>(csvData_[0].size());
		return y * width + x;
	}

	/// <summary>
	/// ステージ全マス分のCellObjectを生成
	/// </summary>
	void CreateWalls();

	/// <summary>
	/// Object3d生成共通処理
	/// </summary>
	void SetupObject(std::unique_ptr<Object3d>& obj, const std::string& path);

	/// <summary>
	/// CSVデータから壁情報を再構築
	/// セルサイズや壁厚変更時に呼ぶ
	/// </summary>
	void RebuildWalls();

	/// <summary>
	/// 各CellObjectにTransformを反映
	/// </summary>
	void UpdateWallTransform();

	/// <summary>
	/// カメラの回転処理
	/// </summary>
	void CameraRotation();

private:
	// メインカメラ
	std::unique_ptr<TuboEngine::Camera> camera_;

	// デバッグ用フリーカメラ
	std::unique_ptr<TuboEngine::DebugCamera> debugCamera_;

	// ステージCSVデータ
	std::vector<std::vector<int>> csvData_;

	// 穴情報（必要なら判定などで使用）
	std::vector<HoleData> holeData_;

	// 壁配置情報
	std::vector<WallData> wallData_;

	// 全マス分の描画オブジェクト
	std::vector<CellObject> cellObjects_;

	// キューブ1辺のマス数
	static constexpr int kCubeSize = 3;

	// デフォルトセルサイズ
	static constexpr float kCellSize = 2.0f;

	// ステージ設定
	float stageCellSize_ = 2.0f;
	float wallThickness_ = 0.2f;
	float cubeMargin_ = 5.0f;

	Vector3 target_ = { 0.0f,0.0f,0.0f };

	float targetRadius_ = 20.0f;

	float yaw_ = 0.0f;      // 左右
	float pitch_ = 0.0f;    // 上下

	// 壁の透明化フラグ
	bool isTransparent_ = false;
	float wallAlpha_;
	float alpha_ = 0.2f;
};
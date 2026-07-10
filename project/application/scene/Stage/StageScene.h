#pragma once
#include "IScene.h"
#include "Camera.h"
#include "Object3d.h"
#include  "Ui.h"

#include "rubikCube/RubikCube.h"
#include "stageClear/StageClear.h"
#include <WallData.h>

#include <memory>
#include <vector>
#include <string>

// ゲーム本編（ステージ）
// ここに自分の処理を足していく（カメラだけ持った最小の雛形）。
class StageScene : public IScene {
public:
	void Initialize() override;
	void Update() override;
	void Finalize() override;
	void Object3DDraw() override;
	void SpriteDraw() override;
	void ImGuiDraw() override;
	void ParticleDraw() override;
	TuboEngine::Camera* GetMainCamera() const override { return camera_.get(); }
	//キューブの回転アニメーション
	void CubeAnimation();
	//ポーズメニューでのシーン切り替え
	void ChangeSceneFromPause();

	//壁(穴つき)オブジェクトを WallData から生成
	void BuildWalls();
	//セルサイズ等の変更時に壁データと描画を作り直す
	void RebuildStage();
	//クリア判定とクリア時のシーン遷移
	void CheckClear();
	//注視点(キューブ中心)を回るオービットカメラの更新
	void UpdateCamera();

	//レベルエディター(ImGui で壁CSVを直接編集)
	void DrawEditor();
	//キューブ先端エディター(ImGui でキューブCSVを直接編集)
	void DrawCubeEditor();
	//展開図(十字)上の有効セルか(6面のいずれかに属するか)
	bool IsValidCell(int x, int y) const;
	//現在の csvData_ を stagePath_ に保存
	void SaveStage();
	//現在の cubeCsvData_ を cubeStagePath_ に保存
	void SaveCubeStage();
	//cubeCsvData_ をキューブの先端配置として適用
	void ApplyCubeCsv();

private:
	std::unique_ptr<TuboEngine::Camera> camera_;
	std::unique_ptr<TuboEngine::Object3d>pauseMenuCube_;//ポーズメニューキューブ
	std::unique_ptr<Ui>ui_;//UIクラス

	// --- パズル本体 ---
	std::unique_ptr<RubikCube> rubikCube_;                            // 先端つきキューブ(他者作成、getterのみ利用)
	std::vector<std::unique_ptr<TuboEngine::Object3d>> wallObjects_;  // 壁(穴つき)描画
	std::vector<std::vector<int>> csvData_;                           // 壁CSV(エディターで編集)
	std::vector<std::vector<int>> cubeCsvData_;                       // キューブ先端CSV(エディターで編集)
	std::vector<WallData> wallData_;                                  // 壁配置情報(StageBuilder由来)
	SixCube required_{};                                              // 先端が必要な位置
	bool cleared_ = false;                                            // クリア済みフラグ
	bool editorEnabled_ = true;                                       // クリア判定を止めて編集に集中
	bool rebuildRequested_ = false;                                   // 次フレームで壁を作り直す(描画中の破棄回避)
	bool applyCubeState_ = false;                                     // 次フレームでキューブ先端をCSVから再適用
	std::string stagePath_ = "Resources/4209_stages/stage1.csv";     // 壁CSVのパス
	std::string cubeStagePath_ = "Resources/4209_stages/cube1.csv";  // キューブCSVのパス

	// キューブ1辺のマス数
	static constexpr int kCubeSize = 3;
	// 壁生成パラメータ(先端 ±1 の外側に穴が来るよう調整)
	// RubikCube はグリッド間隔1・OneCube(2ユニット)を0.3倍で配置しているため、
	// 壁グリッドも間隔1(cellSize=1)に合わせる。
	float stageCellSize_ = 1.0f;
	float wallThickness_ = 0.2f;
	float cubeMargin_ = 0.0f;
	// 壁/穴モデルは実寸2ユニット。タイル同士が重ならないよう cellSize より
	// 小さめの見た目にする(=一定間隔あく)。グリッド間隔(cellSize)とは分離。
	float wallScale_ = 0.4f;

	// --- オービットカメラ ---
	TuboEngine::Math::Vector3 camTarget_ = { 0.0f, 0.0f, 0.0f };
	float camYaw_ = 0.6f;
	float camPitch_ = 0.5f;
	float camDistance_ = 12.0f;
	float camRotateSpeed_ = 0.03f;

	const float PI = 3.1415926f;
	const float DEG90 = PI / 2.0f;

	float cubeBaseScale_ = 1.5f;
	float cubeScale_ = 0.0f;
	float basecubeAngle_ = 0.0f;
	bool prevRotating_ = false;
};
#pragma once
#include "IScene.h"
#include "Camera.h"
#include "Object3d.h"
#include  "Ui.h"
#include "TutorialUI.h"

#include "rubikCube/RubikCube.h"
#include "stageClear/StageClear.h"
#include <WallData.h>

#include <memory>
#include <vector>
#include <string>

// ゲーム本編（ステージ）
// ここに自分の処理を足していく（カメラだけ持った最小の雛形）。
class TutorialScene : public IScene {
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
	//カメラ回転(WASDで回転・QEでズーム。SatouSceneのカメラを移植)
	void CameraRotation();
	//マウスドラッグでルービックキューブの面(スライス)を回す
	void MouseCubeControl();
	//マウスで回るスライスのプレビューと回転フラッシュを画面に描く
	void DrawMouseGuide();
	//クリア状態・操作状況を分かりやすく表示するHUD
	void DrawHud();

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
	//指定番号のステージ(壁+キューブCSV)を読み込む
	void LoadStage(int index);

	void CubeMarginChange();

	// チュートリアルステップ
	enum class TutorialState {
		CameraMove,    // WASDでカメラ回転
		CameraZoom,    // QEでズーム
		CubeRotate,    // ドラッグでキューブ回転
		Transparent,   // Tで壁を透明化
		Distance,      // 壁の距離を調整
		Goal,          // ゴールへ
		Clear
	};

	struct TutorialStep
	{
		TutorialState action;;

		TutorialUI::GuideType guide;

		int targetCount;

		int currentCount;

		bool completed;
	};

	
	std::vector<TutorialStep> tutorialSteps_;
	int currentStep_ = 0;

	void UpdateTutorial();

	void UpdateCameraMoveTutorial();

	void UpdateZoomTutorial();

	void UpdateCubeRotateTutorial();

	void UpdateTransparentTutorial();

	void UpdateDistanceTutorial();

	void UpdateGoalTutorial();

	void UpdateClearTutorial();

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
	SixCube prevCubeState_{};                                         // 前フレームのキューブ状態(操作検知用)
	bool prevCubeStateValid_ = false;                                 // prevCubeState_ が有効か
	int moveCount_ = 0;                                               // キューブ状態が変化した回数(操作が効いた回数)
	std::string stagePath_ = "Resources/4209_stages/tutorialStage.csv";     // 壁CSVのパス
	std::string cubeStagePath_ = "Resources/4209_stages/tutorialCube.csv";  // キューブCSVのパス
	int stageIndex_ = 1;                                             // 現在のステージ番号
	static constexpr int kStageCount = 2;                            // 用意されているステージ数

	// --- マウスによるキューブ回転(3Dピッキング) ---
	bool dragging_ = false;      // 左ドラッグ中か
	float dragStartX_ = 0.0f;    // ドラッグ開始スクリーン位置
	float dragStartY_ = 0.0f;
	float dragAccumX_ = 0.0f;    // 未処理のドラッグ量
	float dragAccumY_ = 0.0f;
	float rotateFlash_ = 0.0f;   // 回転フィードバックの残りフレーム
	bool pickValid_ = false;     // カーソル下にブロックがあるか
	int pickCell_[3] = { 0,0,0 }; // 指しているブロックのセル座標(-1..1)
	int pickNormal_[3] = { 0,0,1 }; // 指している面の法線
	int dragPickCell_[3] = { 0,0,0 };   // ドラッグ開始時のブロック
	int dragPickNormal_[3] = { 0,0,1 }; // ドラッグ開始時の面法線

	//マウス光線でキューブのブロック(面)を拾う。取れたら true。
	bool PickBlock(float mx, float my, int cell[3], int normal[3]);

	// キューブ1辺のマス数
	static constexpr int kCubeSize = 3;
	// 壁生成パラメータ(先端 ±1 の外側に穴が来るよう調整)
	// RubikCube はグリッド間隔1・OneCube(2ユニット)を0.3倍で配置しているため、
	// 壁グリッドも間隔1(cellSize=1)に合わせる。
	float stageCellSize_ = 1.0f;   // グリッド間隔
	float wallThickness_ = 3.0f;   // 壁の押し出し距離(surfaceOffset に加算)
	float cubeMargin_ = 0.0f;      // キューブと壁の追加距離
	// 壁/穴モデルは実寸2ユニット。タイル同士が重ならないよう cellSize より
	// 小さめの見た目にする(=一定間隔あく)。グリッド間隔(cellSize)とは分離。
	float wallScale_ = 0.5f;       // タイルの見た目サイズ

	// --- オービットカメラ(SatouScene から移植: A/D=ヨー W/S=ピッチ Q/E=ズーム) ---
	TuboEngine::Math::Vector3 target_ = { 0.0f, 0.0f, 0.0f }; // 注視点
	float targetRadius_ = 20.0f;                             // 注視点からの距離
	float yaw_ = 0.0f;                                       // 左右
	float pitch_ = 0.0f;                                     // 上下

	const float PI = 3.1415926f;
	const float DEG90 = PI / 2.0f;

	float cubeBaseScale_ = 1.5f;
	float cubeScale_ = 0.0f;
	float basecubeAngle_ = 0.0f;
	bool prevRotating_ = false;

	
	std::unique_ptr<TutorialUI> tutorialUI_;

	// 壁の透明化フラグ
	bool isTransparent_ = false;
	float wallAlpha_;
	float alpha_ = 0.2f;
};
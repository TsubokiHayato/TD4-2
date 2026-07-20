#pragma once
#include "IScene.h"
#include "Camera.h"
#include "rubikCube/RubikCube.h"
#include <array>
#include <memory>

namespace TuboEngine {
class TextObject; // 前方宣言（メニュー項目/カーソルの生ポインタを保持するだけ）
}

// タイトル画面（ここから開始）
// メニュー（ゲームスタート / 設定 / 終了）をキーボード＋パッドで操作する。
// 見た目・配置はすべて Resources/Text/Title.json 側で定義する。
class TitleScene : public IScene {
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
	// メニュー項目（Title.json の name と対応）
	enum MenuItem {
		kMenuStart = 0, // "menu_start" ゲームスタート → STAGE へ
		kMenuOption,    // "menu_option" 設定（プレースホルダ：専用シーン未実装）
		kMenuExit,      // "menu_exit" 終了 → アプリ終了
		kMenuCount,
	};

	// 選択状態を見た目（色・カーソル位置）へ反映する。
	void ApplySelection();
	// 現在の選択項目を決定（Enter/A）したときの処理。
	void DecideSelection();
	// 上下入力（キーボード＋パッド）のトリガーを取る。戻り値: -1=上, +1=下, 0=なし。
	int TakeVerticalInput() const;
	// 決定入力（Enter/Space/Aボタン）のトリガー。
	bool TakeDecideInput() const;

	std::unique_ptr<TuboEngine::Camera> camera_;

	
	std::unique_ptr<TuboEngine::Object3d> background;//背景オブジェクト

	// 背景で自動回転し続けるルービックキューブ（タイトル演出）
	std::unique_ptr<RubikCube> rubikCube_;
	int autoRotateTimer_ = 0; // 次の自動回転までの残りフレーム

	// Title.json からロードしたメニュー本体（TextManager 所有。ここは参照用の生ポインタ）
	std::array<TuboEngine::TextObject*, kMenuCount> menuItems_{};
	TuboEngine::TextObject* cursor_ = nullptr;

	int selected_ = kMenuStart;
};

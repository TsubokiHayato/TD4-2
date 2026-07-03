#include "TitleScene.h"
#include "GameScenes.h"
#include "SceneManager.h" // シーン遷移を使うとき用
#include "TextManager.h"
#include "TextObject.h"
#include "Input.h"   // キーボード / パッド入力
#include <Windows.h> // PostQuitMessage（終了）

using namespace TuboEngine;

namespace {
// 選択中／非選択のメニュー文字色
constexpr Math::Vector4 kColorNormal   = {1.0f, 1.0f, 1.0f, 1.0f};
constexpr Math::Vector4 kColorSelected = {1.0f, 0.85f, 0.2f, 1.0f};

// カーソル「▶」を選択項目の中心からどれだけ左に置くか（Title.json の cursor 位置と揃える）
constexpr float kCursorOffsetX = 175.0f;

// Title.json 内のメニュー項目 name（enum MenuItem の並びと対応）
constexpr const char* kMenuNames[] = {"menu_start", "menu_option", "menu_exit"};
} // namespace

void TitleScene::Initialize() {
	// 最低限のカメラ
	camera_ = std::make_unique<TuboEngine::Camera>();
	camera_->SetTranslate({0.0f, 0.0f, -15.0f});
	camera_->setRotation({0.0f, 0.0f, 0.0f});
	camera_->setScale({1.0f, 1.0f, 1.0f});
	camera_->Update();

	auto* tm = TextManager::GetInstance();

	// タイトル画面の見た目（タイトル・メニュー・カーソル・ヒント・バージョン）はすべて JSON で定義。
	// ImGui の TextManager パネルで位置や色を調整でき、Save で書き戻せる。
	tm->LoadTextLayout("Resources/Text/Title.json");

	// 選択で色替え／移動する要素だけ、name で引いて生ポインタを保持しておく。
	for (int i = 0; i < kMenuCount; ++i) {
		menuItems_[i] = tm->GetTextByName(kMenuNames[i]);
	}
	cursor_ = tm->GetTextByName("cursor");

	selected_ = kMenuStart;
	ApplySelection();
}

void TitleScene::Update() {
	camera_->Update();

	// メニュー移動（上下）
	if (int dir = TakeVerticalInput(); dir != 0) {
		selected_ = (selected_ + dir + kMenuCount) % kMenuCount; // 端でループ
		ApplySelection();
	}

	// 決定
	if (TakeDecideInput()) {
		DecideSelection();
	}

	TuboEngine::TextManager::GetInstance()->UpdateAll();
}

void TitleScene::ApplySelection() {
	for (int i = 0; i < kMenuCount; ++i) {
		if (menuItems_[i]) {
			menuItems_[i]->SetColor(i == selected_ ? kColorSelected : kColorNormal);
		}
	}
	// カーソルは選択中の項目の左隣へ移動する（項目位置は JSON 由来）。
	if (cursor_ && menuItems_[selected_]) {
		const Math::Vector2 itemPos = menuItems_[selected_]->GetPosition();
		cursor_->SetPosition({itemPos.x - kCursorOffsetX, itemPos.y});
	}
}

void TitleScene::DecideSelection() {
	switch (selected_) {
	case kMenuStart:
		SceneManager::GetInstance()->ChangeScene(STAGE); // 次フレームでゲーム本編へ
		break;
	case kMenuOption:
		SceneManager::GetInstance()->ChangeScene(OPTION); // 設定画面へ
		break;
	case kMenuExit:
		PostQuitMessage(0); // アプリ終了（WM_QUIT）
		break;
	default:
		break;
	}
}

int TitleScene::TakeVerticalInput() const {
	auto* in = Input::GetInstance();

	bool up = in->TriggerKey(DIK_UP) || in->TriggerKey(DIK_W);
	bool down = in->TriggerKey(DIK_DOWN) || in->TriggerKey(DIK_S);

	// パッド（XInput）の十字キー。押した瞬間だけ拾う（前フレームとの差分）。
	XINPUT_STATE cur{}, prev{};
	if (in->GetJoystickState(0, cur) && in->GetJoystickStatePrevious(0, prev)) {
		auto trigger = [&](WORD mask) {
			return (cur.Gamepad.wButtons & mask) && !(prev.Gamepad.wButtons & mask);
		};
		if (trigger(XINPUT_GAMEPAD_DPAD_UP)) up = true;
		if (trigger(XINPUT_GAMEPAD_DPAD_DOWN)) down = true;
	}

	if (up && !down) return -1;
	if (down && !up) return +1;
	return 0;
}

bool TitleScene::TakeDecideInput() const {
	auto* in = Input::GetInstance();

	bool decide = in->TriggerKey(DIK_RETURN) || in->TriggerKey(DIK_NUMPADENTER) || in->TriggerKey(DIK_SPACE);

	XINPUT_STATE cur{}, prev{};
	if (in->GetJoystickState(0, cur) && in->GetJoystickStatePrevious(0, prev)) {
		auto trigger = [&](WORD mask) {
			return (cur.Gamepad.wButtons & mask) && !(prev.Gamepad.wButtons & mask);
		};
		if (trigger(XINPUT_GAMEPAD_A) || trigger(XINPUT_GAMEPAD_START)) decide = true;
	}

	return decide;
}

void TitleScene::Finalize() {
	// タイトルの UI を次シーンへ持ち越さないよう片付ける。
	TuboEngine::TextManager::GetInstance()->ClearAllTexts();
	TuboEngine::TextManager::GetInstance()->ClearAllSprites();
	menuItems_.fill(nullptr);
	cursor_ = nullptr;
}

void TitleScene::Object3DDraw() {} // TODO: 3Dオブジェクト描画
void TitleScene::SpriteDraw() { TuboEngine::TextManager::GetInstance()->DrawAll(); } // 2Dスプライト描画
void TitleScene::ImGuiDraw() { TuboEngine::TextManager::GetInstance()->DrawImGui(); } // ImGui描画
void TitleScene::ParticleDraw() {}                                                    // TODO: パーティクル描画

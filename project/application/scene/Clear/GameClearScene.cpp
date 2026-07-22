#include "GameClearScene.h"
#include "GameScenes.h"
#include "SceneManager.h" // シーン遷移を使うとき用
#include "TextManager.h"
#include "Input.h"              // キーボード / パッド入力
#include "audio/AudioManager.h" // BGM / SE
#include "ParticleManager.h"    // 紙吹雪
#include "IParticleEmitter.h"   // ParticlePreset 構造体
#include <Xinput.h>
#include <cmath>   // sin, cos
#include <cstdlib> // rand

using namespace TuboEngine;

namespace {
// ターンテーブルカメラのパラメータ（キューブ中心＝原点を周回する）。
constexpr float kOrbitPitch = 0.42f;   // 見下ろし角（一定）
constexpr float kOrbitRadius = 7.0f;   // キューブからの距離
constexpr float kOrbitSpeed = 0.006f;  // 1フレームあたりの周回角（ゆっくり）
// キューブ全体の自転（カメラ周回とは逆向き＆速めにして、自転がはっきり見えるように）。
constexpr float kCubeSpinSpeed = -0.018f;
} // namespace

void GameClearScene::Initialize() {
	// 原点(キューブ中心)を周回するカメラ。初期角から少し見下ろす。
	camera_ = std::make_unique<TuboEngine::Camera>();
	camera_->setScale({1.0f, 1.0f, 1.0f});
	// 位置は Update の周回計算で毎フレーム設定する（ここでは器だけ用意）。
	camera_->Update();

	// 背景スカイボックス。
	background_ = std::make_unique<TuboEngine::Object3d>();
	background_->Initialize("skyBox/skyBox.obj");
	background_->SetCamera(camera_.get());
	background_->Update();

	// クリア演出: 画面中央で面を次々に回し続けるルービックキューブ。
	rubikCube_ = std::make_unique<RubikCube>();
	rubikCube_->Initialize(camera_.get());
	rubikCube_->SetGuideVisible(false); // 操作用ガイド／矢印は見せない（重なり防止）
	autoRotateTimer_ = 0;

	// 「ゲームクリア！」などの文字は JSON で定義（ImGui の TextManager で調整可）。
	TextManager::GetInstance()->LoadTextLayout("Resources/Text/Clear.json");

	// クリアのファンファーレを1回鳴らす。
	AudioManager::GetInstance()->PlaySe("fanfare.wav");

	// 上から降ってくる金色の紙吹雪（四角い粒）。
	// Default エミッターは重力を0に固定するので、落下は下向きの初速で表現する。
	ParticlePreset preset;
	preset.name = "ClearConfetti";
	preset.texture = "yellow.png"; // 不透明な黄色い正方形＝紙片
	preset.maxInstances = 512;
	preset.billboard = true;
	preset.autoEmit = true;
	preset.emitRate = 60.0f; // 賑やかに
	preset.center = {0.0f, 6.0f, 0.0f};
	preset.posMin = {-7.0f, 0.0f, -2.0f};
	preset.posMax = {7.0f, 2.0f, 2.0f};
	preset.velMin = {-0.6f, -2.6f, -0.2f}; // 下向き＋左右に少し散らす
	preset.velMax = {0.6f, -1.6f, 0.2f};
	preset.lifeMin = 3.0f;
	preset.lifeMax = 5.0f;
	preset.scaleStart = {0.12f, 0.12f, 0.12f};
	preset.scaleEnd = {0.10f, 0.10f, 0.10f}; // ほぼ同じ大きさで四角を保つ
	preset.colorStart = {1.0f, 1.0f, 1.0f, 1.0f};
	preset.colorEnd = {1.0f, 1.0f, 1.0f, 0.0f}; // 最後にフェードアウト
	if (IParticleEmitter* e = ParticleManager::GetInstance()->CreateEmitterByType("Default", preset))
		particleName_ = e->GetName();
}

void GameClearScene::Update() {
	const float dt = 1.0f / 60.0f;
	elapsed_ += dt;

	// ターンテーブル演出: カメラが原点(キューブ)の周りをぐるっと周回する。
	orbitYaw_ += kOrbitSpeed;
	{
		Math::Vector3 pos;
		pos.x = kOrbitRadius * std::cos(kOrbitPitch) * std::sin(orbitYaw_);
		pos.y = kOrbitRadius * std::sin(kOrbitPitch);
		pos.z = kOrbitRadius * std::cos(kOrbitPitch) * std::cos(orbitYaw_);
		camera_->SetTranslate(pos);
		camera_->setRotation({kOrbitPitch, orbitYaw_ + 3.141592f, 0.0f});
	}
	camera_->Update();
	background_->SetCamera(camera_.get());
	background_->Update();

	// キューブ全体を自転させる（描画専用の見た目回転。カメラ周回と合わせて二重に回る）。
	cubeSpin_ += kCubeSpinSpeed;
	if (rubikCube_)
		rubikCube_->SetWholeSpinYaw(cubeSpin_);

	// 面回転は「たまのアクセント」に留める（カメラの周回が主役なので控えめに）。
	if (rubikCube_) {
		if (!rubikCube_->IsRotating()) {
			if (autoRotateTimer_ > 0) {
				autoRotateTimer_--;
			} else {
				int axis = rand() % 3;
				int row = rand() % 3;
				int dir = rand() % 2;
				rubikCube_->RequestRotation(axis, row, dir);
				autoRotateTimer_ = 45; // 次の面回転まで少し間を空ける
			}
		}
		rubikCube_->Update();
	}

	// パーティクルはシーンが駆動する。
	ParticleManager::GetInstance()->Update(dt, camera_.get());

	// 表示直後の誤爆を避けるため、0.5秒経ってから決定入力を受け付ける。
	if (elapsed_ > 0.5f && TakeDecideInput()) {
		AudioManager::GetInstance()->PlaySe("decide.mp3");
		// タイトル BGM へ戻し、タイトルシーンへ。
		AudioManager::GetInstance()->PlayBgm("title.wav");
		SceneManager::GetInstance()->ChangeScene(TITLE);
	}

	TextManager::GetInstance()->UpdateAll();
}

void GameClearScene::Finalize() {
	// このシーンの UI とパーティクルを次シーンへ持ち越さない。
	TextManager::GetInstance()->ClearAllTexts();
	TextManager::GetInstance()->ClearAllSprites();
	if (!particleName_.empty()) {
		ParticleManager::GetInstance()->Remove(particleName_);
		particleName_.clear();
	}
}

void GameClearScene::Object3DDraw() {
	background_->Draw();
	if (rubikCube_)
		rubikCube_->Draw();
}
void GameClearScene::SpriteDraw() { TextManager::GetInstance()->DrawAll(); }
void GameClearScene::ImGuiDraw() { TextManager::GetInstance()->DrawImGui(); }
void GameClearScene::ParticleDraw() { ParticleManager::GetInstance()->Draw(); }

// 決定入力（Enter/Space/NumpadEnter、パッドの A / START）のトリガー。
bool GameClearScene::TakeDecideInput() const {
	auto* in = Input::GetInstance();

	bool decide = in->TriggerKey(DIK_RETURN) || in->TriggerKey(DIK_NUMPADENTER) || in->TriggerKey(DIK_SPACE);

	XINPUT_STATE cur{}, prev{};
	if (in->GetJoystickState(0, cur) && in->GetJoystickStatePrevious(0, prev)) {
		auto trigger = [&](WORD mask) { return (cur.Gamepad.wButtons & mask) && !(prev.Gamepad.wButtons & mask); };
		if (trigger(XINPUT_GAMEPAD_A) || trigger(XINPUT_GAMEPAD_START))
			decide = true;
	}

	return decide;
}

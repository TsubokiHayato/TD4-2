#include "Order.h"

#include "SceneManager.h"
#include "SceneRegistration.h"
#include "GameScenes.h"

void TuboEngine::Order::Initialize() {
	// エンジン基盤の初期化（シーンには触れない）
	TuboEngine::Framework::Initialize();

	// ゲームのシーンを登録してから開始シーンを指定（登録は Initialize より前）
	RegisterGameScenes();
	SceneManager::GetInstance()->Initialize(SAMPLE);
}

void TuboEngine::Order::Update() {
	TuboEngine::Framework::Update();
}

void TuboEngine::Order::Finalize() {
	TuboEngine::Framework::Finalize();
}

void TuboEngine::Order::Draw() {
	TuboEngine::Framework::FrameWorkRenderTargetPreDraw();
	TuboEngine::Framework::Object3dCommonDraw();
	TuboEngine::Framework::SpriteCommonDraw();
	LineManager::GetInstance()->Draw();
	TuboEngine::Framework::ParticleCommonDraw();
	TuboEngine::Framework::FrameworkSwapChainPreDraw();
	TuboEngine::Framework::OffScreenRenderingDraw();
#ifdef USE_IMGUI
	Framework::ImguiPreDraw();
	Framework::ImguiPostDraw();
#endif
	Framework::FrameworkSwapChainPostDraw();
}

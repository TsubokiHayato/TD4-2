#include "SceneRegistration.h"

#include "SceneManager.h"
#include "GameScenes.h"
#include "SampleScene.h"
#include"TitleScene.h"
#include"Stage/StageScene.h"
#include"Clear/GameClearScene.h"
#include"Over/GameOverScene.h"
#include"User/User1Scene.h"
#include"User/User2Scene.h"
#include"User/User3Scene.h"
#include"User/User4Scene.h"

#include <memory>

void RegisterGameScenes() {
	SceneManager* sm = SceneManager::GetInstance();
	// シーン番号 → 生成関数 を登録。増やすときはここに足す。
	sm->RegisterScene(SAMPLE, [] { return std::make_unique<SampleScene>(); }, "Sample");
	sm->RegisterScene(TITLE, [] { return std::make_unique<TitleScene>(); }, "Title");
	sm->RegisterScene(STAGE, [] { return std::make_unique<StageScene>(); }, "Stage");
	sm->RegisterScene(CLEAR, [] { return std::make_unique<GameClearScene>(); }, "GameClear");
	sm->RegisterScene(OVER, [] { return std::make_unique<GameOverScene>(); }, "GameOver");
	sm->RegisterScene(USER1, [] { return std::make_unique<User1Scene>(); }, "User1");
	sm->RegisterScene(USER2, [] { return std::make_unique<User2Scene>(); }, "User2");
	sm->RegisterScene(USER3, [] { return std::make_unique<User3Scene>(); }, "User3");
	sm->RegisterScene(USER4, [] { return std::make_unique<User4Scene>(); }, "User4");
}

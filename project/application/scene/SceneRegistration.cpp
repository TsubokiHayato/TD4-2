#include "SceneRegistration.h"

#include "SceneManager.h"
#include "GameScenes.h"
#include "Sample/SampleScene.h"
#include"Title/TitleScene.h"
#include"Stage/StageScene.h"
#include"Clear/GameClearScene.h"
#include"Over/GameOverScene.h"
#include"User/SatouScene.h"
#include"User/OosakiScene.h"
#include"User/SanoScene.h"
#include"User/TuboScene.h"
#include"Option/OptionScene.h"

#include <memory>

void RegisterGameScenes() {
	SceneManager* sm = SceneManager::GetInstance();
	// シーン番号 → 生成関数 を登録。増やすときはここに足す。
	sm->RegisterScene(SAMPLE, [] { return std::make_unique<SampleScene>(); }, "Sample");
	sm->RegisterScene(TITLE, [] { return std::make_unique<TitleScene>(); }, "Title");
	sm->RegisterScene(STAGE, [] { return std::make_unique<StageScene>(); }, "Stage");
	sm->RegisterScene(CLEAR, [] { return std::make_unique<GameClearScene>(); }, "GameClear");
	sm->RegisterScene(OVER, [] { return std::make_unique<GameOverScene>(); }, "GameOver");
	sm->RegisterScene(USER1, [] { return std::make_unique<SatouScene>(); }, "Satou");
	sm->RegisterScene(Oosaki, [] { return std::make_unique<OosakiScene>(); }, "Oosaki");
	sm->RegisterScene(Sano, [] { return std::make_unique<SanoScene>(); }, "Sano");
	sm->RegisterScene(Tubo, [] { return std::make_unique<TuboScene>(); }, "Tubo");
	sm->RegisterScene(OPTION, [] { return std::make_unique<OptionScene>(); }, "Option");
}

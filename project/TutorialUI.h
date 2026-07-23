#pragma once
#include <memory>
#include <vector>
#include <string>
#include <sprite.h>


using TuboEngine::Math::Vector2;

class TutorialUI
{
public:
    struct GuideAnimation
    {
        std::unique_ptr<TuboEngine::Sprite> sprite = nullptr;

		Vector2 position{};
		Vector2 size{};

        float scale = 0.0f;

        float alpha = 0.0f;

        bool appearing = false;

        bool disappearing = false;

		bool visible = false;

        float timer = 0.0f;
    };

	enum class GuideType
	{
		CameraRotate,
		CameraZoom,
		CubeRotate,
		Transparent,
		Distance,
		Goal,
		Success,
	};
	/*------メンバ関数------*/
	//TutorialUI();

	// 初期化
	void Initialize();

	// 更新
	void Update();

	// 描画
	void Draw();

	// ImGuiの描画
	void DrawImGui();

	void UpdateAnimation(GuideAnimation& anim);

	void Appear(GuideAnimation& anim);

	void Disappear(GuideAnimation& anim);


	float easeInElastic(float x);

	void ShowGuide(GuideType type);

	void ShowSuccess();

	// ゲージの進行度を設定する
	void SetGauge(int current, int target){ targetGaugeRate_ =target == 0 ? 0.0f :static_cast<float>(current) / target; }

private:
	/*------メンバ変数------*/
	
	std::vector<GuideAnimation> guides_;

	Vector2 center_ = { 640.0f,360.0f };//画面の中心座標

	std::unique_ptr<TuboEngine::Sprite> gauge_;
	// ゲージの位置
	Vector2 gaugePosition_ = { 300.0f, 600.0f };

	// ゲージのデフォルトサイズ
	const Vector2 gaugeDefaultSize_ = { 640.0f, 32.0f };

	float gaugeRate_ = 0.0f;
	float targetGaugeRate_ = 0.0f;

	GuideAnimation success_;

	GuideType currentGuide_ = GuideType::CameraRotate;
	bool hasCurrentGuide_ = false;
};


#include "TutorialUI.h"
#include <corecrt_math_defines.h>
void TutorialUI::Initialize()
{
    guides_.resize(static_cast<size_t>(GuideType::Success));

    guides_[0].sprite = std::make_unique<TuboEngine::Sprite>();
    guides_[1].sprite = std::make_unique<TuboEngine::Sprite>();
    guides_[2].sprite = std::make_unique<TuboEngine::Sprite>();
    guides_[3].sprite = std::make_unique<TuboEngine::Sprite>();
    guides_[4].sprite = std::make_unique<TuboEngine::Sprite>();
    guides_[5].sprite = std::make_unique<TuboEngine::Sprite>();
    for (auto& guide : guides_)
    {
        guide.sprite->SetAnchorPoint({ 0.5f, 0.5f });
        guide.scale = 0.0f;
        guide.alpha = 0.0f;
        guide.visible = false;
    }
	guides_[0].sprite->Initialize("cameraRotateTutorial.png");
	guides_[1].sprite->Initialize("zoomTutorial.png");
	guides_[2].sprite->Initialize("cubeRotateTutorial.png");
	guides_[3].sprite->Initialize("transparentTutorial.png");
	guides_[4].sprite->Initialize("distanceTutorial.png");
    guides_[5].sprite->Initialize("clearTutorial.png");

    guides_[0].position = { 640,100 };
    guides_[1].position = { 640,100 };
    guides_[2].position = { 640,100 };
    guides_[3].position = { 640,100 };
    guides_[4].position = { 640,100 };
	guides_[5].position = { 640,100 };

    guides_[0].size = { 1017,71 };
	guides_[1].size = { 870,142 };
	guides_[2].size = { 1122,71 };
	guides_[3].size = { 1122,71 };
	guides_[4].size = { 1122,71 };
    guides_[5].size = { 1052,142 };

	success_.sprite = std::make_unique<TuboEngine::Sprite>();
	success_.sprite->Initialize("success.png");

    success_.position = center_;
    success_.size = { 130,70 };

    success_.scale = 0.0f;
    success_.alpha = 0.0f;
    success_.visible = false;

    gauge_ = std::make_unique<TuboEngine::Sprite>();
    gauge_->Initialize("yellow.png");
}

void TutorialUI::Update()
{
    for (auto& guide : guides_)
    {
        UpdateAnimation(guide);
    }

    UpdateAnimation(success_);

    gaugeRate_ += (targetGaugeRate_ - gaugeRate_) * 0.15f;

    gauge_->SetPosition(gaugePosition_);

    gauge_->SetAnchorPoint({ 0.0f,0.5f });

    gauge_->SetSize({
        gaugeDefaultSize_.x * gaugeRate_,
        gaugeDefaultSize_.y
        });
    gauge_->Update();
}

void TutorialUI::Draw()
{
    for (auto& guide : guides_)
    {
        if (guide.visible)
        {
            guide.sprite->Draw();
        }
    }

    if (success_.visible)
    {
        success_.sprite->Draw();
    }

    gauge_->Draw();
}

void TutorialUI::DrawImGui()
{

}

void TutorialUI::UpdateAnimation(GuideAnimation & anim)
{
    if (anim.appearing)
    {
        anim.timer += 1.0f / 60.0f;

        float t = std::clamp(anim.timer / 0.3f, 0.0f, 1.0f);

        anim.scale = easeInElastic(t);

        anim.alpha = t;

        if (t >= 1.0f)
        {
            anim.appearing = false;
        }
    }
    if (anim.disappearing)
    {
        anim.timer += 1.0f / 60.0f;

        float t = std::clamp(anim.timer / 0.25f, 0.0f, 1.0f);

        anim.scale = 1.0f - t;

        anim.alpha = 1.0f - t;

        if (t >= 1.0f)
        {
            anim.disappearing = false;
            anim.visible = false;
        }
    }
    anim.sprite->SetPosition(anim.position);
    anim.sprite->SetSize({anim.size.x * anim.scale,anim.size.y * anim.scale});

    anim.sprite->SetColor({1.0f,1.0f,1.0f,anim.alpha});
    anim.sprite->Update();
}

void TutorialUI::Appear(GuideAnimation & anim)
{
    anim.appearing = true;
    anim.disappearing = false;
    anim.visible = true;
    anim.timer = 0.0f;
}

void TutorialUI::Disappear(GuideAnimation & anim)
{
    anim.disappearing = true;
    anim.appearing = false;
    anim.timer = 0.0f;
}

float TutorialUI::easeInElastic(float x)
{
    const float c4 = (2.0f * (float)M_PI) / 3.0f;

    return x == 0.0f
        ? 0.0f
        : x == 1.0f
        ? 1.0f
        : -powf(2.0f, 10.0f * x - 10.0f) * sinf((x * 10.0f - 10.75f) * c4);
}

void TutorialUI::ShowGuide(GuideType type)
{
    if (hasCurrentGuide_ &&
        currentGuide_ == type)
    {
        return;
    }

    if (hasCurrentGuide_)
    {
        Disappear(guides_[int(currentGuide_)]);
    }

    currentGuide_ = type;
    hasCurrentGuide_ = true;

    Appear(guides_[int(type)]);
}

void TutorialUI::ShowSuccess()
{
    Appear(success_);
}

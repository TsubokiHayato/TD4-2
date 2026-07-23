#include "ClearEffect.h"
#include "ParticleManager.h"

void ClearEffect::Initialize() {
	ParticlePreset preset;
	preset.name = name;
	preset.texture = "clearParticle.png"; // "Resources/Textures/" から
	preset.maxInstances = 256;
	preset.billboard = true;         // 常にカメラを向く板ポリ
	preset.autoEmit = true;          // 自動で出し続ける
	preset.emitRate = 8.0f;         // 1秒あたりの放出数
	preset.center = { 0.0f, 0.0f, 0.0f };

	const float kPos = 3.0f;
	preset.posMin = { -kPos, -kPos, -kPos };
	preset.posMax = { kPos, kPos, kPos };

	const float kVel = 0.2f;
	preset.velMin = { -kVel, -kVel, -kVel };
	preset.velMax = { kVel, kVel, kVel };
	preset.lifeMin = 0.6f;
	preset.lifeMax = 1.4f;
	preset.gravity = { 0.0f, 0.0f, 0.0f };
	preset.scaleStart = { 0.5f, 0.5f, 0.5f };
	preset.scaleEnd = { 1.0f, 1.0f, 1.0f };
	preset.colorStart = { 0.5f, 0.5f, 0.0f, 1.0f };
	preset.colorEnd = { 0.5f, 0.5f, 0.0f, 0.0f }; // だんだん透明に
	if (IParticleEmitter* e = TuboEngine::ParticleManager::GetInstance()->CreateEmitterByType("Default", preset)) {
		name = e->GetName(); // 片付け用に名前を覚えておく
	}
}

void ClearEffect::Finalize() {
	TuboEngine::ParticleManager::GetInstance()->Remove(name);
}

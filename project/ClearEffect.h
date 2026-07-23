#pragma once
#include "Particle.h"

/// <summary>
/// 
/// </summary>
class ClearEffect {
public:

	void Initialize();
	// 放出中のパーティクルを片付ける(次シーンへ持ち越さないため)。
	void Finalize();

private:
	std::string name = "clear_Effect";
};


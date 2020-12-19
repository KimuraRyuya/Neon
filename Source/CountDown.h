#pragma once
#include "../Library/Billboard.h"

#include "Collision.h"
#include "OBJ3D.h"
#include "../Library/GPUParticle/EmitParticle.h"
#include "Player.h"

#include <memory>
#include <iostream>

class CountDown
{
private:

	std::unique_ptr<Billboard> countTex[4];

	float emitTimer;
	float dissolveScalar;
	int displyNum;

	bool changeNum;

	bool nowGamePlay;
	bool countStartFlg;
public:
	CountDown() {	}
	~CountDown() {}
	void Initialize();
	void Update(std::shared_ptr<Player> player, float elapsedTime);
	void Render();
	void Finalize();

	void Reset();

	// åªç›ÉQÅ[ÉÄíÜÇ©Ç«Ç§Ç©
	bool GetNowGamePlay() { return nowGamePlay; }

};
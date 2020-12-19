#pragma once
#include "CharacterSystem.h"
#include "BG.h"
#include "PlayerShot.h"
#include "Combo.h"
#include "TrackingScore.h" 
#include "Score.h"
#include "BG.h"

namespace CollisionJudge
{

	bool PlayerVSStage(std::shared_ptr<Player>player, std::shared_ptr<BG> bg);
	bool PlayerShotVSStage(PlayerShotSystem* playerShot, std::shared_ptr<BG> bg, CharacterSystem* characterSystem);
	bool PlayerVSEnemy(std::shared_ptr<Player> player, std::shared_ptr<EnemyManager> enmManager, Combo* combo);
	bool PlayerVSTrakingScore(std::shared_ptr<Player> player, TrackingScore* trackingScore);
	bool PlayerDestroyEffectVSEnemy(std::shared_ptr<Player> player, std::shared_ptr<EnemyManager> enmManager);
	bool PlayerShotVSEnemy(PlayerShotSystem* playerShot, CharacterSystem* characterSyatem, Combo* combo, Score* score, std::shared_ptr<BG> bg, TrackingScore* trackingScore, bool reinforcel);

}

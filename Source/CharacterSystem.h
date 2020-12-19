#pragma once
#include "Player.h"
#include "EnemyManager.h"

namespace Edit
{
	class EditPrameter;
}
class CharacterSystem
{
private:
	std::shared_ptr<Player> player;
	std::shared_ptr<EnemyManager> enemyManager;

	std::shared_ptr<Edit::EditPrameter> editParameter;
public:
	CharacterSystem() {}
	~CharacterSystem() { Finalize(); }
	void Initialize();
	void Update(int level, float elapsedTime);
	void TutorialUpdate(int level, float elapsedTime);
	void Render();
	void Finalize();
	void Reset();
	void SetEditParameter(std::shared_ptr<Edit::EditPrameter> ptr);
	std::shared_ptr<Player> GetPlayerAddress() { return player; }
	std::shared_ptr<EnemyManager> GetEnemyManagerAddress() { return enemyManager; }

};
#include "CharacterSystem.h"
#include "EditParameter.h"

void CharacterSystem::Initialize()
{
	player = std::make_shared<Player>();
	player->Initialize();
	enemyManager = std::make_shared<EnemyManager>();
	enemyManager->Initialize();
	
	player->SetEditPlayerParam(editParameter->playerEditParam);
}

void CharacterSystem::TutorialUpdate(int level, float elapsedTime)
{
	player->Update(level, elapsedTime);
}
void CharacterSystem::Update(int level, float elapsedTime)
{
	player->Update(level, elapsedTime);

	enemyManager->EnemyExistJudge(elapsedTime);

	// プレイヤーのスポーン中はエネミーの更新を止める
	if (player->GetNewSpawn()) return;
	enemyManager->SetTargetPos(player->GetPos());
	enemyManager->Update(level, elapsedTime);
}

void CharacterSystem::Render()
{
	enemyManager->Render();
	player->Render();
}

void CharacterSystem::Finalize()
{
	player->Finalize();
	enemyManager->Finalize();
}

void CharacterSystem::Reset()
{
	enemyManager->Reset();
	player->Reset();
}

void CharacterSystem::SetEditParameter(std::shared_ptr<Edit::EditPrameter> ptr)
{
	editParameter = ptr;
}
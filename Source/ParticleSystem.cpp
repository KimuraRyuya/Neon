#include "ParticleSystem.h"
#include <imgui.h>

void ParticleSystem::Initialize()
{
	// GPUParticleのパラメーター
	ParticleInfo particleInfo;

	// プレイヤーのデストロイParticle
	{
		playerDestroyParticle = std::make_unique<EmitParticle>();
		playerDestroyParticle->SetMaxParticleCount(100000);
		particleInfo.speed = 35.0f;
		particleInfo.size = 0.08f;
		particleInfo.scaleX = 0.0f;
		particleInfo.life = 2.0f;
		particleInfo.color = 20.0f;
		particleInfo.motionBlurAmount = 0.04f;
		playerDestroyParticle->setEmitShader("EmitParticleMeshEmitCS.cso");
		playerDestroyParticle->setParticleInfo(particleInfo);
		playerDestroyParticle->Initialize(L"Data/Texture/player_shot.png", true);
	}

	// プレイヤーのダッシュParticle
	{
		playerDashParticle = std::make_unique<EmitParticle>();
		playerDashParticle->SetMaxParticleCount(100000);
		particleInfo.speed = 0.0f;
		particleInfo.size = 0.02f;
		particleInfo.scaleX = 0.0f;
		particleInfo.life = 0.2f;
		particleInfo.color = 20.0f;
		particleInfo.motionBlurAmount = 0.0f;
		playerDashParticle->setEmitShader("EmitParticleMeshEmitCS.cso");
		playerDashParticle->setParticleInfo(particleInfo);
		playerDashParticle->Initialize(L"Data/Texture/player_shot.png", true);
	}

	// プレイヤーのスポーンParticle
	{
		playerSpawnParticle = std::make_unique<EmitParticle>();
		playerSpawnParticle->SetMaxParticleCount(100000);
		particleInfo.speed = 35.0f;
		particleInfo.size = 0.03f;
		particleInfo.scaleX = 0.0f;
		particleInfo.life = 1.0f;
		particleInfo.color = 20.0f;
		particleInfo.motionBlurAmount = 0.04f;
		playerSpawnParticle->setEmitShader("EmitParticleEmitSpawnPlayerCS.cso");
		playerSpawnParticle->setParticleInfo(particleInfo);
		playerSpawnParticle->Initialize(L"Data/Texture/player_shot.png", true);
	}

	// エネミーのデストロイParticle
	{
		destroyEmitParticle[0] = std::make_unique<EmitParticle>();
		destroyEmitParticle[2] = std::make_unique<EmitParticle>();
		destroyEmitParticle[4] = std::make_unique<EmitParticle>();

		destroyEmitParticle[0]->SetMaxParticleCount(100000);
		destroyEmitParticle[2]->SetMaxParticleCount(100000);
		destroyEmitParticle[4]->SetMaxParticleCount(100000);

		particleInfo.speed = 35.0f;
		particleInfo.size = 0.02f;
		particleInfo.scaleX = 0.0f;
		particleInfo.life = 2.0f;
		particleInfo.color = 10.0f;
		particleInfo.motionBlurAmount = 0.04f;

		destroyEmitParticle[0]->setEmitShader("EmitParticleEmitEnemyKillCS.cso");
		destroyEmitParticle[2]->setEmitShader("EmitParticleEmitEnemyKillCS.cso");
		destroyEmitParticle[4]->setEmitShader("EmitParticleEmitEnemyKillCS.cso");

		destroyEmitParticle[0]->setSimulateShader("EmitParticleEnemyKillSimulateCS.cso");
		destroyEmitParticle[2]->setSimulateShader("EmitParticleEnemyKillSimulateCS.cso");
		destroyEmitParticle[4]->setSimulateShader("EmitParticleEnemyKillSimulateCS.cso");

		destroyEmitParticle[0]->setParticleInfo(particleInfo);
		destroyEmitParticle[2]->setParticleInfo(particleInfo);
		destroyEmitParticle[4]->setParticleInfo(particleInfo);

		destroyEmitParticle[0]->Initialize(L"Data/Texture/enemy_shot.png", false);
		destroyEmitParticle[2]->Initialize(L"Data/Texture/enemy_shot2.png", false);
		destroyEmitParticle[4]->Initialize(L"Data/Texture/enemy_shot3.png", false);
	}

	// プレイヤーショットのデストロイParticle
	{
		playerShotDestroyParticle[0] = std::make_unique<EmitParticle>();
		playerShotDestroyParticle[1] = std::make_unique<EmitParticle>();
		playerShotDestroyParticle[0]->SetMaxParticleCount(100000);
		playerShotDestroyParticle[1]->SetMaxParticleCount(100000);
		particleInfo.speed = 0;
		particleInfo.size = 0.15f;
		particleInfo.life = 0.5f;
		particleInfo.motionBlurAmount = 0.0f;
		playerShotDestroyParticle[0]->setParticleInfo(particleInfo);
		particleInfo.size = 0.8f;
		playerShotDestroyParticle[1]->setParticleInfo(particleInfo);
		playerShotDestroyParticle[0]->Initialize(L"Data/Texture/player_shot.png", false);
		playerShotDestroyParticle[1]->Initialize(L"Data/Texture/player_shot.png", false);
	}
	{
		floorParticle = std::make_unique<EmitParticle>();
		floorParticle->SetMaxParticleCount(100000);
		particleInfo.speed = 1;
		particleInfo.size = 0.3f;
		particleInfo.life = 0.5f;
		particleInfo.motionBlurAmount = 0.0f;
		floorParticle->setEmitShader("EmitParticleEmitFloorCS.cso");
		floorParticle->setSimulateShader("EmitParticleEnemyKillSimulate2CS.cso");
		floorParticle->setParticleInfo(particleInfo);
		floorParticle->Initialize(L"Data/Texture/player_shot.png", false);
	}

	// エネミーのデストロイParticle2種類
	{
		destroyEmitParticle[1] = std::make_unique<EmitParticle>();
		destroyEmitParticle[3] = std::make_unique<EmitParticle>();
		destroyEmitParticle[5] = std::make_unique<EmitParticle>();

		destroyEmitParticle[1]->SetMaxParticleCount(100000);
		destroyEmitParticle[3]->SetMaxParticleCount(100000);
		destroyEmitParticle[5]->SetMaxParticleCount(100000);
		
		particleInfo.speed = 18.0f;
		particleInfo.size = 0.15f;
		particleInfo.scaleX = 0.0f;
		particleInfo.life = 2.0f;
		particleInfo.motionBlurAmount = 0.0f;
		particleInfo.rotation = 0.01f;
		particleInfo.color = 10.0f;

		destroyEmitParticle[1]->setEmitShader("EmitParticleEmitEnemyKill2CS.cso");
		destroyEmitParticle[3]->setEmitShader("EmitParticleEmitEnemyKill2CS.cso");
		destroyEmitParticle[5]->setEmitShader("EmitParticleEmitEnemyKill2CS.cso");

		destroyEmitParticle[1]->setSimulateShader("EmitParticleEnemyKillSimulate2CS.cso");
		destroyEmitParticle[3]->setSimulateShader("EmitParticleEnemyKillSimulate2CS.cso");
		destroyEmitParticle[5]->setSimulateShader("EmitParticleEnemyKillSimulate2CS.cso");

		destroyEmitParticle[1]->setParticleInfo(particleInfo);
		destroyEmitParticle[3]->setParticleInfo(particleInfo);
		destroyEmitParticle[5]->setParticleInfo(particleInfo);

		destroyEmitParticle[1]->Initialize(L"Data/Texture/enemy_kill.png", false);
		destroyEmitParticle[3]->Initialize(L"Data/Texture/enemy_kill2.png", false);
		destroyEmitParticle[5]->Initialize(L"Data/Texture/enemy_kill3.png", false);
	}

	// スコアボールの残像Particle
	{
		trakingScoreLocus = std::make_unique<EmitParticle>();
		trakingScoreLocus->SetMaxParticleCount(100000);
		particleInfo.speed = 18.0f;
		particleInfo.size = 0.0f;
		particleInfo.scaleX = 0.05f;
		particleInfo.life = 2.0f;
		particleInfo.motionBlurAmount = 0.0f;
		particleInfo.rotation = 0.01f;
		particleInfo.color = 30.0f;
		trakingScoreLocus->setEmitShader("EmitParticleEmitTrackingScoreCS.cso");
		trakingScoreLocus->setSimulateShader("EmitParticleSimulateTrackingScoreCS.cso");
		trakingScoreLocus->setParticleInfo(particleInfo);
		trakingScoreLocus->Initialize(L"Data/Texture/player_shot.png", false);
	}

	// タイトルのParticle
	{
		titleParticle[0] = std::make_unique<EmitParticle>();
		titleParticle[1] = std::make_unique<EmitParticle>();
		titleParticle[0]->SetMaxParticleCount(100000);
		titleParticle[1]->SetMaxParticleCount(100000);
		particleInfo.speed = 2;
		particleInfo.size = 0.1f;
		particleInfo.scaleX = 0.05f;
		particleInfo.life = 2.0f;
		particleInfo.motionBlurAmount = 0.0f;
		particleInfo.rotation = 0.05f;
		particleInfo.color = 30.0f;
		titleParticle[0]->setEmitShader("EmitParticleTitleLeftEmitCS.cso");
		titleParticle[1]->setEmitShader("EmitParticleTitleRightEmitCS.cso");
		titleParticle[0]->setParticleInfo(particleInfo);
		particleInfo.speed = -2;
		titleParticle[1]->setParticleInfo(particleInfo);
		titleParticle[0]->Initialize(L"Data/Texture/titleParticle.png", false);
		titleParticle[1]->Initialize(L"Data/Texture/titleParticle.png", false);

	}
}

void ParticleSystem::Update(float elapsedTime)
{
	for (auto& it : destroyEmitParticle)
	{
		it->UpdateGPU();
	}
	playerShotDestroyParticle[0]->UpdateGPU();
	playerShotDestroyParticle[1]->UpdateGPU();
	titleParticle[0]->UpdateGPU();
	titleParticle[1]->UpdateGPU();
	floorParticle->UpdateGPU();
	playerDestroyParticle->UpdateGPU();
	playerDashParticle->UpdateGPU();
	playerSpawnParticle->UpdateGPU();
	trakingScoreLocus->UpdateGPU();
#ifdef _DEBUG


	ImGui::Begin("ParticleSystem");

	ImGui::SliderFloat("size", &particleInfo.size, 0.0f, 10.0f);
	ImGui::SliderFloat("life", &particleInfo.life, 0.0f, 10.0f);
	ImGui::SliderFloat("scale", &particleInfo.scaleX, 0.0f, 10.0f);
	ImGui::SliderFloat("speed", &particleInfo.speed, 0.0f, 100.0f);
	ImGui::SliderFloat("rotation", &particleInfo.rotation, 0.0f, 100.0f);
	ImGui::SliderFloat("BlueAmonf", &particleInfo.motionBlurAmount, 0.0f, 10.0f);

	if(ImGui::Button("Compile")) 	destroyEmitParticle[1]->setParticleInfo(particleInfo);


	ImGui::End();
#endif // _DEBUG

}

void ParticleSystem::BloomRender()
{

	destroyEmitParticle[0]->Draw();
	destroyEmitParticle[2]->Draw();
	destroyEmitParticle[4]->Draw();
	destroyEmitParticle[1]->Draw();
	destroyEmitParticle[3]->Draw();
	destroyEmitParticle[5]->Draw();

	playerShotDestroyParticle[0]->Draw();
	playerShotDestroyParticle[1]->Draw();
	titleParticle[0]->Draw();
	titleParticle[1]->Draw();
	playerDestroyParticle->Draw();
	playerDashParticle->Draw();
	floorParticle->Draw();
	playerSpawnParticle->Draw();
	trakingScoreLocus->Draw();
}

void ParticleSystem::NotBloomRender()
{
}

void ParticleSystem::Finalize()
{

}
#pragma once
#include <DirectXMath.h>
#include "Collision.h"
#include "../Library/Model.h"
#include "../Library/ModelResource.h"
#include "../Library/GPUParticle/EmitParticle.h"

class ParticleSystem
{
private:
	ParticleInfo particleInfo;
public:
	std::unique_ptr<EmitParticle> destroyEmitParticle[2 * 3];
	std::unique_ptr<EmitParticle> playerShotDestroyParticle[2];
	std::unique_ptr<EmitParticle> titleParticle[2];
	std::unique_ptr<EmitParticle> playerDestroyParticle;
	std::unique_ptr<EmitParticle> playerDashParticle;
	std::unique_ptr<EmitParticle> playerSpawnParticle;
	std::unique_ptr<EmitParticle> floorParticle;
	std::unique_ptr<EmitParticle> trakingScoreLocus;
public:
	ParticleSystem () {}
	~ParticleSystem() {}

	void Initialize();
	void Update(float elapsedTime);
	void BloomRender();
	void NotBloomRender();
	void Finalize();

	static ParticleSystem* GetInstance()
	{
		static ParticleSystem instance;
		return &instance;
	}

};
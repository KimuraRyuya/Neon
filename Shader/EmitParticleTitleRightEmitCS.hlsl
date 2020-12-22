
#include "Globals.hlsli"
#include "SceneConstants.hlsli"
#include "../Library/GPUParticle/ShaderInTerop_EmittedPaticle.h"
#include "../Library/GPUParticle/ShaderInTeropRenderer.h"

RWSTRUCTUREDBUFFER(particleBuffer, Particle, 0);
RWSTRUCTUREDBUFFER(aliveBuffer_CURRENT, uint, 1);
RWSTRUCTUREDBUFFER(aliveBuffer_NEW, uint, 2);
RWSTRUCTUREDBUFFER(deadBuffer, uint, 3);
RWRAWBUFFER(counterBuffer, 4);

[numthreads(THREADCOUNT_EMIT, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{

	// emit count をロード
	uint emitCount = counterBuffer.Load(PARTICLECOUNTER_OFFSET_REALEMITCOUNT);

	if (DTid.x < emitCount)
	{

		uint constantIndex = DTid.x % (particleParamSize);

		// emit する
		float2 uv = float2(elapsedTime.x + particleParam[constantIndex].xEmitterRandomness, (float)DTid.x / (float)THREADCOUNT_EMIT);
		float seed = 0.12345;

		// Just emit from center point:
		float3 pos = mul(particleParam[constantIndex].xEmitterWorld, float4(0, 0, 0, 1)).xyz;
		float3 nor = float3(1, 1, 1);


		float particleStartingSize = particleParam[constantIndex].xParticleSize + particleParam[constantIndex].xParticleSize * (rand(seed, uv) - 0.5f) * particleParam[constantIndex].xParticleRandomFactor;

		// 新たなparticle 生成
		float addPosY = (rand(seed, uv) - 0.5) * (2.5);

		Particle particle;
		particle.position = pos + float3(0.0f, addPosY, 0.0f);
		particle.velocity = float3(particleParam[constantIndex].xParticleSpeed, 0.0, 0.0);
		particle.accel = float3(0, 0, 0);
		particle.rotationalVelocity = (rand(seed, uv) - 0.5) * particleParam[constantIndex].xParticleRotation;
		particle.maxLife = (abs(addPosY) / -1.0) + particleParam[constantIndex].xParticleLifeSpan;
		particle.life = particle.maxLife;
		particle.sizeBeginEnd = float2(particleStartingSize, particleStartingSize * particleParam[constantIndex].xParticleScaling);
		particle.color = 0;
		particle.opacity = particleParam[constantIndex].xEmitterOpacity;
		particle.motionBlurAmount = particleParam[constantIndex].xParticleMotionBlurAmount;

		particle.color = particleParam[constantIndex].xParticleColor;


		// dead list から新たなParticleのインデックスにセット
		uint deadCount;
		counterBuffer.InterlockedAdd(PARTICLECOUNTER_OFFSET_DEADCOUNT, -1, deadCount);
		uint newParticleIndex = deadBuffer[deadCount - 1];

		// 書き換える
		particleBuffer[newParticleIndex] = particle;

		// alive list にpushする
		uint aliveCount;
		counterBuffer.InterlockedAdd(PARTICLECOUNTER_OFFSET_ALIVECOUNT, 1, aliveCount);
		aliveBuffer_CURRENT[aliveCount] = newParticleIndex;

	}

}
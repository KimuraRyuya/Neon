
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
void main( uint3 DTid : SV_DispatchThreadID )
{
    
    // emit count をロード
    uint emitCount = counterBuffer.Load(PARTICLECOUNTER_OFFSET_REALEMITCOUNT);

    if (DTid.x < emitCount)
    {
        
        uint constantIndex = (DTid.x) % (particleParamSize);

        // emit する
        float2 uv = float2(elapsedTime.x + particleParam[constantIndex].xEmitterRandomness, (float) DTid.x / (float) THREADCOUNT_EMIT);
        float seed = 0.12345;

		// Just emit from center point:
        float3 pos = mul(particleParam[constantIndex].xEmitterWorld, float4(0, 0, 0, 1)).xyz;
        float3 nor = float3(1, 1, 1);

        
        float particleStartingSize = particleParam[constantIndex].xParticleSize + particleParam[constantIndex].xParticleSize * (rand(seed, uv) - 0.5f) * particleParam[constantIndex].xParticleRandomFactor;

		// 新たなparticle 生成
        
        Particle particle;
        particle.position = pos;
        particle.velocity = float3((rand(seed, uv) - 0.5) * particleParam[constantIndex].xParticleSpeed, (rand(seed, uv) + 0.5) * 5, (rand(seed, uv) - 0.5) * particleParam[constantIndex].xParticleSpeed);
        particle.accel = normalize(particle.velocity);
        particle.rotationalVelocity = 10;
        particle.maxLife = particleParam[constantIndex].xParticleLifeSpan + particleParam[constantIndex].xParticleLifeSpan * (rand(seed, uv) - 0.5f) * particleParam[constantIndex].xParticleLifeSpanRandomness;
        particle.life = particle.maxLife;
        particle.sizeBeginEnd = float2(particleStartingSize, particleStartingSize * particleParam[constantIndex].xParticleScaling);
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
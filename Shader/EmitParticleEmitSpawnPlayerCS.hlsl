#include "Globals.hlsli"
#include "SceneConstants.hlsli"
#include "../Library/GPUParticle/ShaderInTerop_EmittedPaticle.h"
#include "../Library/GPUParticle/ShaderInTeropRenderer.h"

RWSTRUCTUREDBUFFER(particleBuffer, Particle, 0);
RWSTRUCTUREDBUFFER(aliveBuffer_CURRENT, uint, 1);
RWSTRUCTUREDBUFFER(aliveBuffer_NEW, uint, 2);
RWSTRUCTUREDBUFFER(deadBuffer, uint, 3);
RWRAWBUFFER(counterBuffer, 4);

TYPEDBUFFER(meshIndexBuffer, uint, TEXSLOT_ONDEMAND0);
RAWBUFFER(meshVertexBuffer_POS, TEXSLOT_ONDEMAND1);

// 回転
float4x4 rotate(float angle, float3 axis)
{
    float3 a = normalize(axis);
    float s = sin(angle);
    float c = cos(angle);
    float r = 1.0 - c;
    float4x4 m = float4x4(
        a.x * a.x * r + c,
        a.y * a.x * r + a.z * s,
        a.z * a.x * r - a.y * s,
        0,
        a.x * a.y * r - a.z * s,
        a.y * a.y * r + c,
        a.z * a.y * r + a.x * s,
        0,
        a.x * a.z * r + a.y * s,
        a.y * a.z * r - a.x * s,
        a.z * a.z * r + c,
        0,
        0,
        0,
        0,
        1
    );
    return m;
}

[numthreads(THREADCOUNT_EMIT, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    
    // emit count をロード
    uint emitCount = counterBuffer.Load(PARTICLECOUNTER_OFFSET_REALEMITCOUNT);

    if (DTid.x < emitCount)
    {
        
        uint constantIndex = DTid.x % (particleParamSize);
        
        // emit する
        float2 uv = float2(elapsedTime.x + particleParam[constantIndex].xEmitterRandomness, (float) DTid.x / (float) THREADCOUNT_EMIT);
        float seed = 0.12345;

		// random triangle on emitter surface:
        uint tri = (uint) ((particleParam[constantIndex].xEmitterMeshIndexCount / 3) * rand(seed, uv));

		// load indices of triangle from index buffer
        uint i0 = meshIndexBuffer[tri * 3 + 0];
        uint i1 = meshIndexBuffer[tri * 3 + 1];
        uint i2 = meshIndexBuffer[tri * 3 + 2];

		// load vertices of triangle from vertex buffer:
        float4 pos_nor0 = asfloat(meshVertexBuffer_POS.Load4(i0 * particleParam[constantIndex].xEmitterMeshVertexPositionStride));
        float4 pos_nor1 = asfloat(meshVertexBuffer_POS.Load4(i1 * particleParam[constantIndex].xEmitterMeshVertexPositionStride));
        float4 pos_nor2 = asfloat(meshVertexBuffer_POS.Load4(i2 * particleParam[constantIndex].xEmitterMeshVertexPositionStride));

        float4 nor0 = asfloat(meshVertexBuffer_POS.Load4(i0 * (particleParam[constantIndex].xEmitterMeshVertexPositionStride + 12)));
        float4 nor1 = asfloat(meshVertexBuffer_POS.Load4(i1 * (particleParam[constantIndex].xEmitterMeshVertexPositionStride + 12)));
        float4 nor2 = asfloat(meshVertexBuffer_POS.Load4(i2 * (particleParam[constantIndex].xEmitterMeshVertexPositionStride + 12)));

		// compute final surface position on triangle from barycentric coords:
        float3 pos = pos_nor0.xyz;
        float3 nor = nor0.xyz;
        float3 sp = normalize(pos);
        float4x4 world = mul(particleParam[constantIndex].xEmitterWorld, rotate((rand(seed, uv) * 2.0 - 1.0), float3((rand(seed, uv) * 2.0 - 1.0), (rand(seed, uv) * 2.0 - 1.0), (rand(seed, uv) * 2.0 - 1.0))));
        pos = mul(world, float4(pos, 1)).xyz;
        
        float particleStartingSize = particleParam[constantIndex].xParticleSize + particleParam[constantIndex].xParticleSize * (rand(seed, uv) - 0.5f) * particleParam[constantIndex].xParticleRandomFactor;

		// 新たなparticle 生成
        
        Particle particle;
        particle.position = sp * 50.0f;
        particle.velocity = (pos - particle.position) / 1.0f;
        particle.accel = float3(0, 0, 0);
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
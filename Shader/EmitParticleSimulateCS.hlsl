#include "../Library/GPUParticle/ShaderInTerop_EmittedPaticle.h"
#include "Globals.hlsli"
#include "SceneConstants.hlsli"

RWSTRUCTUREDBUFFER(particleBuffer, Particle, 0);
RWSTRUCTUREDBUFFER(aliveBuffer_CURRENT, uint, 1);
RWSTRUCTUREDBUFFER(aliveBuffer_NEW, uint, 2);
RWSTRUCTUREDBUFFER(deadBuffer, uint, 3);
RWRAWBUFFER(counterBuffer, 4);

[numthreads(THREADCOUNT_SIMULATION, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID, uint Gid : SV_GroupIndex)
{
	//uint aliveCount = counterBuffer[0].aliveCount;
    uint aliveCount = counterBuffer.Load(PARTICLECOUNTER_OFFSET_ALIVECOUNT);


    GroupMemoryBarrierWithGroupSync();
    
    if (DTid.x < aliveCount)
    {
		// simulation can be either fixed or variable timestep:
        //const float dt = xEmitterFixedTimestep >= 0 ? xEmitterFixedTimestep : elapsedTime;
        const float dt = elapsedTime.x;

        uint particleIndex = aliveBuffer_CURRENT[DTid.x];
        Particle particle = particleBuffer[particleIndex];

        if (particle.life > 0)
        {
			// simulate:

			// integrate:
            particle.velocity += particle.accel * dt;
            particle.position += particle.velocity * dt;
 			// reset force for next frame:
            particle.accel = 0;
            particle.life -= dt;

			// write back simulated particle:
            particleBuffer[particleIndex] = particle;

			// add to new alive list:
            uint newAliveIndex;
            counterBuffer.InterlockedAdd(PARTICLECOUNTER_OFFSET_ALIVECOUNT_AFTERSIMULATION, 1, newAliveIndex);
            aliveBuffer_NEW[newAliveIndex] = particleIndex;

        }
        else
        {
			// kill:
            uint deadIndex;
            counterBuffer.InterlockedAdd(PARTICLECOUNTER_OFFSET_DEADCOUNT, 1, deadIndex);
            deadBuffer[deadIndex] = particleIndex;
        }
    }

}

#include "../Library/GPUParticle/ShaderInTerop_EmittedPaticle.h"
#include "Globals.hlsli"
#include "SceneConstants.hlsli"

RWRAWBUFFER(counterBuffer, 4);
RWRAWBUFFER(indirectBuffers, 5);

[numthreads(1, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{    
    // Particle ��dead count �����[�h����
    uint deadCount = counterBuffer.Load(PARTICLECOUNTER_OFFSET_DEADCOUNT);
    
    // Particle ��alive count �����[�h
    uint aliveCount_NEW = counterBuffer.Load(PARTICLECOUNTER_OFFSET_ALIVECOUNT_AFTERSIMULATION);
    
    // dead list �ɋ󂫃X���b�g�������emit�ł��Ȃ�
    uint realEmitCount = min(deadCount, xEmitCount);
    
    // dispatch argment buffer ��emitting ���Z�b�g
    indirectBuffers.Store3(ARGUMENTBUFFER_OFFSET_DISPATCHEMIT, uint3(ceil((float) realEmitCount / (float) THREADCOUNT_EMIT), 1, 1));
    
    // dispatch argment buffer �� simulation ���Z�b�g
    indirectBuffers.Store3(ARGUMENTBUFFER_OFFSET_DISPATCHSIMULATION, uint3(ceil((float) (aliveCount_NEW + realEmitCount) / (float) THREADCOUNT_SIMULATION), 1, 1));

    // �V����alive count ���Z�b�g
    counterBuffer.Store(PARTICLECOUNTER_OFFSET_ALIVECOUNT, aliveCount_NEW);

    // �V����alive count �����Z�b�g 
    counterBuffer.Store(PARTICLECOUNTER_OFFSET_ALIVECOUNT_AFTERSIMULATION, 0);

    // �V����emit count ���Z�b�g
    counterBuffer.Store(PARTICLECOUNTER_OFFSET_REALEMITCOUNT, realEmitCount);

}
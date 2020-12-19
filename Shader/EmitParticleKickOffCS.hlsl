#include "../Library/GPUParticle/ShaderInTerop_EmittedPaticle.h"
#include "Globals.hlsli"
#include "SceneConstants.hlsli"

RWRAWBUFFER(counterBuffer, 4);
RWRAWBUFFER(indirectBuffers, 5);

[numthreads(1, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{    
    // Particle のdead count をロードする
    uint deadCount = counterBuffer.Load(PARTICLECOUNTER_OFFSET_DEADCOUNT);
    
    // Particle のalive count をロード
    uint aliveCount_NEW = counterBuffer.Load(PARTICLECOUNTER_OFFSET_ALIVECOUNT_AFTERSIMULATION);
    
    // dead list に空きスロットがあるとemitできない
    uint realEmitCount = min(deadCount, xEmitCount);
    
    // dispatch argment buffer のemitting をセット
    indirectBuffers.Store3(ARGUMENTBUFFER_OFFSET_DISPATCHEMIT, uint3(ceil((float) realEmitCount / (float) THREADCOUNT_EMIT), 1, 1));
    
    // dispatch argment buffer の simulation をセット
    indirectBuffers.Store3(ARGUMENTBUFFER_OFFSET_DISPATCHSIMULATION, uint3(ceil((float) (aliveCount_NEW + realEmitCount) / (float) THREADCOUNT_SIMULATION), 1, 1));

    // 新たなalive count をセット
    counterBuffer.Store(PARTICLECOUNTER_OFFSET_ALIVECOUNT, aliveCount_NEW);

    // 新たなalive count をリセット 
    counterBuffer.Store(PARTICLECOUNTER_OFFSET_ALIVECOUNT_AFTERSIMULATION, 0);

    // 新たなemit count をセット
    counterBuffer.Store(PARTICLECOUNTER_OFFSET_REALEMITCOUNT, realEmitCount);

}
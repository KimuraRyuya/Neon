#ifndef WI_SHADERINTEROP_EMITTEDPARTICLE_H
#define WI_SHADERINTEROP_EMITTEDPARTICLE_H

#include "ShaderInterop.h"

//struct EnemyBufferInfo
//{
//	float3 pos;
//	float3 scale;
//	float3 size;
//	float hp;
//	// 40 byte
//};


struct Particle
{
	float3 position;
	float3 accel;
	float rotationalVelocity;
	float3 velocity;
	float maxLife;
	float2 sizeBeginEnd;
	float life;
	uint color;
	float opacity;
	float motionBlurAmount;
};

struct ParticleCounters
{
	uint aliveCount;
	uint deadCount;
	uint realEmitCount;
	uint aliveCount_afterSimulation;
};

static const uint PARTICLECOUNTER_OFFSET_ALIVECOUNT = 0;
static const uint PARTICLECOUNTER_OFFSET_DEADCOUNT = PARTICLECOUNTER_OFFSET_ALIVECOUNT + 4;
static const uint PARTICLECOUNTER_OFFSET_REALEMITCOUNT = PARTICLECOUNTER_OFFSET_DEADCOUNT + 4;
static const uint PARTICLECOUNTER_OFFSET_ALIVECOUNT_AFTERSIMULATION = PARTICLECOUNTER_OFFSET_REALEMITCOUNT + 4;


struct ParticleParam
{
	float4x4	 xEmitterWorld;

	uint		xEmitterMeshIndexCount;
	uint		xEmitterMeshVertexPositionStride;
	float		xEmitterRandomness;
	float		xParticleSize;
	float     xParticleSpeed;
	float		xParticleScaling;
	float		xParticleRotation;
	float		xParticleColor;

	float		xParticleRandomFactor;
	float		xParticleNormalFactor;
	float		xParticleLifeSpan;
	float		xParticleLifeSpanRandomness;

	float		xParticleMotionBlurAmount;
	float		xEmitterOpacity;
	uint		xEmitterMaxParticleCount;

	float		xEmitterFixedTimestep;	// we can force a fixed timestep (>0) onto the simulation to avoid blowing up
};

CBUFFER(EmittedParticleCB, CBSLOT_OTHER_EMITTEDPARTICLE)
{
	ParticleParam particleParam[200];
	uint particleParamSize;
	uint		xEmitCount;
};



#define THREADCOUNT_EMIT 256
#define THREADCOUNT_SIMULATION 256

static const uint ARGUMENTBUFFER_OFFSET_DISPATCHEMIT = 0;
static const uint ARGUMENTBUFFER_OFFSET_DISPATCHSIMULATION = ARGUMENTBUFFER_OFFSET_DISPATCHEMIT + (3 * 4);
static const uint ARGUMENTBUFFER_OFFSET_DRAWPARTICLES = ARGUMENTBUFFER_OFFSET_DISPATCHSIMULATION + (3 * 4);

#endif // WI_SHADERINTEROP_EMITTEDPARTICLE_H


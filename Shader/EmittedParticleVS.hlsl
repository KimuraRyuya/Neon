#include "EmittedParticleHF.hlsli"
#include "../Library/GPUParticle/ShaderInTerop_EmittedPaticle.h"
#include "../Library/GPUParticle/ShaderInTeropRenderer.h"
#include "SceneConstants.hlsli"

static const float3 BILLBOAD[] =
{
    float3(-1, -1, 0),
    float3(1, -1, 0),
    float3(-1, 1, 0),
    float3(-1, 1, 0),
    float3(1, -1, 0),
    float3(1, 1, 0),
};

STRUCTUREDBUFFER(particleBuffer, Particle, 0);
STRUCTUREDBUFFER(aliveList, uint, 1);

VertextoPixel main(uint fakeIndex : SV_VertexID)
{
    VertextoPixel Out;
    
    // geometry shader ���X�L�b�v����vertex shader ��Particle��W�J����
    uint vertexID = fakeIndex % 6;
    uint instanceID = fakeIndex / 6;
    
    // �p�[�e�B�N�������[�h����
    Particle particle = particleBuffer[aliveList[instanceID]];
    
    // life����Render Properties ���쐬
    float lifeLerp = 1 - particle.life / particle.maxLife;
    float size = lerp(particle.sizeBeginEnd.x, particle.sizeBeginEnd.y, lifeLerp);
    float rotation = lifeLerp * particle.rotationalVelocity;
    //float rotation = atan2(particle.velocity.z - particle.position.z, particle.velocity.x - particle.position.x);
    
    // view space �� billboard�p���W�쐬
    float3 quadPos = BILLBOAD[vertexID];
    float2 uv = quadPos.xy * float2(0.5f, -0.5f) + 0.5f;
    uv.x = particle.color & 0x10000000 ? 1.0f - uv.x : uv.x;
    uv.y = particle.color & 0x20000000 ? 1.0f - uv.y : uv.y;

    // �r���{�[�h����]������
    float2x2 rot = float2x2(
    cos(rotation), -sin(rotation),
    sin(rotation), cos(rotation)
    );
    quadPos.xy = mul(quadPos.xy, rot);
    
    // �r���{�[�h���g�k����
    quadPos *= size;
    
    // view space���̓����������쐬
    float3 velocity = mul((float3x3) cameraView, particle.velocity);
   
    quadPos += dot(quadPos, velocity) * velocity * particle.motionBlurAmount;
    
    // Out �ɃR�s�[����
    Out.pos = float4(particle.position, 1);
    Out.pos = mul(cameraView, Out.pos);
    Out.pos.xyz += quadPos.xyz;
    Out.pos = mul(cameraProjection, Out.pos);

    Out.tex = uv;
    Out.size = size;
    Out.color = particle.color;
    Out.pos2D = Out.pos;
    Out.opacity = particle.opacity;

	return Out;
}
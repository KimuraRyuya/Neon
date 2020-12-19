#include "EmittedParticleHF.hlsli"
#include "../Library/GPUParticle/ShaderInTerop_EmittedPaticle.h"
#include "../Library/GPUParticle/ShaderInTeropRenderer.h"
#include "SceneConstants.hlsli"

// �e�X�N�`���ƃT���v��
Texture2D texAlbedo : register(t0);
SamplerState samAlbedo : register(s0);

float4 main(VertextoPixel pin) : SV_TARGET
{
    return texAlbedo.Sample(samAlbedo, pin.tex) * float4(pin.color, pin.color, pin.color, pin.opacity);
}
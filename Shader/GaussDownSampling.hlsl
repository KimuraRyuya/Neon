#include "Bloom.hlsli"

Texture2D hdr_colour_map : register(t0);
SamplerState linear_border_sampler_state : register(s0);

float4 main(float4 position : SV_POSITION, float2 texcoord : TEXCOORD) : SV_TARGET
{
    return hdr_colour_map.Sample(linear_border_sampler_state, texcoord, 0.0);
}

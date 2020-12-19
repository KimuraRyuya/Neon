
#include "Bloom.hlsli"

Texture2D hdr_colour_map : register(t0);

SamplerState point_sampler_state : register(s0);
SamplerState linear_sampler_state : register(s1);
SamplerState anisotropic_sampler_state : register(s2);

float4 main(float4 position : SV_POSITION, float2 texcoord : TEXCOORD) : SV_TARGET
{
    uint mip_level = 0, width, height, number_of_levels;
    hdr_colour_map.GetDimensions(mip_level, width, height, number_of_levels);
    float4 hdr_colour = hdr_colour_map.Sample(anisotropic_sampler_state, texcoord);
    return float4(max(0.0, dot(hdr_colour.rgb, float3(0.2126, 0.7152, 0.0722))), 0, 0, 1);
}


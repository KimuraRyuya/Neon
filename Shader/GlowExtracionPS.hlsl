#include "Bloom.hlsli"

Texture2D hdr_colour_map : register(t0);

SamplerState point_sampler_state : register(s0);
SamplerState linear_sampler_state : register(s1);
SamplerState anisotropic_sampler_state : register(s2);

float4 main(float4 position : SV_POSITION, float2 texcoord : TEXCOORD) : SV_TARGET
{
    float4 colour_map_colour = hdr_colour_map.Sample(point_sampler_state, texcoord);
    float luminance = dot(colour_map_colour.rgb, float3(0.2126, 0.7152, 0.0722));
    return float4(colour_map_colour.rgb, 1.0);
}

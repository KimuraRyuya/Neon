#include "Bloom.hlsli"

Texture2D downsampled_textures[number_of_downsampled] : register(t0);

SamplerState point_sampler_state : register(s0);
SamplerState linear_sampler_state : register(s1);
SamplerState anisotropic_sampler_state : register(s2);

float4 main(float4 position : SV_POSITION, float2 texcoord : TEXCOORD) : SV_TARGET
{
    float3 fragment_colour = 0;
	[unroll]
    for (uint index_of_downsampled = 0; index_of_downsampled < number_of_downsampled; ++index_of_downsampled)
    {
        fragment_colour += downsampled_textures[index_of_downsampled].Sample(anisotropic_sampler_state, texcoord).xyz;
    }
    return float4(fragment_colour * blur_convolution_intensity, 1);
}

#include "SceneConstants.hlsli"

Texture2D colorMap : register(t0);

SamplerState point_sampler_state : register(s0);
SamplerState linear_sampler_state : register(s1);
SamplerState anisotropic_sampler_state : register(s2);

float perspective = 0.3;

cbuffer ZoomBlurConstants : register(b0)
{
    float blurScalar;
    float samples;
    float minBlur;
    float maxBlur;
};


float4 main(float4 position : SV_POSITION, float2 texcoord : TEXCOORD) : SV_TARGET
{
    float4 colour_map_colour = colorMap.Sample(point_sampler_state, texcoord);
	
    float4 result = (0);
    
    float timeQ = lerp(minBlur, maxBlur, (sin(elapsedTime.y * blurScalar) + 1.) / 2.);
    
    for (int i = 0; i <= int(samples); i++)
    {
        float q = float(i) / float(samples);
        float2 uv = texcoord + (float2(0.5f, 0.5f) - texcoord) * q * timeQ / float(samples);
        result += colorMap.Sample(anisotropic_sampler_state, uv);
    }
    
    return result;
}

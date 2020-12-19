#include "SceneConstants.hlsli"

Texture2D myTexture : register(t0); //テクスチャー
Texture2D randomNoise_texture : register(t1);
Texture2D glitch_texture : register(t2);
SamplerState mySampler : register(s0); //サンプラー

#define PI 3.1415926

float2 interlace(float2 uv, float s)
{
    uv.x += s * (4.0 * frac((uv.y / 2.0) - 1.0));
    return uv;
}

float2 fault(float2 uv, float s)
{
    float v = pow(0.5 - 0.5 * cos(2.0 * PI * uv.y), 100.0) * sin(2.0 * PI * uv.y);
    uv.x += v * s;
    return uv;
}

float2 rnd(float2 uv, float s)
{
    uv.x += s * (2.0 * randomNoise_texture.Sample(mySampler, uv * 0.05).x - 1.0);
    return uv;
}

float3 colorSplit(float2 uv, float2 s)
{
    float3 color;
    color.r = myTexture.Sample(mySampler, uv - s).r;
    color.g = myTexture.Sample(mySampler, uv).g;
    color.b = myTexture.Sample(mySampler, uv + s).b;
    return color;
}

float4 main(float4 position : SV_POSITION, float2 texcoord : TEXCOORD, uint sample_index : SV_SAMPLEINDEX) : SV_TARGET
{    
    float t = fmod(elapsedTime.y, 21.0f) / 20.0;
    
    float s = randomNoise_texture.Sample(mySampler, float2(t * 3.0, 0.5)).r;
    texcoord = interlace(texcoord, s * 0.05);

    float r = glitch_texture.Sample(mySampler, float2(t * 0.4, 0.0)).x;
    
    texcoord = fault(texcoord + float2(0.0, frac(t * 2.0)), 10.0 * sign(r) * pow(abs(r), 0.25)) - float2(0.0, frac(t * 2.0));
    texcoord = rnd(texcoord, s * 0.02);
    
    float3 noiseC = colorSplit(texcoord, float2(s * 0.01, 0.0));

    noiseC = lerp(noiseC, randomNoise_texture.Sample(mySampler, 0.5 * texcoord + t * 100.0).rgb, 0.1);
        
    return float4(noiseC, 1.0);
}
#include "Billboard.hlsli" 

Texture2D myTexture : register(t0); //テクスチャー
SamplerState mySampler : register(s0); //サンプラー

float4 main(VS_OUT pin) : SV_TARGET
{
    float2 ndc = float2(pin.tex.x * 2.0 - 1.0, pin.tex.y * 2.0 - 1.0);
    
    float ang = atan2(ndc.y, ndc.x) + 180.0f * 0.01745;
    
    if (ang <= angle)
    {
       clip(-1);
    }
    
        return myTexture.Sample(mySampler, pin.tex) * color * luminans;
    }
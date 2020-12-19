#include "Billboard.hlsli" 

Texture2D    myTexture : register( t0 ); //テクスチャー
SamplerState mySampler : register( s0 ); //サンプラー

float4 main( VS_OUT pin ) : SV_TARGET
{
    return myTexture.Sample(mySampler, pin.tex) * color * luminans;
}
#include "Billboard.hlsli"

SamplerState mySampler : register(s0); //サンプラー
Texture2D myTexture : register(t0); //テクスチャー
Texture2D mask_texture : register(t1);
Texture2D emissive_texture : register(t2);

float4 main(VS_OUT pin) : SV_TARGET
{
    float4 col = myTexture.Sample(mySampler, pin.tex) * color * luminans;
    float mask = mask_texture.Sample(mySampler, pin.tex).r;
    
    float _dissolveValue = 0.0f;
    _dissolveValue = min(dissolveValue, 1.0f);
    _dissolveValue = max(dissolveValue, 0.0f);
    
    float thresold = 1.0f - dissolveValue;
    float alpha = step(mask, thresold);
//発光箇所計算
    float3 emissive = (float3) 0;
    float3 emissive_color = emissive_texture.Sample(mySampler, pin.tex).rgb * emissiveColor.rgb;
    float emissive_value = (mask + emissiveRange > thresold) && (thresold > mask);
    emissive = emissive_color * emissive_value;
//色補正
    col.rgb += emissive * _dissolveValue;
    col.a *= alpha;
    return col;
}
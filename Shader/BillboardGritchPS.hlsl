#include "SceneConstants.hlsli"
#include "Billboard.hlsli"

Texture2D myTexture : register(t0); //テクスチャー
Texture2D mask_texture : register(t1);
Texture2D emissive_texture : register(t2);
Texture2D randomNoise_texture : register(t3);
Texture2D glitch_texture : register(t4);
SamplerState mySampler : register(s0); //サンプラー

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

float4 main(VS_OUT pin) : SV_TARGET
{
    float4 col = (float4)0;
    col = myTexture.Sample(mySampler, pin.tex) * color * luminans;
    
    // dissolve
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
    
    float t = fmod(elapsedTime.y, 21.0f) / 20.0;
    
    float s = randomNoise_texture.Sample(mySampler, float2(t * 3.0, 0.5)).r;
    pin.tex = interlace(pin.tex, s * 0.05);

    float r = glitch_texture.Sample(mySampler, float2(t * 0.4, 0.0)).x;
    
    pin.tex = fault(pin.tex + float2(0.0, frac(t * 2.0)), 10.0 * sign(r) * pow(abs(r), 0.25)) - float2(0.0, frac(t * 2.0));
    pin.tex = rnd(pin.tex, s * 0.02);
    
    float3 noiseC = colorSplit(pin.tex, float2(s * 0.01, 0.0));

    noiseC = lerp(noiseC, randomNoise_texture.Sample(mySampler, 0.5 * pin.tex + t * 100.0).rgb, 0.1);
    
    col.rgb *= noiseC;
    col.rgb += emissive *_dissolveValue;
    col.a *= alpha;    
    
    return float4(col.rgb, col.a);
}
// UNIT.09
#include "SpriteBatch.hlsli"

Texture2D diffuse_map : register(t0);
SamplerState diffuse_map_sampler_state : register(s0);
Texture2D mask_texture : register(t1);
Texture2D emissive_texture : register(t2);

float4 main(VS_OUT pin) : SV_TARGET
{
    float4 col = diffuse_map.Sample(diffuse_map_sampler_state, pin.texcoord) * pin.color;
    float mask = mask_texture.Sample(diffuse_map_sampler_state, pin.texcoord).r;
    
    float _dissolveValue = 0.0f;
    _dissolveValue = min(dissolveValue, 1.0f);
    _dissolveValue = max(dissolveValue, 0.0f);
    
    float thresold = 1.0f - dissolveValue;
    float alpha = step(mask, thresold);
//î≠åıâ”èäåvéZ
    float3 emissive = (float3) 0;
    float3 emissive_color = emissive_texture.Sample(diffuse_map_sampler_state, pin.texcoord).rgb * emissiveColor.rgb;
    float emissive_value = (mask + emissiveRange > thresold) && (thresold > mask);
    emissive = emissive_color * emissive_value;

//êFï‚ê≥
    col.rgb += emissive * _dissolveValue;
    col.a *= alpha;
    return col;
}
#include "Billboard.hlsli" 

VS_OUT main(float4 position : POSITION, float2 tex : TEXCOORD)
{
    VS_OUT vout;

    vout.position = mul(position, world_view_projection);
    vout.tex = tex;

    return vout;
}

struct VS_OUT
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD;
};

cbuffer CONSTANT_BUFFER : register(b0)
{
    row_major float4x4 world_view_projection;
    float4 color;
    float4 luminans;
    float angle;
    float width;
    float height;
    float dummy00;
};
    
cbuffer DISSOLVE_CONSTANT_BUFFER : register(b1)
{
    float4 emissiveColor;
    float dissolveValue;
    float emissiveRange;
    float dummy01;
    float dummy02;
};

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



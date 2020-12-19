// UNIT.09
struct VS_OUT
{
	float4 sv_position : SV_POSITION;
	float2 texcoord : TEXCOORD;
	float4 color : COLOR;
};

cbuffer DISSOLVE_CONSTANT_BUFFER : register(b0)
{
    float4 emissiveColor;
    float dissolveValue;
    float emissiveRange;
    float dummy01;
    float dummy02;
};
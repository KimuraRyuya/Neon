    
    struct VS_OUT
{
	float4 position : SV_POSITION;
	float2 texcoord : TEXCOORD;
	float4 color    : COLOR;
    float3 wNormal : TEXCOORD1;
    float3 wEyeDir : TEXCOORD3;
    float3 wPos : TEXCOORD2;
};

struct VS_IN
{
    float4 position : POSITION;
    float3 normal : NORMAL;
    float2 texcoord : TEXCOORD;
    float4 bone_weights : WEIGHTS;
    uint4 bone_indices : BONES;
};

#define MAX_BONES 32
cbuffer CbMesh : register(b0)
{
	row_major float4x4	bone_transforms[MAX_BONES];
};

cbuffer CbSubset : register(b1)
{
	float4				material_color;
};

cbuffer CbScene : register(b2)
{
	float4x4				world;
    float4 luminance;
};

//�n�C���C�g
cbuffer CBPerFrame2 : register(b3)
{
    float4 LightColor;      //���C�g�̐F
    float4 LightDir;        //���C�g�̕���
    float4 AmbientColor;    //����
    float4 EyePos;          //�J�������W
}

//�t�H�O�̃p�����[�^
static const float3 fogColor = { 0.8, 0.8, 0.6 };
static const float fogNear = 20.0;
static const float fogFar = 50.0;
//�������C�g�p�����[�^
static const float3 skyColor = { 0.4, 0.6, 0.8 };
static const float3 groundColor = { 0.6, 0.4, 0.2 };
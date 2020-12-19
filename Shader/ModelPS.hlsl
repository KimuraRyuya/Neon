#include "Function.hlsli"
#include "Model.hlsli"
Texture2D diffuse_map : register(t0);
SamplerState difffuse_map_sampler_state : register(s0);

float4 main(VS_OUT pin) : SV_TARGET
{
    return diffuse_map.Sample(difffuse_map_sampler_state, pin.texcoord) * pin.color;
}




////ƒnƒCƒ‰ƒCƒg
//#include "function.hlsli"
//#include "model.hlsli"

//Texture2D diffuse_map : register(t0);
//SamplerState difffuse_map_sampler_state : register(s0);

//float4 main(VS_OUT pin) : SV_TARGET
//{
		
//    float4 color = diffuse_map.Sample(difffuse_map_sampler_state, pin.texcoord);
	
//    float3 N = normalize(pin.wNormal);
//    float3 E = normalize(EyePos.xyz - pin.wPos);
//    float3 L = normalize(LightDir.xyz);
	
//	// ŠÂ‹«Œõ
//    float3 A = AmbientColor.rgb;
	
//	// ŠgŽU”½ŽË
//    float3 C = LightColor.rgb;
//    float3 Kd = float3(1, 1, 1);
//    float3 D = Diffuse(N, L, C, Kd);
	
//	// ‹¾–Ê”½ŽË
//    float3 Ks = float3(1, 1, 1);
//    float3 S = BlinnPhongSpecular(N, L, C, E, Ks, 20);
	
//    color *= pin.color * float4(A + D + S, 1.0f);
	
//    return color;
	
//}

//#include "function.hlsli"
//#include "model.hlsli"

//Texture2D diffuse_map : register(t0);
//SamplerState difffuse_map_sampler_state : register(s0);

//float4 main(VS_OUT pin) : SV_TARGET
//{
//    float4 color = diffuse_map.Sample(difffuse_map_sampler_state, pin.texcoord) * pin.color;
	
//    float3 N = normalize(pin.wNormal);
//    float3 E = normalize(pin.wEyeDir);
//    float3 L = normalize(LightDir.xyz);
	
//	//ŠÂ‹«Œõ
//    float3 A = AmbientColor.rgb * HemiSphereLight(N, float3(0, 0, 1), float3(1, 0, 0));
	
//	//ŠgŽU”½ŽË
//    float3 C = LightColor.rgb;
//    float Kd = float3(1, 1, 1);
//    float3 D = Diffuse(N, L, C, Kd);
	
//	//‹¾–Ê”½ŽË
//    float3 Ks = float3(1, 1, 1);
//    float Shinness = 20;
//    float3 S = BlinnPhongSpecular(N, L, C, E, Ks, Shinness);
	
//    color *= pin.color * float4(A + D + S, 1.0);

//    color.rgb = Fog(color.rgb, EyePos.xyz, pin.wPos, fogColor, fogNear, fogFar);
	
//    return color;
	
//}
//通常

#include "SceneConstants.hlsli"
#include "Model.hlsli"

VS_OUT main(VS_IN vin)
{
    float3 p = { 0, 0, 0 };
    float3 n = { 0, 0, 0 };
	
    for (int i = 0; i < 4; i++)
    {
        p += (vin.bone_weights[i] * mul(vin.position, bone_transforms[vin.bone_indices[i]])).xyz;
        n += (vin.bone_weights[i] * mul(float4(vin.normal.xyz, 0), bone_transforms[vin.bone_indices[i]])).xyz;
    }

    VS_OUT vout;
    vout.position = mul(float4(p, 1.0f), rowCameraViewProjection);

    float3 N = normalize(n);
    float3 L = normalize(-lightDirection.xyz);
    float d = dot(L, N);
    vout.color.xyz = material_color.xyz * max(0, d) * luminance.xyz;
    vout.color.w = material_color.w;
    vout.texcoord = vin.texcoord;

    return vout;
    
}

//ハイライト
//#include "model.hlsli"

//VS_OUT main(VS_IN vin)
//{
//    VS_OUT vout = (VS_OUT)0;
    
//    float3 p = { 0, 0, 0 };
//    float3 n = { 0, 0, 0 };
//    for (int i = 0; i < 4; i++)
//    {
//        p += (vin.bone_weights[i] * mul(vin.position, bone_transforms[vin.bone_indices[i]])).xyz;
//        n += (vin.bone_weights[i] * mul(float4(vin.normal.xyz, 0), bone_transforms[vin.bone_indices[i]])).xyz;
//    }

//    vout.position = mul(float4(p, 1.0f), view_projection);

//    float3 N = normalize(n);
//    float3 L = normalize(-light_direction.xyz);
//    float d = dot(L, N);
//    vout.color.xyz = material_color.xyz * max(0, d);
//    vout.color.w = material_color.w;
//    vout.texcoord = vin.texcoord;
    
//    float3 wPos = vin.position;

//    float3 wN = mul((float3x3) world, vin.normal).xyz;
//    wN = normalize(wN);
    
//    vout.wNormal = wN;
//    vout.wPos = wPos;
    
//    return vout;
//}

//フォグ

//VS_OUT main(VS_IN vin)
//{
//    float3 p = { 0, 0, 0 };
//    float3 n = { 0, 0, 0 };
	
//    for (int i = 0; i < 4; i++)
//    {
//        p += (vin.bone_weights[i] * mul(vin.position, bone_transforms[vin.bone_indices[i]])).xyz;
//        n += (vin.bone_weights[i] * mul(float4(vin.normal.xyz, 0), bone_transforms[vin.bone_indices[i]])).xyz;
//    }

//    VS_OUT vout;
    
    
//    vout.position = mul(float4(p, 1.0f), rowCameraView);
    
//    float3 N = normalize(n);
//    float3 L = normalize(-lightDirection.xyz);
//    float d = dot(L, N);
    
//  //  ワールド座標

//    float4 P = float4(p, 1.0f);
//    float3 W = mul(world, P).xyz;
//    float3 E = normalize(EyePos.xyz - W);

//   // ワールド法線算出

//    float3 wN = mul((float3x3) world, vin.normal);
//    wN = normalize(wN);
    
//    vout.color.xyz = material_color.xyz * max(0, d);
//    vout.color.w = material_color.w;
//    vout.texcoord = vin.texcoord;

//    vout.wNormal = wN;
//    vout.wEyeDir = E;
//    vout.wPos = W;
    
//    return vout;
//}


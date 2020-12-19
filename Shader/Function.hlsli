
//--------------------------------------------
// 拡散反射関数（ランバートシェーディング）
//--------------------------------------------
// N:法線(正規化済み)
// L:入射ベクトル(正規化済み)
// C:入射光(色・強さ)
// K:反射率(0～1.0)
float3 Diffuse(float3 N, float3 L, float3 C, float3 K)
{
    float D = dot(N, -L);
    D = max(0, D); // 負の値を０にする
    return K * C * D;
}


//--------------------------------------------
// 鏡面反射関数(ブリン・フォン)
//--------------------------------------------
// N:法線(正規化済み)
// L:入射ベクトル(正規化済み)
// C:入射光(色・強さ)
// E:視点方向ベクトル(正規化済み)
// K:反射率(0～1.0)
// Power:ハイライトの強さ(輝き度)
float3 BlinnPhongSpecular(float3 N, float3 L, float3 C, float3 E,
float3 K, float Power)
{
//ハーフベクトル
    float3 H = normalize(-L + E);
    float3 S = dot(H, N);
    S = max(0, S);
    S = pow(S, Power);
    S = S * K * C;
    return S;
}

//--------------------------------------------
// フォグ
//--------------------------------------------
//color :現在の色
//viewPos :視点
//Pos :ピクセルの位置
//fogColor :フォグの色
//Near :フォグの近距離
//Far :フォグの遠距離
float3 Fog(float3 color, float3 viewPos, float3 Pos, float3 fogColor, float Near, float Far)
{
//フォグ計算
    float Len = length(viewPos - Pos);
    float fogalpha = saturate((Len - Near) / (Far - Near));
    return color.rgb * (1.0 - fogalpha) + fogColor * fogalpha;
}
//--------------------------------------------
// 半球ライト
//--------------------------------------------
//N： :法線
// SkyColor :空(上)色
// GroundColor :地面(下)色
float3 HemiSphereLight(float3 N, float3 SkyColor, float3 GroundColor)
{
    float skyblend = (N.y + 1.0) * 0.5;
    float groundblend = 1.0 - skyblend;
    return SkyColor * skyblend + GroundColor * groundblend;
}
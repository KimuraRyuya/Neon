
//--------------------------------------------
// �g�U���ˊ֐��i�����o�[�g�V�F�[�f�B���O�j
//--------------------------------------------
// N:�@��(���K���ς�)
// L:���˃x�N�g��(���K���ς�)
// C:���ˌ�(�F�E����)
// K:���˗�(0�`1.0)
float3 Diffuse(float3 N, float3 L, float3 C, float3 K)
{
    float D = dot(N, -L);
    D = max(0, D); // ���̒l���O�ɂ���
    return K * C * D;
}


//--------------------------------------------
// ���ʔ��ˊ֐�(�u�����E�t�H��)
//--------------------------------------------
// N:�@��(���K���ς�)
// L:���˃x�N�g��(���K���ς�)
// C:���ˌ�(�F�E����)
// E:���_�����x�N�g��(���K���ς�)
// K:���˗�(0�`1.0)
// Power:�n�C���C�g�̋���(�P���x)
float3 BlinnPhongSpecular(float3 N, float3 L, float3 C, float3 E,
float3 K, float Power)
{
//�n�[�t�x�N�g��
    float3 H = normalize(-L + E);
    float3 S = dot(H, N);
    S = max(0, S);
    S = pow(S, Power);
    S = S * K * C;
    return S;
}

//--------------------------------------------
// �t�H�O
//--------------------------------------------
//color :���݂̐F
//viewPos :���_
//Pos :�s�N�Z���̈ʒu
//fogColor :�t�H�O�̐F
//Near :�t�H�O�̋ߋ���
//Far :�t�H�O�̉�����
float3 Fog(float3 color, float3 viewPos, float3 Pos, float3 fogColor, float Near, float Far)
{
//�t�H�O�v�Z
    float Len = length(viewPos - Pos);
    float fogalpha = saturate((Len - Near) / (Far - Near));
    return color.rgb * (1.0 - fogalpha) + fogColor * fogalpha;
}
//--------------------------------------------
// �������C�g
//--------------------------------------------
//N�F :�@��
// SkyColor :��(��)�F
// GroundColor :�n��(��)�F
float3 HemiSphereLight(float3 N, float3 SkyColor, float3 GroundColor)
{
    float skyblend = (N.y + 1.0) * 0.5;
    float groundblend = 1.0 - skyblend;
    return SkyColor * skyblend + GroundColor * groundblend;
}
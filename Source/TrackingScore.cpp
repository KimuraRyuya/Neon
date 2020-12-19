#include "TrackingScore.h"
#include "ParticleSystem.h"

void TrackingScore::Initialize()
{
	trackingTex = std::make_unique<Billboard>(L"Data/Texture/player_shot.png");

	trackingPos = { 0.0f, 0.0f, 0.0f };
	for (int i = 0; i < Constant::MAX_TRACKINGSCORE; i++)
	{
		trackingInf[i].Clear();
	}
}

void TrackingScore::Reset()
{
	trackingPos = { 0.0f, 0.0f, 0.0f };
	for (int i = 0; i < Constant::MAX_TRACKINGSCORE; i++)
	{
		trackingInf[i].Clear();
	}
}

void TrackingScore::Update(float elapsedTime)
{
	for (int i = 0; i < Constant::MAX_TRACKINGSCORE; i++)
	{
		if (!trackingInf[i].isExist) continue;
		DirectX::XMFLOAT2 trackingVec = { trackingPos.x - trackingInf[i].pos.x, trackingPos.z * -1.0f - trackingInf[i].pos.z };
		DirectX::XMVECTOR _trackingVec = DirectX::XMLoadFloat2(&trackingVec);
		_trackingVec = DirectX::XMVector2Normalize(_trackingVec);
		DirectX::XMStoreFloat2(&trackingVec, _trackingVec);
		trackingInf[i].velocity = { trackingVec.x * 50.0f , 0, trackingVec.y * 50.0f };

		trackingInf[i].pos.x += trackingInf[i].velocity.x * elapsedTime;
		trackingInf[i].pos.z += trackingInf[i].velocity.z * elapsedTime;
		DirectX::XMFLOAT3 emitParticlePos = { trackingInf[i].pos.x, trackingInf[i].pos.y , trackingInf[i].pos.z * -1.0f };
		ParticleSystem::GetInstance()->trakingScoreLocus->Burst(1, emitParticlePos, elapsedTime, 0.0f, 0.0f);
	}
}

void TrackingScore::Render()
{
	trackingTex->Begin();
	for (int i = 0; i < Constant::MAX_TRACKINGSCORE; i++)
	{
		if (!trackingInf[i].isExist) continue;
		trackingTex->Render(trackingInf[i].pos, 0.0f, 0.0f, 256.0f, 256.0f, { 0.0f, 0.0f, 0.0f }, { 0.01f, 0.01f }, { 30.0f, 30.0f, 30.0f, 1.0f }, {1.0f, 1.0f, 1.0f, 1.0f});
	}
	trackingTex->End();
}

void TrackingScore::Finalize()
{

}

void TrackingScore::EmitTrackingScore(DirectX::XMFLOAT3 emitP)
{
	for (int i = 0; i < Constant::MAX_TRACKINGSCORE; i++)
	{
		if (trackingInf[i].isExist) continue;
		trackingInf[i].isExist = true;
		trackingInf[i].pos = emitP;
		trackingInf[i].pos.z *= -1;
		break;
	}
}

void TrackingScore::Destory(int i)
{
	trackingInf[i].Clear();
}
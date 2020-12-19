#include "PlayerShot.h"
#include "../Library/MyLib.h"
#include "../Library/Camera.h"
#include "../Library/FbxLoader.h"
#include "../Library/FrameWork.h"
#include "../Library/Easing.h"
#include <imgui.h>

void PlayerShotSystem::Initialize()
{
	const char* fbx_filename = "Data/PlayerShot/playerShot.fbx";
	std::unique_ptr<ModelData> model_data = std::make_unique<ModelData>();
	FbxLoader fbx_loader;
	fbx_loader.Load(fbx_filename, *model_data);

	modelResource = std::make_shared<ModelResource>(pFrameWork.GetDevice(), std::move(model_data), true);
	modelRenderer = std::make_unique<ModelRenderer>(pFrameWork.GetDevice());

	meshEmitParticleUpdateTime = 0;

	for (int i = 0; i < Constant::SHOT_MAX; i++)
	{
		shotObj[i].Initialize(modelResource);
		isExist[i] = false;
	}

}

void PlayerShotSystem::Reset()
{
	meshEmitParticleUpdateTime = 0;

	for (int i = 0; i < Constant::SHOT_MAX; i++)
	{
		shotObj[i].Initialize(modelResource);
		isExist[i] = false;
	}
}

void PlayerShotSystem::Update(float elapsedTime)
{
	// ƒVƒ‡ƒbƒg‚ÌˆÚ“®ˆ—
	for (int i = 0; i < Constant::SHOT_MAX; i++)
	{
		if (!isExist[i])
		{
			continue;
		}

		DirectX::XMFLOAT3 nextPos = {};
		nextPos.x = shotObj[i].GetPos().x + shotObj[i].GetVelocity().x * elapsedTime;
		nextPos.z = shotObj[i].GetPos().z + shotObj[i].GetVelocity().z * elapsedTime;

		shotObj[i].SetPos(nextPos);
	}

}

void PlayerShotSystem::Render()
{
	modelRenderer->Begin(pFrameWork.GetContext());
	for (int i = 0; i < Constant::SHOT_MAX; i++)
	{
		if (!isExist[i]) continue;
		shotObj[i].GiveModelInfo();
		modelRenderer->Draw(pFrameWork.GetContext(), shotObj[i].GetModel(), { 20.0f, 20.0f, 25.0f, 1.0f });
	}
	modelRenderer->End(pFrameWork.GetContext());
	
}

void PlayerShotSystem::Finalize()
{

}

void PlayerShotSystem::ShotBurst(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 vel, bool isReinforce)
{
	for (int i = 0; i < Constant::SHOT_MAX; i++)
	{
		if (isExist[i]) continue;
		shotObj[i].SetPos(pos);
		shotObj[i].SetVelocity(vel);
		if (isReinforce)
		{
			shotObj[i].SetScale({ 0.5f, 0.5f, 0.5f });
		}
		else
		{
			shotObj[i].SetScale({ 0.25f, 0.25f, 0.25f });
		}
		isExist[i] = true;

		float ang = atan2(shotObj[i].GetVelocity().z, shotObj[i].GetVelocity().x);
		shotObj[i].SetRotate({sin(ang) * -1, 0.0f, cos(ang)});
		break;
	}
}
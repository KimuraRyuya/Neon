#include "PlayerDestoryEffect.h"

#include "../Library/MyLib.h"
#include "../Library/Camera.h"
#include "../Library/FbxLoader.h"
#include "../Library/FrameWork.h"
#include "../Library/Easing.h"

void PlayerDestoryEffect::Initialize()
{

	const char* fbx_filename = "Data/Effect/PlayerDestoryEffect.fbx";
	std::unique_ptr<ModelData> model_data = std::make_unique<ModelData>();
	FbxLoader fbx_loader;
	fbx_loader.Load(fbx_filename, *model_data);

	modelResource = std::make_shared<ModelResource>(pFrameWork.GetDevice(), std::move(model_data), true);
	effectObj.Initialize(modelResource);
	effectObj.SetScale({ 0.0f, 0.0f, 0.0f });
	modelRenderer = std::make_unique<ModelRenderer>(pFrameWork.GetDevice());

	luminans = { 10.0f, 15.0f, 20.0f, 1.0f };

	scaleTime = 0;
	hasFinishSpawn = true;
}

void PlayerDestoryEffect::Reset()
{
	effectObj.SetScale({ 0.0f, 0.0f, 0.0f });

	luminans = { 10.0f, 15.0f, 20.0f, 1.0f };

	scaleTime = 0;
	hasFinishSpawn = true;
}

void PlayerDestoryEffect::Update(DirectX::XMFLOAT3 playerPos, bool playerNowSpawn, float elapsedTime)
{
	if (!playerNowSpawn)
	{
		// プレイヤーがスポーン中じゃなかったら常に座標を取得する
		effectObj.SetPos(playerPos);
		scaleTime = 0.0f;
		effectObj.SetScale({ 0.0f, 1.0f, 0.0f });
		hasFinishSpawn = false;
	}
	else
	{
		if (hasFinishSpawn) return;
		// スポーン中になったらeasingでスケールを大きくしてエフェクトを出す
		float scale = Easing::InOutSine(scaleTime, 1.0f, 44.0f * 2.0f, 0.0f);
		effectObj.SetScale({ scale, 1.0f, scale });
		scaleTime += elapsedTime;
		if (scaleTime >= 1.0f)
		{
			hasFinishSpawn = true;
		}
	}
}

void PlayerDestoryEffect::Render()
{

	modelRenderer->Begin(pFrameWork.GetContext());
	effectObj.GiveModelInfo();
	modelRenderer->Draw(pFrameWork.GetContext(), effectObj.GetModel(), luminans);
	modelRenderer->End(pFrameWork.GetContext());

}

void PlayerDestoryEffect::Finalize()
{



}


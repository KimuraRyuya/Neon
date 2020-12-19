#include "FloorFillEffect.h"

#include "../Library/MyLib.h"
#include "../Library/Camera.h"
#include "../Library/FbxLoader.h"
#include "../Library/FrameWork.h"
#include "../Library/Easing.h"

void FloorFillEffect::Initialize()
{
	// ÉÇÉfÉãÉçÅ[Éh
	effectTex = std::make_unique<Billboard>(L"Data/Texture/floorFillEffect.png");

	const char* fbx_filename = "Data/Effect/PlayerDestoryEffect.fbx";
	std::unique_ptr<ModelData> model_data = std::make_unique<ModelData>();
	FbxLoader fbx_loader;
	fbx_loader.Load(fbx_filename, *model_data);

	modelResource = std::make_shared<ModelResource>(pFrameWork.GetDevice(), std::move(model_data), true);
	for (int i = 0; i < Constant::MAX_FILLEFFECT; i++)
	{
		effectObj[i].Initialize(modelResource);
		effectObj[i].SetScale({ 0.0f, 0.0f, 0.0f });

		effetcInf[i].Clear();
	}
	modelRenderer = std::make_unique<ModelRenderer>(pFrameWork.GetDevice());

	luminans = { 10.0f, 15.0f, 20.0f, 1.0f };
}

void FloorFillEffect::Reset()
{
	for (int i = 0; i < Constant::MAX_FILLEFFECT; i++)
	{
		effectObj[i].SetScale({ 0.0f, 0.0f, 0.0f });

		effetcInf[i].Clear();
	}

	luminans = { 10.0f, 15.0f, 20.0f, 1.0f };

}

void FloorFillEffect::Update(DirectX::XMFLOAT3 p, float elapsedTime)
{
	for (int i = 0; i < Constant::MAX_FILLEFFECT; i++)
	{
		if (!effetcInf[i].isExist) continue;
		effectObj[i].SetPos(p);
		effectObj[i].SetPosZ(p.z * -1.0f);
		float s = Easing::InSine(effetcInf[i].scaleTime, 0.5f, effetcInf[i].maxScale, 0.0f);
		float a= Easing::InSine(effetcInf[i].scaleTime, 0.5f, 0.0f, 1.0f);
		effectObj[i].SetScale({s, 1.0f, s});
		effectObj[i].SetColorA(a);
		if (effetcInf[i].scaleTime > 0.5f)
		{
			effetcInf[i].Clear();
		}
		effetcInf[i].scaleTime += elapsedTime;
	}
}

void FloorFillEffect::Render()
{

	effectTex->Begin();
	for (int i = 0; i < Constant::MAX_FILLEFFECT; i++)
	{
		if (!effetcInf[i].isExist) continue;
		effectTex->Render(effectObj[i].GetPos(), 0.0f, 0.0f, 1024.0f, 1024.0f, { 0, 0, 0 }, {effectObj[i].GetScale().x, effectObj[i].GetScale().x}, luminans, effectObj[i].GetColor());
	}
	effectTex->End();

}

void FloorFillEffect::Finalize()
{

}

void FloorFillEffect::EmitEffect(DirectX::XMFLOAT3 p, float s)
{
	for (int i = 0; i < Constant::MAX_FILLEFFECT; i++)
	{
		if (effetcInf[i].isExist) continue;
		effetcInf[i].isExist = true;
		effetcInf[i].maxScale = s;
		effetcInf[i].scaleTime = 0.0f;
		effectObj[i].SetPos(p);
		effectObj[i].SetColorA(1.0f);
		break;
	}
}
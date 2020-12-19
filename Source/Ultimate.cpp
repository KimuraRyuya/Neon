#include "Ultimate.h"
#include "SceneManager.h"
#include "../Library/Easing.h"
#include <imgui.h>

void Ultimate::Initialize()
{
	ultimateCountDownTex = std::make_unique<Billboard>(L"Data/Texture/Ultimate.png");
	
	pos = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	angle = DirectX::XMFLOAT3(90.0f, 0.0f, 0.0f);
	scale = DirectX::XMFLOAT2(0.0f, 0.0f);
	alpha = 0.6f;
	nowScale = 0.0f;
	startScale = 0.0f;
	ultimateScalingTime = 0.0f;

	onUltimateCountDown = false;
	onceOnUltimateReset = false;
	onceOffUltimateReset = false;
	onUltimate = false;
}

void Ultimate::Reset()
{
	pos = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	angle = DirectX::XMFLOAT3(90.0f, 0.0f, 0.0f);
	scale = DirectX::XMFLOAT2(0.0f, 0.0f);
	alpha = 0.6f;
	nowScale = 0.0f;
	startScale = 0.0f;
	ultimateScalingTime = 0.0f;

	onUltimateCountDown = false;
	onceOnUltimateReset = false;
	onceOffUltimateReset = false;
	onUltimate = false;
}

void Ultimate::Update(float elapsedTime)
{
	if (onUltimateCountDown)
	{
		if (!onceOnUltimateReset)
		{
			startScale = nowScale;
			ultimateScalingTime = 0.0f;
			onceOnUltimateReset = true;
		}

		nowScale = Easing::OutSine(ultimateScalingTime, 10.0f, 70.0f, startScale);
		scale = DirectX::XMFLOAT2(nowScale, nowScale);
		ultimateScalingTime += elapsedTime;

		if (ultimateScalingTime > 10.0f)
		{
			nowScale = Easing::OutSine(10.0f, 10.0f, 70.0f, 0.0f);
			scale = DirectX::XMFLOAT2(nowScale, nowScale);
			onUltimate = true;
			ultimateScalingTime = 0.0f;
		}

		onceOffUltimateReset = false;
	}
	else
	{
		if (nowScale > 0)
		{
			if (!onceOffUltimateReset)
			{
				startScale = nowScale;
				ultimateScalingTime = 0.0f;
				onceOffUltimateReset = true;
			}

			float s = Easing::OutSine(ultimateScalingTime, 3.0f, 0.0f, nowScale);
			scale = DirectX::XMFLOAT2(s, s);
			ultimateScalingTime += elapsedTime;
			if (ultimateScalingTime > 3.0f)
			{
				nowScale = Easing::OutSine(3.0f, 3.0f, 0.0f, startScale);
				scale = DirectX::XMFLOAT2(nowScale, nowScale);
				ultimateScalingTime = 0.0f;
			}
		}
		onceOnUltimateReset = false;
	}

	ImGui::Begin("Ultimate");
	ImGui::DragFloat3("pos", &pos.x);
	ImGui::DragFloat3("angle", &angle.x);
	ImGui::DragFloat2("scale", &scale.x);
	ImGui::DragFloat("alpha", &alpha, 0.1);
	ImGui::End();

}

void Ultimate::Render()
{
	pSceneManager->blend_states[pSceneManager->ADD]->Activate(pFrameWork.GetContext());
	ultimateCountDownTex->Begin();
	ultimateCountDownTex->Render(pos, 0.0f, 0.0f, 1280.0f, 1280.0f, { angle.x * 0.01745f, angle.y * 0.01745f, angle.z * 0.01745f}, scale, { 5, 5, 5, alpha }, { 1, 1, 1, alpha }, false);
	ultimateCountDownTex->End();
	pSceneManager->blend_states[pSceneManager->ADD]->DeActivate(pFrameWork.GetContext());
}

void Ultimate::Finalize()
{

}

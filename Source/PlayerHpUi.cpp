#include "PlayerHpUi.h"
#include "SceneManager.h"
#include "../Library/FrameWork.h"

void PlayerHpUi::Initialize()
{
	playerCircleUiFrame = std::make_unique<Billboard>(L"Data/Texture/playerOriginUiFrame.png");
	playerCircleUiText = std::make_unique<Billboard>(L"Data/Texture/playerOriginUiText.png");
	playerCircleUi = std::make_unique<Billboard>(L"Data/Texture/playerOriginUi.png");
	playerCircleUi->DissolveTextureLoad(L"Data/Texture/playerUIDissolve.png");
	playerCircleUi->EmissiveTextureLoad(L"Data/Texture/playerUIEmissive.png");

	playerCircleUiPos = { 0.0f, 10.0f, 0.0f };

	playerHpUi = std::make_unique<Billboard>(L"Data/Texture/Hp.png");
	playerHpUiPos = { 0.0f, 10.0f, 0.0f };

	buttonUI = std::make_unique<Billboard>(L"Data/Texture/Button.png");
	
	LoadDivGraph(L"Data/Texture/Aiming.png", 1, 1, 1, 512, 512, &aimTexture);
	
	playerNowHp = 3;

	buttonAnim = 0.0f;

	buttonScaleTime = 0.0f;
	buttonScale = 0.0f;
	ultGauge = 0.0f;
	ang = 0.0f;
	isFloorUIDisply = false;
}

void PlayerHpUi::Reset()
{
	playerCircleUiPos = { 0.0f, 10.0f, 0.0f };

	playerHpUiPos = { 0.0f, 10.0f, 0.0f };

	playerNowHp = 3;
	ultGauge = 0.0f;
	buttonScaleTime = 0.0f;
	buttonScale = 0.0f;
	
	ang = 0.0f;

	buttonAnim = 0.0f;
	
	isFloorUIDisply = false;
}

void PlayerHpUi::Update(float elapsedTime)
{
	// アルティメットが使えるときのみUIを出す
	if (isFloorUIDisply)
	{
		buttonScale = sin(buttonScaleTime * 0.01745f) * 3.0f + 2.0f;

		buttonScaleTime += 300.0f  * elapsedTime;
		if (buttonScaleTime > 180.0f)
		{
			buttonScaleTime = buttonScaleTime - 180.0f;
		}
		buttonAnim += 10.0f * elapsedTime;

		if (buttonAnim > 2.0f)
		{
			buttonAnim = 0.0f;
		}

	}
	else
	{
		buttonAnim = 0.0f;
		buttonScaleTime = 0.0f;
		buttonScale = 0.0f;
	}

}

void PlayerHpUi::Render()
{
	GetInputMouse();

	pSceneManager->blend_states[pSceneManager->ALPHA]->Activate(pFrameWork.GetContext());
	ultGauge = std::fmin(ultGauge, 500.0);
	playerCircleUiFrame->Begin();
	playerCircleUiFrame->Render(playerCircleUiPos, 0.0f, 0.0f, 1980.0f, 1980.0f, { 0.0f, 0.0f, 180.0f * 0.01745f }, { 10.0f, 10.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f });
	playerCircleUiFrame->Render(playerCircleUiPos, 1980.0f, 0.0f, 1980.0f, 1980.0f, { 0.0f, 0.0f, 180.0f * 0.01745f }, { 10.0f, 10.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f });
	playerCircleUiFrame->End();

	playerCircleUiText->Begin();
	playerCircleUiText->Render(playerCircleUiPos, 0.0f, 0.0f, 1980.0f, 1980.0f, { 0.0f, 0.0f, 180.0f * 0.01745f }, { 10.0f, 10.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f });
	playerCircleUiText->Render(playerCircleUiPos, 1980.0f, 0.0f, 1980.0f, 1980.0f, { 0.0f, 0.0f, 180.0f * 0.01745f }, { 10.0f, 10.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f });
	playerCircleUiText->End();

	playerCircleUi->BeginDissolve();
	playerCircleUi->RenderDissolve(playerCircleUiPos, 0.0f, 0.0f, 1980.0f, 1980.0f, abs(1 - (ultGauge / 500.0f)), { 0.0f, 0.0f, 180.0f  * 0.01745f }, { 10.0f, 10.0f }, {1.0f, 1.0f, 1.0f, 1.0f}, { 0.5f, 0.5f, 0.5f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, 0.0f);
	playerCircleUi->RenderDissolve(playerCircleUiPos, 1980.0f, 0.0f, 1980.0f, 1980.0f, (ang / 360.0f), { 0.0f, 0.0f, 180.0f  * 0.01745f }, { 10.0f, 10.0f }, {1.0f, 1.0f, 1.0f, 1.0f}, { 0.5f, 0.5f, 0.5f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, 0.0f);
	playerCircleUi->End();


	playerHpUi->Begin();
	DirectX::XMFLOAT3 _hpPos =
	{
			playerHpUiPos.x + 5.2f,
			playerHpUiPos.y,
			playerHpUiPos.z - 4.5f
	};

	for (int i = 0; i < playerNowHp; i++)
	{
		playerHpUi->Render(_hpPos, 0.0f, 0.0f, 512.0f, 512.0f, { 0.0f, 0.0f, 0.0f }, { 10.0f, 10.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f });
		_hpPos.z += 1.3f;
	}
	playerHpUi->End();

	DirectX::XMFLOAT3 _buttonPos =
	{
		playerCircleUiPos.x - 6.0f,
		playerCircleUiPos.y,
		playerCircleUiPos.z - 4.0f
	};

	//buttonUI->Begin();
	//buttonUI->Render(_buttonPos, 512.0f * (2.0f + static_cast<int>(buttonAnim)), 512.0f * 2.0f, 512.0f, 512.0f, { 0.0f, 0.0f, 0.0f }, {buttonScale, buttonScale }, { 1.0f, 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f });
	//buttonUI->End();

	if (PAD->mouseIndex >= 0 && pFrameWork.GetOnGame())
	{
		aimTexture.spr->Begin();
		aimTexture.spr->DrawRota2(PAD->mb.X, PAD->mb.Y, 0.0f, 0.1f, aimTexture.inf[0]);
		aimTexture.spr->End();
	}

	pSceneManager->blend_states[pSceneManager->ALPHA]->DeActivate(pFrameWork.GetContext());
}

void PlayerHpUi::Finalize()
{
	if(aimTexture.spr) TextureRelease(&aimTexture);
}

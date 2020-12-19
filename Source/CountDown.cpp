#include "CountDown.h"
#include "../Library/MyLib.h"
#include "../Library/Camera.h"
#include "../Library/FbxLoader.h"
#include "../Library/FrameWork.h"
#include "../Library/Easing.h"
#include "Sound.h"
#include "ParticleSystem.h"
#include "SceneManager.h"

void CountDown::Initialize()
{
	countTex[0] = std::make_unique<Billboard>(L"Data/Texture/countGo.png");
	countTex[0]->DissolveTextureLoad(L"Data/Texture/countDownNoise.png");
	countTex[0]->EmissiveTextureLoad(L"Data/Texture/dissolve_edgecolor2.png");
	
	countTex[1] = std::make_unique<Billboard>(L"Data/Texture/count1.png");
	countTex[1]->DissolveTextureLoad(L"Data/Texture/countDownNoise.png");
	countTex[1]->EmissiveTextureLoad(L"Data/Texture/dissolve_edgecolor2.png");

	countTex[2] = std::make_unique<Billboard>(L"Data/Texture/count2.png");
	countTex[2]->DissolveTextureLoad(L"Data/Texture/countDownNoise.png");
	countTex[2]->EmissiveTextureLoad(L"Data/Texture/dissolve_edgecolor2.png");

	countTex[3] = std::make_unique<Billboard>(L"Data/Texture/count3.png");
	countTex[3]->DissolveTextureLoad(L"Data/Texture/countDownNoise.png");
	countTex[3]->EmissiveTextureLoad(L"Data/Texture/dissolve_edgecolor2.png");

	emitTimer = 0;
	displyNum = 0;
	dissolveScalar = 1.0f;
	changeNum = false;
	nowGamePlay = false;
	countStartFlg = false;
}

void CountDown::Reset()
{
	emitTimer = 0;
	displyNum = 0;
	dissolveScalar = 1.0f;
	changeNum = false;
	nowGamePlay = false;
	countStartFlg = false;
}

void CountDown::Update(std::shared_ptr<Player> player, float elapsedTime)
{
	// ゲームが開始されていたら return
	if (nowGamePlay) return;

	if (!countStartFlg)
	{
		// 1番最初の数字の時のみここで鳴らす
		Sound::GetInstance()->openDissolve->PlayWAV();
		countStartFlg = true;
	}

	// ディゾルブで表示/非表示をする
	if (emitTimer > 1.0f)
	{
		dissolveScalar = Easing::InQuad(emitTimer - 1.0f, 1.0f, 1.0f, 0.0f);
	}
	else
	{
		dissolveScalar = Easing::InQuad(emitTimer, 1.0f, 0.0f, 1.0f);
	}

	if (2.0f < emitTimer)
	{
		Sound::GetInstance()->openDissolve->PlayWAV();
		changeNum = false;
		emitTimer = 0;
		// 次の数字へ
		displyNum++;
		if (displyNum >= 4)
		{
			// カウントダウン終了時ゲーム開始
			nowGamePlay = true;
		}
	}
	emitTimer += 2.0f * elapsedTime;
}

void CountDown::Render()
{
	if (nowGamePlay) return;
	pSceneManager->blend_states[pSceneManager->ADD]->Activate(pFrameWork.GetContext());
	countTex[abs(displyNum - 3)]->BeginGlitch();
	countTex[abs(displyNum - 3)]->RenderGlitch({ 0.0f, -1.0f, 1.0f }, 0.0f, 0.0f, 520.0f, 520.0f, dissolveScalar, { 0.0f, 0.0f, 0.0f }, { 40.0f, 40.0f }, { 100.0f, 100.0f, 100.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, {1.0f, 1.0f, 1.0f, 1.0f});
	countTex[abs(displyNum - 3)]->End();
	pSceneManager->blend_states[pSceneManager->ADD]->DeActivate(pFrameWork.GetContext());
}

void CountDown::Finalize()
{

}

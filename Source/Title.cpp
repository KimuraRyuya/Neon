#include "Title.h"
#include "../Library/FbxLoader.h"
#include "../Library/FrameWork.h"
#include "../Library/MyLib.h"
#include "ParticleSystem.h"
#include <imgui.h>
#include <shellapi.h>
#include "Sound.h"
#include "SceneManager.h"

void Title::Initialize()
{

	titleTexure = std::make_unique<Billboard>(L"Data/Texture/Title.png");
	titleTexure->DissolveTextureLoad();
	titleTexure->EmissiveTextureLoad();
	pressATexure = std::make_unique<Billboard>(L"Data/Texture/pressA.png");
	pressATexure->DissolveTextureLoad();
	pressATexure->EmissiveTextureLoad();

	dissolve = 2.0f;
	addDissolve = -1.0f;
	hasOpenDissolveSound = true;
	titleTexPos = { 1.0f, 41.0f, -10.0f };

	bloomEffect = std::make_unique<Bloom>(pFrameWork.GetDevice(), pFrameWork.screenWidth, pFrameWork.screenHeight);
	framebuffers[0] = std::make_unique<Descartes::FrameBuffer>(pFrameWork.GetDevice(), pFrameWork.screenWidth, pFrameWork.screenHeight, false/*enable_msaa*/, 8, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R24G8_TYPELESS);
	framebuffers[1] = std::make_unique<Descartes::FrameBuffer>(pFrameWork.GetDevice(), pFrameWork.screenWidth, pFrameWork.screenHeight, false/*enable_msaa*/, 1, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R24G8_TYPELESS);
	tonemap_effect = std::make_unique<ToneMap>(pFrameWork.GetDevice());

	canNextSceneState = false;
}

void Title::Reset()
{
	
	dissolve = 2.0f;
	addDissolve = -1.0f;
	titleTexPos = { 1.0f, 41.0f, -10.0f };
	hasOpenDissolveSound = false;
	canNextSceneState = false;
}

SceneState Title::Update(float elapsedTime)
{
	GetInputXbox();

	if (!hasOpenDissolveSound)
	{
		Sound::GetInstance()->openDissolve->PlayWAV();
		hasOpenDissolveSound = true;
	}

	dissolve += addDissolve * elapsedTime;
	if (dissolve < 0)
	{
		dissolve = 0.0f;
	}
	else if (dissolve > 2.0f)
	{
		dissolve = 2.0f;
	}

	if (!canNextSceneState)
	{
		ParticleSystem::GetInstance()->titleParticle[0]->Burst(1, DirectX::XMFLOAT3(titleTexPos.x + 4.4f, titleTexPos.y, -titleTexPos.z), elapsedTime, 0.0f, 0.0f);
		ParticleSystem::GetInstance()->titleParticle[1]->Burst(1, DirectX::XMFLOAT3(titleTexPos.x - 4.4f, titleTexPos.y, -titleTexPos.z), elapsedTime, 0.0f, 0.0f);
	}

	// Aボタン押したらdissolve起動
	if (PAD->xb.A_BUTTON && !canNextSceneState && GetDissolve() == 0.0f)
	{
		Sound::GetInstance()->decision->PlayWAV();
		Sound::GetInstance()->closeDissolve->PlayWAV();

		canNextSceneState = true;
		SetAddDissolve(1);
	}
	// dissolveし終わったらメニューに遷移
	if (canNextSceneState && GetDissolve() >= 2.0f)
	{
		canNextSceneState = false;
		Reset();
		return SceneState::MENU;
	}
	Sound::GetInstance()->titleBgmVolume += 0.01 * elapsedTime;
	if (Sound::GetInstance()->titleBgmVolume > 1.0f) Sound::GetInstance()->titleBgmVolume = 1.0f;
	Sound::GetInstance()->titleBgm->SetVolume(Sound::GetInstance()->titleBgmVolume);

	return SceneState::TITLE;
}

void Title::Render(std::shared_ptr<BG> bg)
{
	framebuffers[0]->Clear(pFrameWork.GetContext(), 0.0f, 0.0f, 0.0f, 1.0f);
	framebuffers[0]->Activate(pFrameWork.GetContext());
	pSceneManager->blend_states[pSceneManager->ALPHA]->Activate(pFrameWork.GetContext());
	bg->Render();
	pSceneManager->blend_states[pSceneManager->ALPHA]->DeActivate(pFrameWork.GetContext());

	titleTexure->BeginGlitch();
	titleTexure->RenderGlitch(titleTexPos, 0.0f, 0.0f, 1080.0f, 1080.0f, dissolve, { 0.0f, 0.0f, 0.0f }, { 10.0f, 10.0f }, { 50.0f, 50.0f, 50.0f, 1.0f }, {0.0f, 0.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f});
	titleTexure->End();

	pressATexure->BeginGlitch();
	DirectX::XMFLOAT3 _pressAPos = {
		titleTexPos.x,
		titleTexPos.y - 3.0f,
		titleTexPos.z
	};

	pressATexure->RenderGlitch(_pressAPos, 0.0f, 0.0f, 512.0f, 256.0f, dissolve, { 0.0f, 0.0f, 0.0f }, { 4.0f, 2.0f }, { 50.0f, 50.0f, 50.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f });
	pressATexure->End();

	ParticleSystem::GetInstance()->BloomRender();

	framebuffers[0]->DeActivate(pFrameWork.GetContext());

	///generate bloom texture from scene framebuffer
	bloomEffect->generate(pFrameWork.GetContext(), framebuffers[0]->renderTargetShaderResourceView.Get());
	//convolute bloom texture to scene framebuffer
	framebuffers[1]->Clear(pFrameWork.GetContext(), 0.0f, 0.0f, 0.0f, 1.0f);
	framebuffers[1]->Activate(pFrameWork.GetContext());
	bloomEffect->Blit(pFrameWork.GetContext());
	framebuffers[1]->DeActivate(pFrameWork.GetContext());
	tonemap_effect->Blit(pFrameWork.GetContext(), framebuffers[1]->renderTargetShaderResourceView.Get(), pFrameWork.GetElapsedTime(), true);


}

void Title::Finalize()
{

}
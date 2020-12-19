#include "Menu.h"
#include "../Library/FrameWork.h"
#include "../Library/MyLib.h"
#include "../Library/Camera.h"
#include "../Library/Easing.h"
#include "Sound.h"
#include "SceneManager.h"

void Menu::Initialize()
{
	bloomEffect = std::make_unique<Bloom>(pFrameWork.GetDevice(), pFrameWork.screenWidth, pFrameWork.screenHeight);
	framebuffers[0] = std::make_unique<Descartes::FrameBuffer>(pFrameWork.GetDevice(), pFrameWork.screenWidth, pFrameWork.screenHeight, false/*enable_msaa*/, 8, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R24G8_TYPELESS);
	framebuffers[1] = std::make_unique<Descartes::FrameBuffer>(pFrameWork.GetDevice(), pFrameWork.screenWidth, pFrameWork.screenHeight, false/*enable_msaa*/, 1, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R24G8_TYPELESS);
	tonemap_effect = std::make_unique<ToneMap>(pFrameWork.GetDevice());

	canNextSceneState = false;

	LoadDivGraph(L"Data/Texture/menu.png", 5, 1, 5, 2000, 320, &menuText);
	menuText.spr->DissolveTextureLoad(L"Data/Texture/pauseDissolve.png");
	menuText.spr->EmissiveTextureLoad(L"Data/Texture/dissolve_edgecolor1.png");

	LoadDivGraph(L"Data/Texture/pauseBack.png", 1, 1, 1, 1920, 1080, &menuBack);
	LoadDivGraph(L"Data/Texture/pauseCursol.png", 1, 1, 1, 520, 320, &menuCursol);

	LoadDivGraph(L"Data/Texture/menuFrame.png", 1, 1, 1, 2500, 1380, &menuFrame);
	menuFrame.spr->DissolveTextureLoad(L"Data/Texture/pauseDissolve.png");
	menuFrame.spr->EmissiveTextureLoad(L"Data/Texture/dissolve_edgecolor1.png");
	
	LoadDivGraph(L"Data/Texture/rankingFrame.png", 1, 1, 1, 2500, 1600, &rankingFrame);
	rankingFrame.spr->DissolveTextureLoad(L"Data/Texture/rankingDissolve.png");
	rankingFrame.spr->EmissiveTextureLoad(L"Data/Texture/dissolve_edgecolor1.png");

	LoadDivGraph(L"Data/Texture/new_number.png", 10, 10, 1, 128, 256, &scoreNumText);

	dissolve = 2.0f;
	addDissolve = -1.0f;
	nowMode = 0;
	tmpMode = 0;
	inputTrg = false;

	targetCursolPos[0] = DirectX::XMFLOAT2(585.0f, 495.0f);
	targetCursolPos[1] = DirectX::XMFLOAT2(705.0f, 595.0f);
	targetCursolPos[2] = DirectX::XMFLOAT2(715.0f, 695.0f);
	targetCursolPos[3] = DirectX::XMFLOAT2(475.0f, 795.0f);
	cursolPos = targetCursolPos[0];

	hasOpenDissolveSound = false;
	hasCursolMove = false;
	cursolMoveTime = 0.0f;
	scoreTextAlpha = 1.0f;
}

void Menu::Reset()
{
	nowMode = 0;
	tmpMode = 0;
	inputTrg = false;
	dissolve = 2.0f;
	addDissolve = -1.0f;
	hasOpenDissolveSound = false;

	hasCursolMove = false;
	cursolMoveTime = 0.0f;

	targetCursolPos[0] = DirectX::XMFLOAT2(585.0f, 490.0f);
	targetCursolPos[1] = DirectX::XMFLOAT2(705.0f, 600.0f);
	targetCursolPos[2] = DirectX::XMFLOAT2(715.0f, 700.0f);
	targetCursolPos[3] = DirectX::XMFLOAT2(475.0f, 800.0f);
	cursolPos = targetCursolPos[0];

	scoreTextAlpha = 1.0f;
}

SceneState Menu::Update(float elapsedTime)
{
	GetInputXbox();
	// 最初のdissolve表示のサウンドはここで鳴らす
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
	// 少し余裕を持たすために1秒多くにした
	else if (dissolve > 2.0f)
	{
		dissolve = 2.0f;
	}

	// パッド入力 スティック上 十字キー上
	if (PAD->xb.DPAD_UP && !inputTrg && dissolve == 0.0f && !hasCursolMove && nowMode > 0)
	{
		Sound::GetInstance()->menuSelect->PlayWAV();
		hasCursolMove = true;
		tmpMode = nowMode;
		nowMode--;
		inputTrg = true;
		if (nowMode < 0) nowMode = 0;
	}
	// パッド入力 スティック下 十字キー下
	else if (PAD->xb.DPAD_DOWN && !inputTrg && dissolve == 0.0f && !hasCursolMove && nowMode < 3)
	{
		Sound::GetInstance()->menuSelect->PlayWAV();
		hasCursolMove = true;
		tmpMode = nowMode;
		nowMode++;
		inputTrg = true;
		if (nowMode > 3) nowMode = 3;
	}
	// どちらも押されていないときトリガーをfalseにする
	else if (!PAD->xb.DPAD_DOWN && !PAD->xb.DPAD_UP && inputTrg)
	{
		inputTrg = false;
	}

	// 上または下入力時のCursorの移動処理
	if (hasCursolMove)
	{
		cursolPos.x = Easing::OutSine(cursolMoveTime, 0.1f, targetCursolPos[nowMode].x, targetCursolPos[tmpMode].x);
		cursolPos.y = Easing::OutSine(cursolMoveTime, 0.1f, targetCursolPos[nowMode].y, targetCursolPos[tmpMode].y);
				
		cursolMoveTime += elapsedTime;
		if (cursolMoveTime > 0.1f)
		{
			cursolPos.x = Easing::OutSine(0.1f, 0.1f, targetCursolPos[nowMode].x, targetCursolPos[tmpMode].x);
			cursolPos.y = Easing::OutSine(0.1f, 0.1f, targetCursolPos[nowMode].y, targetCursolPos[tmpMode].y);
			cursolMoveTime = 0.0f;
			hasCursolMove = false;
		}
	}

	// セレクトの処理
	if (PAD->xb.A_BUTTON && !canNextSceneState && GetDissolve() == 0.0f)
	{
		Sound::GetInstance()->decision->PlayWAV();
		Sound::GetInstance()->closeDissolve->PlayWAV();

		canNextSceneState = true;
		SetAddDissolve(1.0f);
	}

	// 現在のモードによって何をするか決める
	if (canNextSceneState && GetDissolve() >= 2.0f)
	{
		// チュートリアルに移行
		if (GetNowMode() == 0)
		{
			Sound::GetInstance()->titleBgmVolume -= 0.1 * elapsedTime;
			if (Sound::GetInstance()->titleBgmVolume < 0.0f) Sound::GetInstance()->titleBgmVolume = 0.0f;
			Sound::GetInstance()->titleBgm->SetVolume(Sound::GetInstance()->titleBgmVolume);

			Camera::GetInstance().CameraMove(titleCameraPos, gameCameraPos, titleCameraTarget, gameCameraTarget, cameraMoveTime);
			cameraMoveTime += elapsedTime;
			if (cameraMoveTime > 2.0f)
			{
				Camera::GetInstance().CameraMove(titleCameraPos, gameCameraPos, titleCameraTarget, gameCameraTarget, 2.0f);
				cameraMoveTime = 0.0f;
				canNextSceneState = false;
				Reset();

				Sound::GetInstance()->gameBgm->PlayWAV();
				Sound::GetInstance()->gameBgm->SetVolume(Sound::GetInstance()->gameBgmVolume);
				SetOpenDissolveSound(false);
				return SceneState::TUTORIAL;
			}
		}
		// ノーマルゲームに移行
		else if (GetNowMode() == 1)
		{
			Sound::GetInstance()->titleBgmVolume -= 0.1 * elapsedTime;
			if (Sound::GetInstance()->titleBgmVolume < 0.0f) Sound::GetInstance()->titleBgmVolume = 0.0f;
			Sound::GetInstance()->titleBgm->SetVolume(Sound::GetInstance()->titleBgmVolume);

			Camera::GetInstance().CameraMove(titleCameraPos, gameCameraPos, titleCameraTarget, gameCameraTarget, cameraMoveTime);
			cameraMoveTime += elapsedTime;
			if (cameraMoveTime > 2.0f)
			{
				Camera::GetInstance().CameraMove(titleCameraPos, gameCameraPos, titleCameraTarget, gameCameraTarget, 2.0f);
				cameraMoveTime = 0.0f;
				canNextSceneState = false;
				Reset();

				Sound::GetInstance()->gameBgm->PlayWAV();
				Sound::GetInstance()->gameBgm->SetVolume(Sound::GetInstance()->gameBgmVolume);
				SetOpenDissolveSound(false);
				return SceneState::COUNT;
			}
		}
		// エンドレスゲームに移行
		else if (GetNowMode() == 2)
		{
			Sound::GetInstance()->titleBgmVolume -= 0.1 * elapsedTime;
			if (Sound::GetInstance()->titleBgmVolume < 0.0f) Sound::GetInstance()->titleBgmVolume = 0.0f;
			Sound::GetInstance()->titleBgm->SetVolume(Sound::GetInstance()->titleBgmVolume);

			Camera::GetInstance().CameraMove(titleCameraPos, gameCameraPos, titleCameraTarget, gameCameraTarget, cameraMoveTime);
			cameraMoveTime += elapsedTime;
			if (cameraMoveTime > 2.0f)
			{
				Camera::GetInstance().CameraMove(titleCameraPos, gameCameraPos, titleCameraTarget, gameCameraTarget, 2.0f);
				cameraMoveTime = 0.0f;
				canNextSceneState = false;
				Reset();

				Sound::GetInstance()->gameBgm->PlayWAV();
				Sound::GetInstance()->gameBgm->SetVolume(Sound::GetInstance()->gameBgmVolume);
				SetOpenDissolveSound(false);
				return SceneState::COUNT;
			}
		}
		// タイトルに移行
		else if (GetNowMode() == 3)
		{
			cameraMoveTime = 0.0f;
			canNextSceneState = false;
			Reset();
			return SceneState::TITLE;
		}
	}

	Sound::GetInstance()->titleBgmVolume += 0.01 * elapsedTime;
	if (Sound::GetInstance()->titleBgmVolume > 1.0f) Sound::GetInstance()->titleBgmVolume = 1.0f;
	Sound::GetInstance()->titleBgm->SetVolume(Sound::GetInstance()->titleBgmVolume);
	
	return SceneState::MENU;
}

void Menu::Render(std::shared_ptr<BG> bg)
{
	framebuffers[0]->Clear(pFrameWork.GetContext(), 0.0f, 0.0f, 0.0f, 1.0f);
	framebuffers[0]->Activate(pFrameWork.GetContext());

	pSceneManager->blend_states[pSceneManager->ALPHA]->Activate(pFrameWork.GetContext());
	bg->Render();
	pSceneManager->blend_states[pSceneManager->ALPHA]->DeActivate(pFrameWork.GetContext());

	framebuffers[0]->DeActivate(pFrameWork.GetContext());

	///generate bloom texture from scene framebuffer
	bloomEffect->generate(pFrameWork.GetContext(), framebuffers[0]->renderTargetShaderResourceView.Get());
	//convolute bloom texture to scene framebuffer
	framebuffers[1]->Clear(pFrameWork.GetContext(), 0.0f, 0.0f, 0.0f, 1.0f);
	framebuffers[1]->Activate(pFrameWork.GetContext());
	bloomEffect->Blit(pFrameWork.GetContext());
	framebuffers[1]->DeActivate(pFrameWork.GetContext());
	tonemap_effect->Blit(pFrameWork.GetContext(), framebuffers[1]->renderTargetShaderResourceView.Get(), pFrameWork.GetElapsedTime(), true);


	menuFrame.spr->BeginDissolve();
	menuFrame.spr->DrawDissolve(250.0f, 200.0f + (3.0f * 100.0f), 0, 0.4f, menuFrame.inf[0], dissolve, 1.0f);
	menuFrame.spr->End();

	menuText.spr->BeginDissolve();
	menuText.spr->DrawDissolve(200.0f, 230.0f, 0, 0.3f, menuText.inf[0], dissolve, 1.0f);
	for (int i = 1; i < 5; i++)
	{
		menuText.spr->DrawDissolve(200.0f, 250.0f + (i * 100.0f), 0, 0.2f, menuText.inf[i], dissolve, 1.0f);
	}
	menuText.spr->End();

	menuCursol.spr->Begin();
	menuCursol.spr->DrawRota(cursolPos.x, cursolPos.y, 0.0f, 0.1f, menuCursol.inf[0], { 1.0f, 1.0f, 1.0f, abs(dissolve - 2.0f) / 2.0f });
	menuCursol.spr->End();

	rankingFrame.spr->BeginDissolve();
	rankingFrame.spr->DrawDissolve(1920.0f - 250.0f, 200.0f + (3.0f * 100.0f), 0, 0.4f, rankingFrame.inf[0], dissolve, 1.0f);
	rankingFrame.spr->End();

	scoreNumText.spr->Begin();
	{
		if (GetNowMode() == 1)
		{
				for (int i = 0; i < 5; i++)
				{
					for (int j = 0; j < normalModeScore.scoreDigit[i]; j++)
					{
						scoreNumText.spr->DrawRota(1920.0f - 500.0f + (48.0f * j), 445.0f + (90.0f * i), 0.0f, 0.3f, scoreNumText.inf[normalModeScore.scoreframe[i][j]], { 1.0f, 1.0f, 1.0f, abs(dissolve - 2.0f) / 2.0f });
					}
				}
		}
		else if (GetNowMode() == 2)
		{
			for (int i = 0; i < 5; i++)
			{
				for (int j = 0; j < endlessModeScore.scoreDigit[i]; j++)
				{
					scoreNumText.spr->DrawRota(1920.0f - 500.0f + (48.0f * j), 445.0f + (90.0f * i), 0.0f, 0.3f, scoreNumText.inf[endlessModeScore.scoreframe[i][j]], { 1.0f, 1.0f, 1.0f, abs(dissolve - 2.0f) / 2.0f });
				}
			}
		}
	}
	scoreNumText.spr->End();


}

void Menu::ReceiveScoreValue(int* normalHiScore, int* endlessHiScore)
{
	for (int i = 0; i < 5; i++)
	{
		normalModeScore.bestScore[i] = normalHiScore[i];
		endlessModeScore.bestScore[i] = endlessHiScore[i];

		normalModeScore.scoreDigit[i] = AnimationFrameCalc(normalModeScore.bestScore[i], normalModeScore.scoreframe[i]);
		endlessModeScore.scoreDigit[i] = AnimationFrameCalc(endlessModeScore.bestScore[i], endlessModeScore.scoreframe[i]);

	}
}

void Menu::Finalize()
{
	TextureRelease(&rankingFrame);
	TextureRelease(&menuFrame);
	TextureRelease(&menuText);
	TextureRelease(&menuCursol);
	TextureRelease(&menuBack);
	TextureRelease(&scoreNumText);
}
#include"Pause.h"
#include "SceneManager.h"
#include "../Library/Easing.h"
#include "../Library/Camera.h"
#include "Sound.h"

void Pause::Initialize()
{
	LoadDivGraph(L"Data/Texture/paused.png", 4, 1, 4, 2000, 320, &pauseText);
	pauseText.spr->DissolveTextureLoad(L"Data/Texture/pauseDissolve.png");
	pauseText.spr->EmissiveTextureLoad(L"Data/Texture/dissolve_edgecolor1.png");

	LoadDivGraph(L"Data/Texture/pauseBack.png", 1, 1, 1, 1920, 1080, &pauseBack);
	LoadDivGraph(L"Data/Texture/pauseCursol.png", 1, 1, 1, 520, 320, &pauseCursol);

	LoadDivGraph(L"Data/Texture/pausedFrame.png", 1, 1, 1, 2500, 1380, &pauseFrame);
	pauseFrame.spr->DissolveTextureLoad(L"Data/Texture/pauseDissolve.png");
	pauseFrame.spr->EmissiveTextureLoad(L"Data/Texture/dissolve_edgecolor1.png");

	pauseTrg = false;
	nowPaused = false;
	isSelect = false;
	hasCursolMove = false;
	cursolMoveTrg = false;
	hasMenuTransition = false;
	hasRestartTransition = false;
	hasTitleTransition = false;
	dissolve = 1.0f;
	alpha = 0.0f;

	nowMode = 0;
	nextMode = 0;

	targetCursolPos[0] = DirectX::XMFLOAT2(715.0f, 546.0f);
	targetCursolPos[1] = DirectX::XMFLOAT2(576.0f, 647.0f);
	targetCursolPos[2] = DirectX::XMFLOAT2(475.0f, 744.0f);
	cursolPos = targetCursolPos[0];
	cursolMoveTime = 0.0f;
}


void Pause::Reset()
{
	pauseTrg = false;
	nowPaused = false;
	isSelect = false;
	hasCursolMove = false;
	cursolMoveTrg = false;
	hasMenuTransition = false;
	hasRestartTransition = false;
	hasTitleTransition = false;
	dissolve = 1.0f;
	alpha = 0.0f;

	nowMode = 0;
	nextMode = 0;

	targetCursolPos[0] = DirectX::XMFLOAT2(715.0f, 546.0f);
	targetCursolPos[1] = DirectX::XMFLOAT2(576.0f, 647.0f);
	targetCursolPos[2] = DirectX::XMFLOAT2(475.0f, 744.0f);
	cursolPos = targetCursolPos[0];
	cursolMoveTime = 0.0f;
}

void Pause::Update(float elapsedTime)
{
	GetInputXbox();

	// 現在ポーズ状態かどうか
	if (nowPaused)
	{
		// dissolveで表示しきったらセレクト開始
		if (!isSelect)
		{
			dissolve -= 2.0f * elapsedTime;
			alpha += 1.0f * elapsedTime;

			if (alpha > 0.5f) alpha = 0.5f;
			
			if (dissolve < 0.0f)
			{
				dissolve = 0.0f;
				isSelect = true;
			}
		}
		else
		{
			// パッドの下入力
			if (PAD->xb.DPAD_DOWN && !hasCursolMove && !cursolMoveTrg)
			{
				// モードが3つしかないからnextModeが2ならそれ以降下に行かないようにする
				if (nextMode < 2)
				{
					hasCursolMove = true;
					nextMode++;
				}
				Sound::GetInstance()->menuSelect->PlayWAV();
				cursolMoveTrg = true;
			}	
			// パッドの上入力
			else if (PAD->xb.DPAD_UP && !hasCursolMove && !cursolMoveTrg)
			{
				// nextModeが0より下はないので
				if (nextMode > 0)
				{
					hasCursolMove = true;
					nextMode--;
				}
				Sound::GetInstance()->menuSelect->PlayWAV();
				cursolMoveTrg = true;
			}
			// トリガー処理
			else if (!PAD->xb.DPAD_UP && !PAD->xb.DPAD_DOWN && cursolMoveTrg)
			{
				cursolMoveTrg = false;
			}

			// カーソルの移動処理
			if (hasCursolMove)
			{
				cursolPos.x = Easing::OutSine(cursolMoveTime, 0.1f, targetCursolPos[nextMode].x, targetCursolPos[nowMode].x);
				cursolPos.y = Easing::OutSine(cursolMoveTime, 0.1f, targetCursolPos[nextMode].y, targetCursolPos[nowMode].y);

				cursolMoveTime += elapsedTime;

				if (cursolMoveTime > 0.1f)
				{
					nowMode = nextMode;
					cursolMoveTime = 0.0f;
					hasCursolMove = false;
				}
			}

			// セレクト処理 モードによって遷移を決める
			if (PAD->xb.A_BUTTON && !hasCursolMove && !cursolMoveTrg)
			{
				Sound::GetInstance()->decision->PlayWAV();
				// ゲームリスタート
				if (nowMode == 0)
				{
					Camera::GetInstance().SetTempPos(Camera::GetInstance().GetPos());
					Camera::GetInstance().SetTempTarget(Camera::GetInstance().GetTarget());
					pauseTrg = true;
					nowPaused = false;
					isSelect = false;
					hasRestartTransition = true;
				}
				// メニューへ遷移
				else if(nowMode == 1)
				{
					pauseTrg = true;
					nowPaused = false;
					isSelect = false;
					hasMenuTransition = true;
				}
				// タイトルに遷移
				else
				{
					pauseTrg = true;
					nowPaused = false;
					isSelect = false;
					hasTitleTransition = true;
				}
			}
			
			// ポーズボタンを押されたらゲームを続きからさせる
			if (PAD->xb.MENU_BUTTON && !pauseTrg)
			{
				pauseTrg = true;
				nowPaused = false;
				isSelect = false;
			}
			else if (!PAD->xb.MENU_BUTTON && pauseTrg)
			{
				pauseTrg = false;
			}
		}

	}
	else
	{
		dissolve += 2.0f * elapsedTime;
		alpha -= 1.0f * elapsedTime;
		if (alpha < 0.0f) alpha = 0.0f;
		if (dissolve > 1.0f)
		{
			dissolve = 1.0f;
		}
		isSelect = false;
	}


}

void Pause::Render()
{
	pSceneManager->blend_states[pSceneManager->ALPHA]->Activate(pFrameWork.GetContext());
	pauseBack.spr->Begin();
	pauseBack.spr->DrawRota(1920.0f / 2.0f, 1080.0f / 2.0f, 0.0f, 1.0f, pauseBack.inf[0], DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, alpha));
	pauseBack.spr->End();

	pauseFrame.spr->BeginDissolve();
	pauseFrame.spr->DrawDissolve(250.0f, 200.0f + (3.0f * 100.0f), 0, 0.4f, pauseFrame.inf[0], dissolve, 1.0f);
	pauseFrame.spr->End();

	pauseText.spr->BeginDissolve();
	pauseText.spr->DrawDissolve(200.0f, 280.0f, 0, 0.3f, pauseText.inf[0], dissolve, 1.0f);
	for (int i = 1; i < 4; i++)
	{
		pauseText.spr->DrawDissolve(200.0f, 300.0f + (i * 100.0f), 0, 0.2f, pauseText.inf[i], dissolve, 1.0f);
	}
	pauseText.spr->End();

	if (nowPaused)
	{
		pauseCursol.spr->Begin();
		pauseCursol.spr->DrawRota(cursolPos.x, cursolPos.y, 0.0f, 0.1f, pauseCursol.inf[0], DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, alpha + 0.5f));
		pauseCursol.spr->End();
	}

	pSceneManager->blend_states[pSceneManager->ALPHA]->DeActivate(pFrameWork.GetContext());
}

void Pause::Finalize()
{
	TextureRelease(&pauseText);
	TextureRelease(&pauseFrame);
	TextureRelease(&pauseCursol);
	TextureRelease(&pauseBack);
}


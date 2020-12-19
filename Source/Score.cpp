#include "Score.h"
#include <imgui.h>
#include "../Library/Camera.h"

void Score::Initialize()
{
	LoadDivGraph(L"Data/Texture/ScoreText.png", 1, 1, 1, 1280, 256, &scoreText);
	LoadDivGraph(L"Data/Texture/new_number.png", 10, 10, 1, 128, 256, &scoreNumText);
	LoadDivGraph(L"Data/Texture/result.png", 1, 1, 1, 1920, 1080, &resultBack);
	LoadDivGraph(L"Data/Texture/pressA.png", 1, 1, 1, 512, 256, &pressA);
	LoadDivGraph(L"Data/Texture/resultBase.png", 40, 8, 5, 274, 270, &resultBase);

	dissolveScaler = 2.0f;
	addDissolveScaler = -1.0f;
	nowScore = 0.0f;
	scoreNumTextDigit = 1;
	tmpScore = 0;
	addScore = 1.0f;
	addScoreTotalTimer = 2.0f;
	pressAAlpha = 1.0f;
	isPressABlinking = false;
	addScoreTimer = 0.0f;

	for (int h = 0; h < 5; h++)
	{
		for (int w = 0; w < 8; w++)
		{
			if (h % 2 == 0)  resultBasePos[w][h] = DirectX::XMFLOAT2(136.0f + 236.0f * w, 130.0f + h * 201.0f);
			else resultBasePos[w][h] = DirectX::XMFLOAT2(253.0f + 236.0f * w, 130.0f + h * 201.0f);

			resultStartPos[w][h] = DirectX::XMFLOAT2(960.0f + static_cast<float>(rand() % 2400 - 1200), 540.0f + static_cast<float>(rand() % 1500 - 750));
			resultPos[w][h] = resultStartPos[w][h];
			resultTime[w][h] = 0.0f;
			resultTotalTime[w][h] = static_cast<float>(rand() % 100 + 50) / 100.0f;
		}
	}


}

void Score::Reset()
{
	dissolveScaler = 2.0f;
	nowScore = 0.0f;
	scoreNumTextDigit = 1;
	tmpScore = 0;
	addScore = 1.0f;
	addScoreTotalTimer = 2.0f;
	addDissolveScaler = -1.0f;
	addScoreTimer = 0.0f;
	pressAAlpha = 1.0f;
	isPressABlinking = false;
	scoreNumTextDigit = AnimationFrameCalc((int)nowScore, scoreNumTextFrame);

	for (int h = 0; h < 5; h++)
	{
		for (int w = 0; w < 8; w++)
		{
			if (h % 2 == 0)  resultBasePos[w][h] = DirectX::XMFLOAT2(136.0f + 236.0f * w, 130.0f + h * 201.0f);
			else resultBasePos[w][h] = DirectX::XMFLOAT2(253.0f + 236.0f * w, 130.0f + h * 201.0f);

			resultStartPos[w][h] = DirectX::XMFLOAT2(960.0f + static_cast<float>(rand() % 2400 - 1200), 540.0f + static_cast<float>(rand() % 1500 - 750));
			resultPos[w][h] = resultStartPos[w][h];
			resultTime[w][h] = 0.0f;
			resultTotalTime[w][h] = static_cast<float>(rand() % 100 + 50) / 100.0f;
		}
	}

}

void Score::Update(bool finishFlg, float elapsedTime)
{
	// ゲーム終了したかどうか
	if (finishFlg)
	{
		// 現在のスコアを代入
		nowScore = tmpScore;
		scoreNumTextDigit = AnimationFrameCalc((int)nowScore, scoreNumTextFrame);
		dissolveScaler += addDissolveScaler * elapsedTime;
		
		if (dissolveScaler >= 2.0f)
		{
			dissolveScaler = 2.0f;
		}
		else if (dissolveScaler <= 0.0f)
		{
			dissolveScaler = 0.0f;
		}

		// dissolve表示終わってたら
		if (abs(dissolveScaler - 2.0f) / 2.0f >= 1.0f)
		{
			// PRESS AのUIを点滅させる
			if (!isPressABlinking)
			{
				pressAAlpha -= 3.0f * elapsedTime;
			}
			else
			{
				pressAAlpha += 3.0f * elapsedTime;
			}
			if (pressAAlpha > 1.0f) isPressABlinking = false;
			else if (pressAAlpha < 0) isPressABlinking = true;

			for (int h = 0; h < 5; h++)
			{
				for (int w = 0; w < 8; w++)
				{
					if (resultTime[w][h] < resultTotalTime[w][h])
					{
						float x = Easing::OutCirc(resultTime[w][h], resultTotalTime[w][h], resultBasePos[w][h].x, resultStartPos[w][h].x);
						float y = Easing::OutCirc(resultTime[w][h], resultTotalTime[w][h], resultBasePos[w][h].y, resultStartPos[w][h].y);
						resultPos[w][h] = DirectX::XMFLOAT2(x, y);
						resultTime[w][h] += elapsedTime;
					}
					else
					{
						float x = Easing::OutCirc(resultTotalTime[w][h], resultTotalTime[w][h], resultBasePos[w][h].x, resultStartPos[w][h].x);
						float y = Easing::OutCirc(resultTotalTime[w][h], resultTotalTime[w][h], resultBasePos[w][h].y, resultStartPos[w][h].y);
						resultPos[w][h] = DirectX::XMFLOAT2(x, y);
					}
				}
			}

		}
		return;
	}

	// スコアを上げる
	if (tmpScore > nowScore)
	{
		addScore = tmpScore / 5.0f;
		nowScore += addScore * elapsedTime;
	}
	else
	{
		addScore = 1.0f;
	}
	if (tmpScore < nowScore) nowScore = tmpScore;

	// 限界値を超えないようにする
	if (nowScore > 99999999.0f) nowScore = 99999999.0f;

	scoreNumTextDigit = AnimationFrameCalc((int)nowScore, scoreNumTextFrame);



#ifdef _DEBUG

	ImGui::Begin("Score");

	ImGui::Text("Score##Num : %f", nowScore);
	ImGui::Text("tmpScore##Num : %f", tmpScore);
	ImGui::Text("addScore##Num : %f", addScore);

	ImGui::End();
#endif // _DEBUG


}

void Score::DissolveUpdate(float elapsedTime)
{

}

void Score::ScoreRender()
{
	
	resultBack.spr->Begin();
	resultBack.spr->DrawRota(1920.0f / 2.0f, 1080.0f / 2.0f, 0.0f, 1.0f, resultBack.inf[0], {1.0f, 1.0f, 1.0f, abs(dissolveScaler - 2.0f) / 2.0f });
	resultBack.spr->End();

	resultBase.spr->Begin();
	for (int h = 0; h < 5; h++)
	{
		for (int w = 0; w < 8; w++)
		{
			if(h % 2 == 0)  resultBase.spr->DrawRota(resultPos[w][h].x, resultPos[w][h].y, 0.0f, 1.0f, resultBase.inf[w + 8 * h]);
			else  resultBase.spr->DrawRota(resultPos[w][h].x, resultPos[w][h].y, 0.0f, 1.0f, resultBase.inf[w + 8 * h]);
		}
	}
	resultBase.spr->End();

	scoreNumText.spr->Begin();
	{
		float textPosX = 1150.0f;
		int j = scoreNumTextDigit - 1;
		for (int i = 0; i < scoreNumTextDigit; i++)
		{

			scoreNumText.spr->DrawRota(textPosX, 580.0f, 0.0f, 0.5f, scoreNumText.inf[scoreNumTextFrame[i]], { 1.0f, 1.0f, 1.0f, abs(dissolveScaler - 2.0f) / 2.0f });
			textPosX += 64.0f;
			j--;
		}
	}
	scoreNumText.spr->End();

	pressA.spr->Begin();
	pressA.spr->DrawRota(1920.0f - 128.0f, 1080.0f / 2.0f + 128.0f, 0.0f, 0.6f, pressA.inf[0], {1.0f, 1.0f, 1.0f, pressAAlpha * abs(dissolveScaler - 2.0f) / 2.0f });
	pressA.spr->End();

}

void Score::Render()
{
	scoreText.spr->Begin();
	scoreText.spr->DrawRota2(160.0f, 376.0f, 0.0f, 0.15f, scoreText.inf[0]);
	scoreText.spr->End();

	scoreNumText.spr->Begin();
	{
		float textPosX = 150.0f;
		int j = scoreNumTextDigit - 1;
		for (int i = 0; i < scoreNumTextDigit; i++)
		{

			scoreNumText.spr->DrawRota(textPosX, 520.0f, 0.0f, 0.25f, scoreNumText.inf[scoreNumTextFrame[i]]);
			textPosX += 32.0f;
			j--;
		}
	}
	scoreNumText.spr->End();

}
void Score::Finalize()
{
	TextureRelease(&scoreText);
	TextureRelease(&scoreNumText);
	TextureRelease(&resultBack);
	TextureRelease(&pressA);
	TextureRelease(&resultBase);
}

void Score::AddScore(int combo)
{
	tmpScore += combo;
}
#include "GameTimer.h"
#include <imgui.h>

void GameTimer::Initialize()
{
	LoadDivGraph(L"Data/Texture/TimerText.png", 1, 1, 1, 1280, 256, &timeText);
	LoadDivGraph(L"Data/Texture/new_number.png", 10, 10, 1,128, 256, &timeNumText);

	floatTimer = 90.0f;
	intTimer = 90;
	timeNumTextDigit = 2;
}

void GameTimer::Reset()
{
	floatTimer = 90.0f;
	intTimer = 90;
	timeNumTextDigit = AnimationFrameCalc(intTimer, timeNumTextFrame);
	timeNumTextDigit = 2;
}

void GameTimer::Update(float elapsedTime)
{
	timeNumTextDigit = AnimationFrameCalc(intTimer, timeNumTextFrame);

	floatTimer -= elapsedTime;
	intTimer = (int)floatTimer;

}
void GameTimer::Render()
{

	timeText.spr->Begin();
	timeText.spr->DrawRota2(960.0f, 48.0f, 0.0f, 0.2f, timeText.inf[0]);
	timeText.spr->End();

	timeNumText.spr->Begin();
	{
		if (timeNumTextDigit == 2)
		{
			timeNumText.spr->DrawRota2(960.0f - 17.0f, 96.0f, 0.0f, 0.3f, timeNumText.inf[timeNumTextFrame[0]]);
			timeNumText.spr->DrawRota2(960.0f + 23.0f, 96.0f, 0.0f, 0.3f, timeNumText.inf[timeNumTextFrame[1]]);
		}
		else
		{
			if (timeNumTextFrame[0] < 0) timeNumTextFrame[0] = 0;
			timeNumText.spr->DrawRota2(960.0f, 96.0f, 0.0f, 0.3f, timeNumText.inf[timeNumTextFrame[0]]);
		}
	}
	timeNumText.spr->End();

}
void GameTimer::Finalize()
{
	TextureRelease(&timeNumText);
	TextureRelease(&timeText);
}

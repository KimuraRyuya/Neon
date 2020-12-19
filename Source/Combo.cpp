#include "Combo.h"
#include "../Library/MyLib.h"
#include <imgui.h>

void Combo::Initialize()
{
	nowCombo = 0;
	comboTime = 0.0f;
	comboMaxTime =5.0f;

	comboText = std::make_unique<Billboard>(L"Data/Texture/comboText.png");
	comboTextPos = { 0.0f, 10.0f, 0.0f };

	comboNumText = std::make_unique<Billboard>(L"Data/Texture/Numbers.png");
	comboNumTextPos = { 0.0f, 10.0f, 0.0f };

	comboNumTextDigit = 0;
	level = 1;
}

void Combo::Reset()
{
	nowCombo = 0;
	comboTime = 0.0f;
	comboMaxTime = 5.0f;

	comboTextPos = { 0.0f, 10.0f, 0.0f };

	comboNumTextPos = { 0.0f, 10.0f, 0.0f };

	comboNumTextDigit = 0;
	level = 1;

}

void Combo::Update(float elapsedTime)
{
	// 現在コンボが０以上なら
	if (nowCombo > 0)
	{
		comboTime += elapsedTime;
		// 一定のタイム過ぎたらコンボなどをリセット
		if (comboTime > comboMaxTime)
		{
			nowCombo = 0;
			comboTime = 0;
		}
	}

	// コンボの値をフレームごとに分割
	comboNumTextDigit = AnimationFrameCalc(nowCombo, comboNumTextFrame);

	// レベルの振り分け
	level = 1;
	if (nowCombo > 50)
	{
		level = 3;
	}
	else if (nowCombo > 20)
	{
		level = 2;
	}

#ifdef _DEBUG
	ImGui::Begin("Combo");

	ImGui::Text("Combo##Num : %d", nowCombo);
	ImGui::Text("Combo##Time : %f", comboTime);

	ImGui::End();
#endif // _DEBUG

}

void Combo::Render()
{
	comboText->Begin();
	comboText->Render({ comboTextPos.x - 2.5f, comboTextPos.y, comboTextPos.z }, 0.0f, 0.0f, 1024.0f, 1024.0f, { 0.0f, 0.0f, 0.0f }, { 13.0f, 13.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f });
	comboText->End();

	comboNumText->Begin();
	for (int i = 0; i < comboNumTextDigit; i++)
	{
		comboNumText->Render({ comboNumTextPos .x - 6.3f - (i * 0.75f), comboNumTextPos .y, comboNumTextPos .z}, 32.0f * comboNumTextFrame[i], 0.0f, 32.0f, 64.0f, { 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f });
	}
	comboNumText->End();
}

void Combo::Finalize()
{

}

void Combo::AddCombo()
{
	nowCombo++;
	comboTime = 0.0f;
}
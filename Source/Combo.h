#pragma once
#include "../Library/Billboard.h"

class Combo
{
private:
	int nowCombo;
	float comboTime;
	float comboMaxTime;

	std::unique_ptr<Billboard> comboText;
	DirectX::XMFLOAT3 comboTextPos;

	std::unique_ptr<Billboard> comboNumText;
	DirectX::XMFLOAT3 comboNumTextPos;
	int comboNumTextDigit;
	int comboNumTextFrame[10];
	int level;
public:
	Combo() {}
	~Combo() {}

	void Initialize();
	void Update(float elapsedTime);
	void Render();
	void Finalize();

	void Reset();
	
	// プレイヤーの位置からコンボのテキストの位置を調整
	void SetComboTextPos(DirectX::XMFLOAT3 pos) { comboTextPos = pos; comboTextPos.z *= -1; }
	void SetComboNumTextPos(DirectX::XMFLOAT3 pos) { comboNumTextPos = pos; comboNumTextPos.z *= -1; }

	void SetLevel(int l) { level = l; }
	int GetLevel() { return level; }
	int GetNowCombo() { return nowCombo; }
	
	//コンボを+していく
	void AddCombo();

	void ComboReset() 
	{
		nowCombo = 0;
		comboTime = 0.0f;
	}
};

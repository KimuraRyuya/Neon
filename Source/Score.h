#pragma once
#include "../Library/Billboard.h"
#include "../Library/MyLib.h"

class Score
{
private:
	float dissolveScaler;
	float addDissolveScaler;

	float nowScore;
	float tmpScore;
	float addScore;
	float addScoreTimer;
	float addScoreTotalTimer;

	DirectX::XMFLOAT2 resultPos[8][5];
	DirectX::XMFLOAT2 resultBasePos[8][5];
	DirectX::XMFLOAT2 resultStartPos[8][5];
	float resultTime[8][5];
	float resultTotalTime[8][5];

	int scoreNumTextFrame[20];
	int scoreNumTextDigit;

	float pressAAlpha;
	bool isPressABlinking;

	LoadInf scoreText;
	LoadInf pressA;
	LoadInf resultBase;
	LoadInf scoreNumText;
	LoadInf resultBack;
public:
	Score() : nowScore(0.0f), scoreNumTextDigit(1), tmpScore(0), addScore(0.0f), scoreNumTextFrame()
	{	}
	~Score() {}
	void Initialize();
	void Update(bool finishFlg, float elapsedTime);
	void DissolveUpdate(float elapsedTime);
	void Render();
	void ScoreRender();
	void Finalize();
	void Reset();

	void SetAddDissolveScaler(float s) { addDissolveScaler = s; }
	float GetDissolveScaler() { return dissolveScaler; }
	int GetNowScore() { return static_cast<int>(nowScore); }
	void AddScore(int combo);

};
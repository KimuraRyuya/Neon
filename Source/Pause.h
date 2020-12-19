#pragma once
#include "../Library/MyLib.h"
#undef max
#undef min

class Pause
{
private:
	LoadInf pauseText;
	LoadInf pauseFrame;
	LoadInf pauseBack;
	LoadInf pauseCursol;

	int nowMode;
	int nextMode;

	DirectX::XMFLOAT2 cursolPos;
	DirectX::XMFLOAT2 targetCursolPos[3];
	bool cursolMoveTrg;
	float cursolMoveTime;

	bool hasRestartTransition;
	bool hasMenuTransition;
	bool hasTitleTransition;

	bool hasCursolMove;
	bool pauseTrg;
	bool nowPaused;
	bool isSelect;
	float alpha;
	float dissolve;
public:
	Pause() {  }
	~Pause() {  }
	void Initialize();
	void Update(float elapsedTime);
	void Render();
	void Finalize();
	void Reset();

	bool GetNowPaused() { return nowPaused; }
	void SetNowPaused(bool f) { nowPaused = f; }
	bool GetPauseTrg() { return pauseTrg; }
	void SetPauseTrg(bool f) { pauseTrg = f; }
	bool GetMenuTransition() { return hasMenuTransition; }
	void SetMenuTransition(bool f) { hasMenuTransition = f; }
	bool GetTitleTiransition() { return hasTitleTransition; }
	void SetTitleTransition(bool f) { hasTitleTransition = f; }
	bool GetRestartTiransition() { return hasRestartTransition; }
	void SetRestartTransition(bool f) { hasRestartTransition = f; }

};
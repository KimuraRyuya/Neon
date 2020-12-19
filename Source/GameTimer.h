#pragma once
#include "../Library/MyLib.h"

class GameTimer
{
private:
	float floatTimer;
	int intTimer;

	int timeNumTextDigit;
	int timeNumTextFrame[2];

	LoadInf timeText;
	LoadInf timeNumText;

public:
	void Initialize();
	void Update(float elapsedTime);
	void Render();
	void Finalize();
	void Reset();

	int GetGameTimer	() { return intTimer; }

};
#pragma once

#include "../Library/Billboard.h"
#include "../Library/Bloom.h"
#include "../Library/FrameBuffer.h"
#include "../Library/ToneMap.h"
#include "../Library/MyLib.h"
#include "Scene.h"
#include "BG.h"

class Menu
{
private:

	struct ScoreInfo
	{
		int bestScore[5];
		int scoreDigit[5];
		int scoreframe[5][20];
	};

	const DirectX::XMFLOAT3 titleCameraPos = { 1.0f, 40.0f, 7.0f };
	const DirectX::XMFLOAT3 titleCameraTarget = { 1.0f, 40.0f, -60.0 };
	const DirectX::XMFLOAT3 gameCameraPos = { 0.0f, 100.0f, 0.0f };
	const DirectX::XMFLOAT3 gameCameraTarget = { 0.0f, 0.0f, 0.0f };

	std::unique_ptr<Descartes::FrameBuffer> framebuffers[2];
	std::unique_ptr<Bloom> bloomEffect;
	std::unique_ptr<ToneMap> tonemap_effect;

	LoadInf menuText;
	LoadInf menuFrame;
	LoadInf menuBack;
	LoadInf menuCursol;
	LoadInf scoreNumText;
	LoadInf rankingFrame;

	DirectX::XMFLOAT2 cursolPos;
	DirectX::XMFLOAT2 targetCursolPos[4];

	ScoreInfo normalModeScore;
	ScoreInfo endlessModeScore;

	float cursolMoveTime;
	int tmpMode;
	float dissolve;
	float scoreTextAlpha;
	float addDissolve;
	int nowMode;
	float cameraMoveTime;

	bool hasCursolMove;
	bool inputTrg;
	bool hasOpenDissolveSound;
	bool canNextSceneState;

public:
	Menu() {}
	~Menu() {}

	void SetOpenDissolveSound(bool f) { hasOpenDissolveSound = f; }
	int GetNowMode() { return nowMode; }
	float GetDissolve() { return dissolve; }
	void SetAddDissolve(float d) { addDissolve = d; }

	// ƒXƒRƒA‚ð‚à‚ç‚Á‚Ä‚­‚é
	void ReceiveScoreValue(int* normalHiScore, int* endlessHiScore);
	void Reset();

	void Initialize();
	SceneState Update(float elapsedTime);
	void Render(std::shared_ptr<BG> bg);
	void Finalize();

};	
#pragma once
#include "CharacterSystem.h"
#include "BG.h"
#include "PlayerShot.h"
#include "CountDown.h"
#include "Combo.h"
#include "Score.h"
#include "PlayerHpUi.h"
#include "TrackingScore.h"
#include "GameTimer.h"
#include "MetaAI.h"
#include "../Library/Bloom.h"
#include "../Library/FrameBuffer.h"
#include "../Library/ToneMap.h"
#include "Scene.h"

namespace Edit
{
	class EditPrameter;
}

class Game : Descartes::FullscreenQuad
{
private:
	std::unique_ptr<CharacterSystem> characterSystem;
	std::shared_ptr<BG> bg;
	std::unique_ptr<CountDown> countDown;
	std::unique_ptr<Combo> combo;
	std::unique_ptr<Score> score;
	std::unique_ptr<GameTimer> gameTimer;
	std::unique_ptr<PlayerHpUi> playerHpUi;
	std::unique_ptr<TrackingScore> trackingScore;
	std::unique_ptr<MetaAI> metaAI;

	std::unique_ptr<Descartes::FrameBuffer> framebuffers[2];
	std::unique_ptr<Bloom> bloomEffect;
	std::unique_ptr<ToneMap> tonemap_effect;

	bool finishFlg;
	bool nextSceneStateFlg;
	std::shared_ptr<Edit::EditPrameter> editParameter;
public:
	Score* getScoreAddress() { return score.get(); }
	std::shared_ptr<BG> getBgAddress() { return bg; }
	bool getNextSceneFlg() { return nextSceneStateFlg; }

public:
	Game() : Descartes::FullscreenQuad(pFrameWork.GetDevice()) {}
	~Game() {}

	void Initialize();
	SceneState Update(float elapsedTime);
	SceneState EndlessUpdate(float elapsedTime);
	SceneState CountDownUpdate(SceneState nextScene, float elapsedTime);
	SceneState ResultUpdate(float elapsedTime);
	void Render();
	void EndlessRender();
	void ResultRender();
	void Finalize();
	void Reset();
	void SetEditParameter(std::shared_ptr<Edit::EditPrameter> ptr);
};
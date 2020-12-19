#pragma once
#include "../Library/Billboard.h"
#include "../Library/Bloom.h"
#include "../Library/FrameBuffer.h"
#include "../Library/ToneMap.h"
#include "Scene.h"
#include "BG.h"
#include "CharacterSystem.h"
#include "PlayerShot.h"
#include "Combo.h"
#include "PlayerHpUi.h"
#include "TrackingScore.h"

namespace Edit
{
	class EditPrameter;
}

class Tutorial
{
private:
	enum State
	{
		Move,
		Dash,
		Shot,
		Fill,
		Power
	};

	std::unique_ptr<CharacterSystem> characterSystem;
	std::unique_ptr<Combo> combo;
	std::unique_ptr<PlayerHpUi> playerHpUi;
	std::unique_ptr<TrackingScore> trackingScore;

	std::unique_ptr<Descartes::FrameBuffer> framebuffers[4];
	std::unique_ptr<Bloom> bloomEffect;
	std::unique_ptr<ToneMap> tonemap_effect;

	std::unique_ptr<Billboard> textTex;
	std::unique_ptr<Billboard> buttonTex;

	float displyNum;
	float buttonAnimFrame;
	
	bool buttonTrg;
	
	State state;
	
	DirectX::XMFLOAT3 textPos;

	bool isCameraMove;
	DirectX::XMFLOAT3 cameraTarget;
	float cameraMoveTime;

	std::shared_ptr<Edit::EditPrameter> editParameter;
public:


	void Initialize();
	SceneState Update(std::shared_ptr<BG> bg, float elapsedTime);
	void Render(std::shared_ptr<BG> bg);
	void Finalize();
	void Reset();
	
	// チュートリアルのスキップ
	void SkipText();

	void SetEditParameter(std::shared_ptr<Edit::EditPrameter> ptr);
};
#pragma once
#include "Scene.h"
#include "Title.h"
#include "Menu.h"
#include "Tutorial.h"
#include "Ranking.h"
#include "Game.h"
#include "Pause.h"
#include "../Library/Bloom.h"
#include "../Library/FrameBuffer.h"
#include "../Library/ToneMap.h"
//#include "EditParameter.h"

namespace Edit
{
	class EditPrameter;
}
class SceneGame : public Scene
{
private:

	const DirectX::XMFLOAT3 titleCameraPos = { 1.0f, 40.0f, 7.0f };
	const DirectX::XMFLOAT3 titleCameraTarget = { 1.0f, 40.0f, -60.0 };
	const DirectX::XMFLOAT3 gameCameraPos = { 0.0f, 100.0f, 0.0f };
	const DirectX::XMFLOAT3 gameCameraTarget = { 0.0f, 0.0f, 0.0f };

	std::unique_ptr<Title> title;
	std::unique_ptr<Game> game;
	std::unique_ptr<Menu> menu;
	std::unique_ptr<Tutorial> tutorial;
	std::unique_ptr<Ranking> ranking;
	std::unique_ptr<Pause> pause;

	float cameraMoveTime;
	bool isFinish;

	bool canNextSceneState;

	SceneState sceneState;
	SceneState gameMode;

	// 外部ImGuiとの連携ポインタ
	std::shared_ptr<Edit::EditPrameter> editParameter;

public:
	SceneGame()
	{
		isFinish = false;
	}

	~SceneGame()
	{
		Finalize();
	}

	void Initialize();
	void Update(float elapsedTime);
	void Render();
	void Finalize();
	void Reset();
	void SomeReset();


	static SceneGame* GetInstance()
	{
		static SceneGame instance;
		return &instance;
	}
};
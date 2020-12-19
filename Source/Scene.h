#pragma once
#include <DirectXMath.h>

enum class SceneState
{
	TITLE,
	MENU,
	TUTORIAL,
	COUNT,
	GAME,
	ENDLESS,
	TIMEOVER,
	DEAD,
	RESULT
};

// SceneƒNƒ‰ƒX
class Scene
{
private:

public:
	Scene() {}
	~Scene(){}

	virtual void Initialize() {}
	virtual void Update(float elapsedTime) {}
	virtual void Render() {}
	virtual void Finalize() {}
};
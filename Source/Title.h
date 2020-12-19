#pragma once

#include "../Library/Model.h"
#include "../Library/ModelRenderer.h"
#include "../Library/ModelResource.h"

#include "../Library/Billboard.h"
#include "../Library/Bloom.h"
#include "../Library/FrameBuffer.h"
#include "../Library/ToneMap.h"
#include "Scene.h"
#include "OBJ3D.h"
#include "BG.h"

class Title
{
private:
	std::unique_ptr<Descartes::FrameBuffer> framebuffers[2];
	std::unique_ptr<Bloom> bloomEffect;
	std::unique_ptr<ToneMap> tonemap_effect;

	DirectX::XMFLOAT3 titleTexPos;
	std::unique_ptr<Billboard> titleTexure;
	std::unique_ptr<Billboard> pressATexure;
	float dissolve;
	float addDissolve;

	bool hasOpenDissolveSound;
	bool canNextSceneState;

public:
	Title() {}
	~Title() {}

	void SetOpenDissolveSound(bool f) { hasOpenDissolveSound = f; }
	float GetDissolve() { return dissolve; }
	void SetAddDissolve(float d) { addDissolve = d; }

	void Reset();

	void Initialize();
	SceneState Update(float elapsedTime);
	void Render(std::shared_ptr<BG> bg);
	void Finalize();

};
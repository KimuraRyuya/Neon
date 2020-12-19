#pragma once

#include "Scene.h"
#include <memory>
#include <iostream>
#include "../Library/FrameBuffer.h"
#include "../Library/RenderState.h"
#include "../Library/Shader.h"
#include "../Library/Rasterizer.h"

class SceneManager
{
public:
	SceneManager() {}
	~SceneManager() { Finalize(); }

	enum { ALPHA, ALPHA2, ADD, MULTIPLY, ALPHA_TO_COVERAGE, NONE };
	std::unique_ptr<Descartes::BlendState> blend_states[6];
	enum { SOLID, SOLID_CULL_NONE, WIREFRAME, SOLID_CULL_FRONT };
	std::unique_ptr<Descartes::RasterizerState> rasterizer_states[4];
	enum { POINT, LINEAR, ANISOTROPIC };
	std::unique_ptr<Descartes::SamplerState> sampler_states[3];
	enum { ZT_ON_ZW_ON, ZT_ON_ZW_OFF, ZT_OFF_ZW_ON, ZT_OFF_ZW_OFF };
	std::unique_ptr<Descartes::DepthStencilState> depth_stencil_states[4];

private:
	Scene* pScene;
	Scene* pStack;
	Scene* pNext;

	DirectX::XMFLOAT4X4 view;
	DirectX::XMFLOAT4X4 projection;
	DirectX::XMFLOAT4 lightDirection;

public:
	void Initialize();
	void Update(float elapsedTime);
	void Render();
	void Finalize();

	// シーン遷移
	void SetScene(Scene* scene);
	// 現在のシーンをスタック(Renderのみ通る)状態にする
	void SetStack(Scene* scene);
	// スタックシーンを破棄
	void DestroyStack();


	static SceneManager* GetInstance()
	{
		static SceneManager instance;
		return &instance;
	}
};

#define pSceneManager	 SceneManager::GetInstance()
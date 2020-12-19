#include "../Library/FrameWork.h"
#include "../Library/Camera.h"
#include "../Library/MyLib.h"
#include "SceneManager.h"
#include "SceneGame.h"
#include <iostream>
#include <windows.h>
#undef max
#undef min

void SceneManager::Initialize()
{
	srand((unsigned int)time(NULL));

	// 初期化処理
	SetScene(SceneGame::GetInstance());
	pStack = NULL;

	// ブレンドの設定
	blend_states[ALPHA] = std::make_unique<Descartes::BlendState>(pFrameWork.GetDevice(), BLEND_MODE::ALPHA);
	blend_states[ALPHA2] = std::make_unique<Descartes::BlendState>(pFrameWork.GetDevice(), BLEND_MODE::ALPHA2);
	blend_states[ADD] = std::make_unique<Descartes::BlendState>(pFrameWork.GetDevice(), BLEND_MODE::ADD);
	blend_states[MULTIPLY] = std::make_unique<Descartes::BlendState>(pFrameWork.GetDevice(), BLEND_MODE::MULTIPLY);
	blend_states[ALPHA_TO_COVERAGE] = std::make_unique<Descartes::BlendState>(pFrameWork.GetDevice(), BLEND_MODE::ALPHA_TO_COVERAGE);
	blend_states[NONE] = std::make_unique<Descartes::BlendState>(pFrameWork.GetDevice(), BLEND_MODE::NONE);

	// カリングの設定
	rasterizer_states[SOLID] = std::make_unique<Descartes::RasterizerState>(pFrameWork.GetDevice(), D3D11_FILL_SOLID, D3D11_CULL_BACK);
	rasterizer_states[SOLID_CULL_NONE] = std::make_unique<Descartes::RasterizerState>(pFrameWork.GetDevice(), D3D11_FILL_SOLID, D3D11_CULL_NONE);
	rasterizer_states[WIREFRAME] = std::make_unique<Descartes::RasterizerState>(pFrameWork.GetDevice(), D3D11_FILL_WIREFRAME);
	rasterizer_states[SOLID_CULL_FRONT] = std::make_unique<Descartes::RasterizerState>(pFrameWork.GetDevice(), D3D11_FILL_SOLID, D3D11_CULL_FRONT);

	// サンプラーの設定
	sampler_states[POINT] = std::make_unique<Descartes::SamplerState>(pFrameWork.GetDevice(), D3D11_FILTER_MIN_MAG_MIP_POINT);
	sampler_states[LINEAR] = std::make_unique<Descartes::SamplerState>(pFrameWork.GetDevice(), D3D11_FILTER_MIN_MAG_MIP_LINEAR);
	sampler_states[ANISOTROPIC] = std::make_unique<Descartes::SamplerState>(pFrameWork.GetDevice(), D3D11_FILTER_ANISOTROPIC);

	// デプスの設定
	depth_stencil_states[ZT_ON_ZW_ON] = std::make_unique<Descartes::DepthStencilState>(pFrameWork.GetDevice(), TRUE, D3D11_DEPTH_WRITE_MASK_ALL, D3D11_COMPARISON_LESS);
	depth_stencil_states[ZT_ON_ZW_OFF] = std::make_unique<Descartes::DepthStencilState>(pFrameWork.GetDevice(), TRUE, D3D11_DEPTH_WRITE_MASK_ZERO, D3D11_COMPARISON_LESS);
	depth_stencil_states[ZT_OFF_ZW_ON] = std::make_unique<Descartes::DepthStencilState>(pFrameWork.GetDevice(), FALSE, D3D11_DEPTH_WRITE_MASK_ALL, D3D11_COMPARISON_LESS);
	depth_stencil_states[ZT_OFF_ZW_OFF] = std::make_unique<Descartes::DepthStencilState>(pFrameWork.GetDevice(), FALSE, D3D11_DEPTH_WRITE_MASK_ZERO, D3D11_COMPARISON_LESS);

}

void SceneManager::Update(float elapsedTime)
{

	if (!pStack)
	{
		// 更新処理
		if (pNext)
		{
			pScene = pNext;
			pNext = nullptr;
			pScene->Initialize();
		}
		pScene->Update(elapsedTime);
	}
	else
	{
		pStack->Update(elapsedTime);
	}

	Camera::GetInstance().Update();
}

void SceneManager::Render()
{
	// 描画処理
	if (!pNext) pScene->Render();
	if (pStack) pStack->Render();

}

// シーンの切り換え
void SceneManager::SetScene(Scene* scene)
{
	pNext = scene;
}
// スタックシーンの切り換え
void SceneManager::SetStack(Scene* scene)
{
	pStack = scene;
	pStack->Initialize();
}
//スタックリリース
void SceneManager::DestroyStack()
{
	if (!pStack) return;
	pStack->Finalize();
	pStack = NULL;
}



void SceneManager::Finalize()
{
	//pScene = nullptr;
	//pNext = nullptr;
	//pScene->Finalize();
	//if (pScene)
	//{
	//	pScene->Finalize();
	//	delete pScene;
	//	pScene = nullptr;
	//}
	//if (pNext)
	//{
	//	pNext->Finalize();
	//	delete pNext;
	//	pNext = nullptr;
	//}

}
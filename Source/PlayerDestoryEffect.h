#pragma once

#include "../Library/Model.h"
#include "../Library/ModelRenderer.h"
#include "../Library/ModelResource.h"

#include "Collision.h"
#include "OBJ3D.h"

#include <memory>
#include <iostream>

class PlayerDestoryEffect
{

private:
	std::unique_ptr<ModelRenderer> modelRenderer;
	std::shared_ptr<ModelResource> modelResource;
	DirectX::XMFLOAT4 luminans;
	float scaleTime;
	bool hasFinishSpawn;
public:
	PlayerDestoryEffect() {}
	~PlayerDestoryEffect() {}

	OBJ3D effectObj;

	void Initialize();
	void Update(DirectX::XMFLOAT3 playerPos, bool playerNowSpawn, float elapsedTime);
	void Render();
	void Finalize();
	void Reset();

};
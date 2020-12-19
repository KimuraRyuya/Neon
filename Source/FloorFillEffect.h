#pragma once

#include "../Library/model.h"
#include "../Library/ModelRenderer.h"
#include "../Library/ModelResource.h"
#include "../Library/Billboard.h"
#include "Constant.h"
#include "Collision.h"
#include "OBJ3D.h"

#include <memory>
#include <iostream>

class FloorFillEffect
{
private:
	// エフェクトの基本パラメーター
	struct EffectInf
	{
		float scaleTime;
		float maxScale;
		bool isExist;

		void Clear()
		{
			scaleTime = 0.0f;
			maxScale = 0.0f;
			isExist = false;
		}
	};

	std::unique_ptr<Billboard> effectTex;
	std::unique_ptr<ModelRenderer> modelRenderer;
	std::shared_ptr<ModelResource> modelResource;
	DirectX::XMFLOAT4 luminans;
	EffectInf effetcInf[Constant::MAX_FILLEFFECT];
public:
	FloorFillEffect() {}
	~FloorFillEffect() {}

	OBJ3D effectObj[Constant::MAX_FILLEFFECT];


	void Initialize();
	void Update(DirectX::XMFLOAT3 p, float elapsedTime);
	void Render();
	void Finalize();
	void Reset();

	bool GetEffectExist(int i) { return effetcInf[i].isExist; }

	/// <summary>
	/// エフェクト生成
	/// </summary>
	/// <param name="p"> 生成座標 </param>
	/// <param name="s"> 最大スケール </param>
	void EmitEffect(DirectX::XMFLOAT3 p, float s);

};
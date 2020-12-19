#pragma once
#include "../Library/Model.h"
#include "../Library/ModelRenderer.h"
#include "../Library/ModelResource.h"

#include "Collision.h"
#include "OBJ3D.h"
#include "../Library/GPUParticle/EmitParticle.h"

#include <memory>
#include <iostream>
#include "Constant.h"

class PlayerShotSystem
{
private:
	std::unique_ptr<ModelRenderer> modelRenderer;
	std::shared_ptr<ModelResource> modelResource;

	OBJ3D shotObj[Constant::SHOT_MAX];
	bool isExist[Constant::SHOT_MAX];

	int meshEmitParticleUpdateTime;

public:
	PlayerShotSystem() {}
	~PlayerShotSystem() { Finalize(); }


	void Initialize();
	void Update(float elapsedTime);
	void Render();
	void Finalize();

	void Reset();


	/// <summary>
	/// �v���C���[�̃V���b�g�̐����֐�
	/// </summary>
	/// <param name="pos"> �ǂ̈ʒu����o���� </param>
	/// <param name="vel"> �V���b�g�̑��� </param>
	/// <param name="isReinforce"> �o�t���������Ă��邩 </param>
	void ShotBurst(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 vel, bool isReinforce);

	bool GetExist(int i) { return isExist[i]; }

	DirectX::XMFLOAT3 GetPos(int i) { return shotObj[i].GetPos(); }
	float GetScale(int i) { return shotObj[i].GetScale().x; }
	DirectX::XMFLOAT3 GetVelocity(int i) { return shotObj[i].GetVelocity(); }
	void SetExist(bool _exist, int i) { isExist[i] = _exist; };

	static PlayerShotSystem* GetInstance()
	{
		static PlayerShotSystem instance;
		return &instance;
	}

};
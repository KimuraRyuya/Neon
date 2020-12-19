#pragma once
#include "../Library/Model.h"
#include "../Library/ModelRenderer.h"
#include "../Library/ModelResource.h"
#include "../Library/Billboard.h"

#include "Collision.h"
#include "OBJ3D.h"
#include "../Library/GPUParticle/EmitParticle.h"
#include "PlayerDestoryEffect.h"
#include "FloorFillEffect.h"
#include "Ultimate.h"
#include <memory>
#include <iostream>
#include <string>

namespace Edit
{
	namespace Player
	{
		class EditPlayerParameter;
	}
}

class Player
{
private:
	std::unique_ptr<ModelRenderer> modelRenderer;
	std::shared_ptr<ModelResource> modelResource;
	std::unique_ptr<CollisionPrimitive> playerCollision;

	DirectX::XMFLOAT4 luminans;
	OBJ3D playerObj;
	ParticleInfo particleInfo;
	std::unique_ptr<PlayerDestoryEffect> playerDestoryEffect;
	std::unique_ptr<FloorFillEffect> floorFillEffect;
	std::unique_ptr<Ultimate> ultimate;

	DirectX::XMFLOAT4 quaternion;
	DirectX::XMFLOAT4X4 rotateMat;

	int hp;
	float burstElapsedTime;
	
	float accel;
	float vibValue;
	float newSpawnTime;
	float dushGauge;
	float emitPowerUpEffectElapsedTime;

	float luminansAnimationElapsedTime;
	float luminansAnimationAddTime;
	float luminansAnimationTime;

	bool hasNewSpawn;
	bool emitSpawnEffect;
	bool isReinforce;
	bool hasFillChipDestory;
	bool isVib;
	bool isShotSound;
	bool isBuffShotSound;
	bool hasEnemyAllKill;

	// ImGuiパラメーター
	std::shared_ptr<Edit::Player::EditPlayerParameter> editPlayerParam;
public:
	Player() {}
	~Player() { }
	void Initialize();
	void Update(int lev, float elapsedTime);
	void Render();
	void Finalize();
	void Reset();

	// HPを減らす
	void SubHp();
	// ダッシュGaugeを増やす
	void AddDushGauge(float v) { 
		dushGauge += v; 
		if (dushGauge > 360) dushGauge = 360.0f;
		else if (dushGauge < 0) dushGauge = 0.0f;
	}
	float GetDushGauge() { return dushGauge; }
	int GetHp() { return hp; }
	bool GetNewSpawn() {return hasNewSpawn;}
	bool GetEnemyAllKillFlg() {return hasEnemyAllKill;}
	void SetEnemyAllKillFlg(bool f) { hasEnemyAllKill = f;}
	bool GetReinforce() {return isReinforce;}
	void SetReinforce(bool f) {isReinforce = f;}
	void SetFillChipDestory(bool f) { hasFillChipDestory = f;}
	bool GetFillChipDestory() {return hasFillChipDestory;}
	float GetPlayerDestoryEffectScale() { return playerDestoryEffect->effectObj.GetScale().x; }
	PlayerDestoryEffect* GetPlayerDestoryEffect() { return playerDestoryEffect.get(); }
	Ultimate* GetPlayerUltimate() { return ultimate.get(); }
	DirectX::XMFLOAT3 GetPlayerDestoryEffectPos() { return playerDestoryEffect->effectObj.GetPos(); }
	void SetPos(DirectX::XMFLOAT3 _p) { playerObj.SetPos(_p); }
	DirectX::XMFLOAT3 GetPos() { return playerObj.GetPos(); }
	std::shared_ptr<ModelResource> GetModelResource() { return modelResource; }
	CollisionPrimitive* GetPlayerCollision() { return playerCollision.get(); }

	void SetEditPlayerParam(std::shared_ptr<Edit::Player::EditPlayerParameter> ptr);

};
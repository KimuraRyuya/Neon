#pragma once
#include "Enemy.h"
#include "../Library/GPUParticle/GPUParticleBuffers.h"
#include "../Library/modelRenderer.h"
#include "../Library/modelResource.h"
#include "Constant.h"
#include "Combo.h"

class EnemyManager
{
public:

private:
	std::unique_ptr<ModelRenderer> modelRenderer;
	std::shared_ptr<ModelResource> modelResource;

	std::shared_ptr<Model> model[3];
	std::shared_ptr<Model> spawnModel[3];

	EnemyBufferInfo enemyBufferInfo[Constant::MAX_ENEMY];
	Enemy enemy[Constant::MAX_ENEMY];
	std::unique_ptr<GPUParticleBuffers::GPUBuffer> enemyBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> readBuffer;

	DirectX::XMFLOAT3 targetPos;

	float popElapsedTime;
	float popTime;
	bool isWarning;
	// ChaseとRefrect用の生成数
	int enemyPopNum;
	// エネミーの行動パターン
	Enemy::ActionType actionType;
	int enemyExistNum;
	bool canNextEnemyPop;
	DirectX::XMFLOAT4* bgLuminans;
public:

private:
public:
	EnemyManager() {}
	~EnemyManager() { Finalize(); }

	void Initialize();
	void Update(int lev, float elapsedTime);
	void Render();
	void Finalize();
	void Reset();

	/// <summary>
	/// エネミーの行動パターンセレクト
	/// </summary>
	/// <param name="type"> 行動タイプ </param>
	/// <param name="lev"> プレイヤーのレベル </param>
	void ActionSelect(int type, int lev);

	// MetaAIからパラメーターを取得
	void SetNextEnemyParamerter(Enemy::ActionType type, int pN/*生成数*/, int pT/*生成時間*/);

	// エネミーのターゲット
	void SetTargetPos(DirectX::XMFLOAT3 t) { targetPos = t; }

	EnemyBufferInfo GetEnemyInfo(int i) { return enemyBufferInfo[i]; }
	bool GetEnemyExist(int i) { return enemy[i].isExist; }
	bool GetEnemyNowSpawn(int i) { return enemy[i].hasNowSpawn; }
	int GetEnemyExistNum() { return 	enemyExistNum; }
	void GetHp(int i, float hp) { enemyBufferInfo[i].hp = hp; }

	// エネミー存在してるのかジャッジ
	void EnemyExistJudge(float elapsedTime);
	bool GetIsWarning() { return isWarning; }
	void ResetWarning() { isWarning = false; }
	// エネミーをすべてkillする
	void AllDown();
	void SetBgLuminans(DirectX::XMFLOAT4* lum);
};

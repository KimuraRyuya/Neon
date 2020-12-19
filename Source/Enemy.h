#pragma once
#include <DirectXMath.h>
#include "Collision.h"
#include "../Library/Model.h"
#include "../Library/ModelResource.h"
#include "../Library/GPUParticle/EmitParticle.h"

// エネミーのバッファ用変数兼パラメーター
struct EnemyBufferInfo
{
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT3 scale;
	DirectX::XMFLOAT3 rotate;
	DirectX::XMFLOAT3 size;
	float hp;
	// 40 byte
};

class Enemy
{
private:

public:
	// エネミーの色の種類
	enum ColorType
	{
		Red,
		Yellow,
		Purple
	};

	// エネミーの行動パターン
	enum ActionType
	{
		Top_to_Bottom_Liner,
		Bottom_to_Top_Liner,
		Right_to_LeftLiner,
		Left_to_Right_Liner,
		Chase,
		Reflection,
		WARNING
	};

	std::shared_ptr<Model> model;
	std::shared_ptr<Model> tmpModel;
	std::unique_ptr<CollisionPrimitive> enmCollision;

	float angle;
	float speed;

	ColorType colorType;
	ActionType actionType;
	DirectX::XMFLOAT3 velocity;
	DirectX::XMFLOAT3 target;

	float spawnTimer;
	float spawnMaxTimer;
	
	bool hasNowSpawn;
	bool isExist;

	void Initialize(EnemyBufferInfo& enm);

	bool Pop(EnemyBufferInfo& enm);
	/// <summary>
	/// プレイヤーを追いかけるエネミー
	/// </summary>
	/// <param name="enm"> エネミーの基本パラメーターの保存先 </param>
	/// <param name="type"> 行動タイプ </param>
	/// <param name="level"> プレイヤーのレベル </param>
	/// <param name="_model"> エネミーのモデル </param>
	/// <param name="_spawnModel"> エネミーのスポーンモデル </param>
	/// <returns></returns>
	bool ChasePop(EnemyBufferInfo& enm, Enemy::ActionType type, int level, std::shared_ptr<Model> _model, std::shared_ptr<Model> _spawnModel);
	
	/// <summary>
	/// 直線状に移動するエネミー
	/// </summary>
	/// <param name="enm"> エネミーの基本パラメーターの保存先 </param>
	/// <param name="p"> スポーン位置 </param>
	/// <param name="level"> プレイヤーのレベル </param>
	/// <param name="type"> エネミーの行動パターン </param>
	/// <param name="_model"> エネミーのモデル </param>
	/// <param name="_spawnModel"> エネミーのスポーン時のモデル </param>
	/// <returns></returns>
	bool LinearPop(EnemyBufferInfo& enm, DirectX::XMFLOAT3 p, int level, Enemy::ActionType type, std::shared_ptr<Model> _model, std::shared_ptr<Model> _spawnModel);
	
	/// <summary>
	/// ランダムな進行ベクトルのほうに進むエネミー
	/// </summary>
	/// <param name="enm"> エネミーの基本パラメーターの保存先 </param>
	/// <param name="type"> 行動パターン </param>
	/// <param name="level"> プレイヤーのレベル </param>
	/// <param name="_model"> エネミーのモデル </param>
	/// <param name="_spawnModel"> エネミーのスポーン時のモデル </param>
	/// <returns></returns>
	bool ReflexPop(EnemyBufferInfo& enm, Enemy::ActionType type, int level, std::shared_ptr<Model> _model, std::shared_ptr<Model> _spawnModel);

	// エネミーの行動関数
	void Move(EnemyBufferInfo& enm, float elapsedTime);
	// ワールド座標計算
	void GiveModelInfo(EnemyBufferInfo& enm);
	// エネミーのパラメーターをクリア
	void EnemyInfClear(EnemyBufferInfo& enm);

	Enemy() {}


	~Enemy() {}
private:

};

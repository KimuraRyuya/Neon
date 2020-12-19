#pragma once

#include <map>
#include <memory>
#include "BG.h"
#include "Player.h"
#include "EnemyManager.h"

//creal
#include <cereal/cereal.hpp>
#include <cereal/types/polymorphic.hpp>
#include <cereal/types/string.hpp>
#include <cereal/archives/binary.hpp>
#include <cereal/archives/json.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/vector.hpp>


class MetaAI
{
public:
	struct MetaAIEnemyEditParameter
	{
		int value;
		int popNum[4];
		float popTime[4];

		MetaAIEnemyEditParameter() {}
		~MetaAIEnemyEditParameter() {}

		void Clear()
		{
			value = 0;
			for (int i = 0; i < 4; i++)
			{
				popNum[i] = 0;
				popTime[i] = 0;
			}
		}



		template<class T>
		void serialize(T& archive, unsigned int version)
		{
			archive
			(
				value,
				popNum[0], popNum[1], popNum[2], popNum[3],
				popTime[0], popTime[1], popTime[2], popTime[3]
			);
		}
	};


private:
	// プレイヤーのアドレス
	std::shared_ptr<Player> player;
	// エネミーマネージャーのアドレス
	std::shared_ptr<EnemyManager> enemyManager;
	// 背景のアドレス(床情報が欲しいため)
	std::shared_ptr<BG> bg;

	//std::map<Enemy::ActionType, >

	std::vector<std::shared_ptr<MetaAIEnemyEditParameter>> enemyPrameter;
	std::map<Enemy::ActionType, std::shared_ptr<MetaAIEnemyEditParameter>> metaAIEnemyEditParameter;

	struct EnemyType
	{
		int Top_to_Bottom_Liner = 5;
		int  Bottom_to_Top_Liner = 5;
		int Right_to_LeftLiner = 5;
		int Left_to_Right_Liner = 5;
		int Chase = 30;
		int Reflection = 30;
		int WARNING = 5;
		
		void Clear()
		{
			Top_to_Bottom_Liner = 5;
			 Bottom_to_Top_Liner = 5;
			Right_to_LeftLiner = 5;
			Left_to_Right_Liner = 5;
			Chase = 30;
			Reflection = 30;
			WARNING = 5;
		}
		
		// 重み計算
		int CalcWeight()
		{ 
			return Top_to_Bottom_Liner + Bottom_to_Top_Liner + Right_to_LeftLiner
				+ Left_to_Right_Liner + Chase + Reflection + WARNING;
		}
	};

	// 次生成するエネミーのパターン
	EnemyType type;
	Enemy::ActionType nextActionType;
	// 次回生成するエネミーの数
	int popEnemyNum;
	// エネミーの生成時間
	float nextEnemyPopTime;

public:
	MetaAI() {}
	~MetaAI() {}

	void Initialize(std::shared_ptr<Player> p, std::shared_ptr<EnemyManager> e, std::shared_ptr<BG> b);
	void Load();
	void Save();
	void Update(float elapsedTime);

	void NextEnemyPopSelect();

	Enemy::ActionType GetNextEnemyType() { return nextActionType; };
	int GetNextPopEnemyNum() { return popEnemyNum; }
	int GetNextEnemyPopTime() { return nextEnemyPopTime; }

};


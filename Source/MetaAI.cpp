#include "MetaAI.h"
#include "../Library/FrameWork.h"
#include <string>

CEREAL_CLASS_VERSION(MetaAI::MetaAIEnemyEditParameter, 1)

void MetaAI::Initialize(std::shared_ptr<Player> p, std::shared_ptr<EnemyManager> e, std::shared_ptr<BG> b)
{
	player = p;
	enemyManager = e;
	bg = b;

	nextEnemyPopTime = 0;

	nextActionType = Enemy::ActionType::Chase;

	popEnemyNum = 0;

	enemyPrameter.resize(Enemy::ActionType::WARNING + 1);

	for (int i = 0; i < Enemy::ActionType::WARNING + 1; i++)
	{
		enemyPrameter.at(i) = std::make_shared<MetaAIEnemyEditParameter>();
		enemyPrameter.at(i)->Clear();
	}

	Load();

	metaAIEnemyEditParameter.insert(std::make_pair(Enemy::ActionType::Top_to_Bottom_Liner, enemyPrameter.at(Enemy::ActionType::Top_to_Bottom_Liner)));
	metaAIEnemyEditParameter.insert(std::make_pair(Enemy::ActionType::Bottom_to_Top_Liner, enemyPrameter.at(Enemy::ActionType::Bottom_to_Top_Liner)));
	metaAIEnemyEditParameter.insert(std::make_pair(Enemy::ActionType::Right_to_LeftLiner, enemyPrameter.at(Enemy::ActionType::Right_to_LeftLiner)));
	metaAIEnemyEditParameter.insert(std::make_pair(Enemy::ActionType::Left_to_Right_Liner, enemyPrameter.at(Enemy::ActionType::Left_to_Right_Liner)));
	metaAIEnemyEditParameter.insert(std::make_pair(Enemy::ActionType::Chase, enemyPrameter.at(Enemy::ActionType::Chase)));
	metaAIEnemyEditParameter.insert(std::make_pair(Enemy::ActionType::Reflection, enemyPrameter.at(Enemy::ActionType::Reflection)));
	metaAIEnemyEditParameter.insert(std::make_pair(Enemy::ActionType::WARNING, enemyPrameter.at(Enemy::ActionType::WARNING)));


	type.Clear();
}

void MetaAI::Load()
{
	// json読み込み
	std::ifstream ifs;
	ifs.open("Data/Document/MetaAIParameter.json", std::ios::out);
	{
		cereal::JSONInputArchive i_archive(ifs);
		i_archive(enemyPrameter);
	}
	ifs.close();
}

void MetaAI::Save()
{
	//cereal でjson とbinary に保存
	static std::string data_name;
	data_name = "Data/Document/MetaAIParameter.json";
	std::ofstream ofs;
	ofs.open(data_name.c_str(), std::ios::out);
	{
		cereal::JSONOutputArchive o_archive(ofs);
		o_archive(enemyPrameter);
	}
	ofs.close();
}

// ルールベースAI
void MetaAI::Update(float elapsedTime)
{
	type.Clear();

	// ImGui
	if(!FrameWork::GetOnGame())
	{
		ImGui::Begin("MetaAI");
		if (ImGui::CollapsingHeader(u8"上から下"))
		{
			for (int i = 0; i < 4; i++)
			{
				std::string text = u8"生成時間" + std::to_string(i);
				ImGui::DragFloat(text.c_str(), &metaAIEnemyEditParameter.at(Enemy::ActionType::Top_to_Bottom_Liner)->popTime[i]);
				text = u8"生成数" + std::to_string(i);
				ImGui::DragInt(text.c_str(), &metaAIEnemyEditParameter.at(Enemy::ActionType::Top_to_Bottom_Liner)->popNum[i]);
			}
			ImGui::DragInt(u8"確率加算値", &metaAIEnemyEditParameter.at(Enemy::ActionType::Top_to_Bottom_Liner)->value);
		}
		if (ImGui::CollapsingHeader(u8"下から上"))
		{
			for (int i = 0; i < 4; i++)
			{
				std::string text = u8"生成時間" + std::to_string(i);
				ImGui::DragFloat(text.c_str(), &metaAIEnemyEditParameter.at(Enemy::ActionType::Bottom_to_Top_Liner)->popTime[i]);
				text = u8"生成数" + std::to_string(i);
				ImGui::DragInt(text.c_str(), &metaAIEnemyEditParameter.at(Enemy::ActionType::Bottom_to_Top_Liner)->popNum[i]);
			}
			ImGui::DragInt(u8"確率加算値", &metaAIEnemyEditParameter.at(Enemy::ActionType::Bottom_to_Top_Liner)->value);
		}
		if (ImGui::CollapsingHeader(u8"左から右"))
		{
			for (int i = 0; i < 4; i++)
			{
				std::string text = u8"生成時間" + std::to_string(i);
				ImGui::DragFloat(text.c_str(), &metaAIEnemyEditParameter.at(Enemy::ActionType::Left_to_Right_Liner)->popTime[i]);
				text = u8"生成数" + std::to_string(i);
				ImGui::DragInt(text.c_str(), &metaAIEnemyEditParameter.at(Enemy::ActionType::Left_to_Right_Liner)->popNum[i]);
			}
			ImGui::DragInt(u8"確率加算値", &metaAIEnemyEditParameter.at(Enemy::ActionType::Left_to_Right_Liner)->value);
		}
		if (ImGui::CollapsingHeader(u8"右から左"))
		{
			for (int i = 0; i < 4; i++)
			{
				std::string text = u8"生成時間" + std::to_string(i);
				ImGui::DragFloat(text.c_str(), &metaAIEnemyEditParameter.at(Enemy::ActionType::Right_to_LeftLiner)->popTime[i]);
				text = u8"生成数" + std::to_string(i);
				ImGui::DragInt(text.c_str(), &metaAIEnemyEditParameter.at(Enemy::ActionType::Right_to_LeftLiner)->popNum[i]);
			}
			ImGui::DragInt(u8"確率加算値", &metaAIEnemyEditParameter.at(Enemy::ActionType::Right_to_LeftLiner)->value);
		}
		if (ImGui::CollapsingHeader(u8"追尾"))
		{
			for (int i = 0; i < 4; i++)
			{
				std::string text = u8"生成時間" + std::to_string(i);
				ImGui::DragFloat(text.c_str(), &metaAIEnemyEditParameter.at(Enemy::ActionType::Chase)->popTime[i]);
				text = u8"生成数" + std::to_string(i);
				ImGui::DragInt(text.c_str(), &metaAIEnemyEditParameter.at(Enemy::ActionType::Chase)->popNum[i]);
			}
			ImGui::DragInt(u8"確率加算値", &metaAIEnemyEditParameter.at(Enemy::ActionType::Chase)->value);
		}
		if (ImGui::CollapsingHeader(u8"反射"))
		{
			for (int i = 0; i < 4; i++)
			{
				std::string text = u8"生成時間" + std::to_string(i);
				ImGui::DragFloat(text.c_str(), &metaAIEnemyEditParameter.at(Enemy::ActionType::Reflection)->popTime[i]);
				text = u8"生成数" + std::to_string(i);
				ImGui::DragInt(text.c_str(), &metaAIEnemyEditParameter.at(Enemy::ActionType::Reflection)->popNum[i]);
			}
			ImGui::DragInt(u8"確率加算値", &metaAIEnemyEditParameter.at(Enemy::ActionType::Reflection)->value);
		}
		if (ImGui::CollapsingHeader(u8"全方位"))
		{
			for (int i = 0; i < 4; i++)
			{
				std::string text = u8"生成時間" + std::to_string(i);
				ImGui::DragFloat(text.c_str(), &metaAIEnemyEditParameter.at(Enemy::ActionType::WARNING)->popTime[i]);
				text = u8"生成数" + std::to_string(i);
				ImGui::DragInt(text.c_str(), &metaAIEnemyEditParameter.at(Enemy::ActionType::WARNING)->popNum[i]);
			}
			ImGui::DragInt(u8"確率加算値", &metaAIEnemyEditParameter.at(Enemy::ActionType::WARNING)->value);
		}
		if (ImGui::Button("Save"))
		{
			Save();
		}
		if (ImGui::Button("Load"))
		{
			Load();
		}

		ImGui::End();
	}


	// Playerの情報を見てAIを調整
	{
		if (player->GetReinforce())
		{
			type.Top_to_Bottom_Liner += metaAIEnemyEditParameter.at(Enemy::ActionType::Top_to_Bottom_Liner)->value;
			type.Bottom_to_Top_Liner += metaAIEnemyEditParameter.at(Enemy::ActionType::Bottom_to_Top_Liner)->value;
			type.Right_to_LeftLiner += metaAIEnemyEditParameter.at(Enemy::ActionType::Right_to_LeftLiner)->value;
			type.Left_to_Right_Liner += metaAIEnemyEditParameter.at(Enemy::ActionType::Left_to_Right_Liner)->value;
			type.WARNING += metaAIEnemyEditParameter.at(Enemy::ActionType::WARNING)->value;
		}
		else
		{
			type.Chase += metaAIEnemyEditParameter.at(Enemy::ActionType::Chase)->value;
			type.Reflection += metaAIEnemyEditParameter.at(Enemy::ActionType::Reflection)->value;
		}
	}

	// BGの情報を見てAIを調整
	{
		if (bg->GetNowFillNum() > 550)
		{
			type.WARNING += metaAIEnemyEditParameter.at(Enemy::ActionType::WARNING)->value;
		}
	}

	// 調整した値を使ってランダムに決める
	{
		NextEnemyPopSelect();
	}

	// EnemyManagerの情報を見ながら調整
	{
		int existNum = enemyManager->GetEnemyExistNum();
		if (existNum > 30)
		{
			nextEnemyPopTime = metaAIEnemyEditParameter.at(nextActionType)->popTime[3];
			popEnemyNum = metaAIEnemyEditParameter.at(nextActionType)->popNum[3];
		}
		else if (existNum > 20)
		{
			nextEnemyPopTime = metaAIEnemyEditParameter.at(nextActionType)->popTime[2];
			popEnemyNum = metaAIEnemyEditParameter.at(nextActionType)->popNum[2];
		}
		else if (existNum > 10)
		{
			nextEnemyPopTime = metaAIEnemyEditParameter.at(nextActionType)->popTime[1];
			popEnemyNum = metaAIEnemyEditParameter.at(nextActionType)->popNum[1];
		}
		else if (existNum <= 10)
		{
			nextEnemyPopTime = metaAIEnemyEditParameter.at(nextActionType)->popTime[0];
			popEnemyNum = metaAIEnemyEditParameter.at(nextActionType)->popNum[0];
		}
	}

	enemyManager->SetNextEnemyParamerter(nextActionType, popEnemyNum, nextEnemyPopTime);

}

void MetaAI::NextEnemyPopSelect()
{
	bool canSelectEnemy = true;
	int totalWeight = type.CalcWeight();
	int rnd = rand() % totalWeight;


	if (type.Top_to_Bottom_Liner > rnd && canSelectEnemy)
	{
		nextActionType = Enemy::ActionType::Top_to_Bottom_Liner;
		canSelectEnemy = false;
	}
	else rnd -= type.Top_to_Bottom_Liner;

	if (type.Bottom_to_Top_Liner > rnd && canSelectEnemy)
	{
		nextActionType = Enemy::ActionType::Bottom_to_Top_Liner;
		canSelectEnemy = false;
	}
	else rnd -= type.Bottom_to_Top_Liner;

	if (type.Right_to_LeftLiner > rnd && canSelectEnemy)
	{
		nextActionType = Enemy::ActionType::Right_to_LeftLiner;
		canSelectEnemy = false;
	}
	else rnd -= type.Right_to_LeftLiner;

	if (type.Left_to_Right_Liner > rnd && canSelectEnemy)
	{
		nextActionType = Enemy::ActionType::Left_to_Right_Liner;
		canSelectEnemy = false;
	}
	else rnd -= type.Left_to_Right_Liner;

	if (type.Chase > rnd && canSelectEnemy && canSelectEnemy)
	{
		nextActionType = Enemy::ActionType::Chase;
		canSelectEnemy = false;
	}
	else rnd -= type.Chase;

	if (type.Reflection > rnd && canSelectEnemy)
	{
		nextActionType = Enemy::ActionType::Reflection;
		canSelectEnemy = false;
	}
	else rnd -= type.Reflection;

	if (type.WARNING > rnd && canSelectEnemy)
	{
		nextActionType = Enemy::ActionType::WARNING;
		canSelectEnemy = false;
	}
	else rnd -= type.WARNING;
}

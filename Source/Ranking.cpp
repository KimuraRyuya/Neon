#include "Ranking.h"
#include "../Library/MyLib.h"
#include "SceneManager.h"
#include <fstream>
#include <string>
#include <imgui.h>

void Ranking::Initialize()
{
	for (int i = 0; i < 5; i++)
	{
		bestScore[i] = 0;
		bestEndlessScore[i] = 0;
	}
	Load();
}

void Ranking::Sort(int s, SceneState nowScene)
{
	nowScore = s;
	int tmp[6];
	// モードによって保存先が違うため
	if (nowScene == SceneState::GAME)
	{
		// 一度tmpに入れる
		for (int i = 0; i < 5; i++)
		{
			tmp[i] = bestScore[i];
		}
		tmp[5] = nowScore;
	}
	else if (nowScene == SceneState::ENDLESS)
	{
		// 一度tmpに入れる
		for (int i = 0; i < 5; i++)
		{
			tmp[i] = bestEndlessScore[i];
		}
		tmp[5] = nowScore;
	}

	// tmpの情報をソートする
	for (int i = 0; i < 6; ++i)
	{
		for (int j = i + 1; j < 6; ++j)
		{
			if (tmp[i] < tmp[j])
			{
				int _tmp = tmp[i];
				tmp[i] = tmp[j];
				tmp[j] = _tmp;
			}
		}
	}

	// 上位5個を保存する
	if (nowScene == SceneState::GAME)
	{
		for (int i = 0; i < 5; i++)
		{
			bestScore[i] = tmp[i];
		}
	}
	else if (nowScene == SceneState::ENDLESS)
	{
		for (int i = 0; i < 5; i++)
		{
			bestEndlessScore[i] = tmp[i];
		}
	}

	// セーブする
	Save();
}

void Ranking::Save()
{
	//cereal でjson とbinary に保存
	static std::string data_name;
	data_name = "Data/Document/Ranking.json";
	std::ofstream ofs;
	ofs.open(data_name.c_str(), std::ios::out);
	{
		cereal::JSONOutputArchive o_archive(ofs);
		o_archive(*this);
	}
	ofs.close();
}

void Ranking::Load()
{
	// json読み込み
	std::ifstream ifs;
	ifs.open(L"Data/Document/Ranking.json", std::ios::out);
	{
		cereal::JSONInputArchive i_archive(ifs);
		i_archive(*this);
	}
	ifs.close();
}
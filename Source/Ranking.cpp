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
	// ���[�h�ɂ���ĕۑ��悪�Ⴄ����
	if (nowScene == SceneState::GAME)
	{
		// ��xtmp�ɓ����
		for (int i = 0; i < 5; i++)
		{
			tmp[i] = bestScore[i];
		}
		tmp[5] = nowScore;
	}
	else if (nowScene == SceneState::ENDLESS)
	{
		// ��xtmp�ɓ����
		for (int i = 0; i < 5; i++)
		{
			tmp[i] = bestEndlessScore[i];
		}
		tmp[5] = nowScore;
	}

	// tmp�̏����\�[�g����
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

	// ���5��ۑ�����
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

	// �Z�[�u����
	Save();
}

void Ranking::Save()
{
	//cereal ��json ��binary �ɕۑ�
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
	// json�ǂݍ���
	std::ifstream ifs;
	ifs.open(L"Data/Document/Ranking.json", std::ios::out);
	{
		cereal::JSONInputArchive i_archive(ifs);
		i_archive(*this);
	}
	ifs.close();
}
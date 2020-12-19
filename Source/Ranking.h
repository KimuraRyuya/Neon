#pragma once
#undef max
#undef min

#include "../Library/Billboard.h"
#include "../Library/Bloom.h"
#include "../Library/FrameBuffer.h"
#include "../Library/ToneMap.h"
#include "Scene.h"
#include "BG.h"
#include "../Library/MyLib.h"
//creal
#include <cereal/cereal.hpp>
#include <cereal/types/string.hpp>
#include <cereal/archives/binary.hpp>
#include <cereal/archives/json.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/vector.hpp>

class Ranking
{
private:
	int scoreDigit[5];
	int scoreframe[5][20];

	int nowScore;
	float dissolve;
	float addDissolve;

public:
	int bestScore[5];
	int bestEndlessScore[5];

	void Initialize();

	/// <summary>
	/// スコアのソート
	/// </summary>
	/// <param name="s"> 今回のスコア </param>
	/// <param name="nowScene"> どのゲームモードか </param>
	void Sort(int s, SceneState nowScene);
	// スコアのセーブ
	void Save();
	// スコアの読み込み
	void Load();

	template<class Archive>
	void serialize(Archive& archive)
	{
		archive(
			cereal::make_nvp("Top1", bestScore[0]),
			cereal::make_nvp("Top2", bestScore[1]),
			cereal::make_nvp("Top3", bestScore[2]),
			cereal::make_nvp("Top4", bestScore[3]),
			cereal::make_nvp("Top5", bestScore[4]),

			cereal::make_nvp("EndlessTop5", bestEndlessScore[0]),
			cereal::make_nvp("EndlessTop5", bestEndlessScore[1]),
			cereal::make_nvp("EndlessTop5", bestEndlessScore[2]),
			cereal::make_nvp("EndlessTop5", bestEndlessScore[3]),
			cereal::make_nvp("EndlessTop5", bestEndlessScore[4])

		);
	}

};
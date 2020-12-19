#pragma once

namespace Constant
{
	// プレイヤーの最大弾数
	const static int SHOT_MAX =	200;
	// エネミーの最大数
	const static int MAX_ENEMY = 200;
	// 塗られたエリアにいるときのバフエフェクト数
	const static int MAX_FILLEFFECT = 20;
	// 塗られた瞬間のエフェクト数
	const static int MAX_FILL_EFFECT_LOCUS = 400;
	// 敵を倒したときに出てくるscoreボール
	const static int MAX_TRACKINGSCORE = 100;

	const static int MAP_WIDTH = 45;
	const static int MAP_HEIGHT = 45;

	// BG用 塗られてないときの輝度
	const static float minLuminans = 0.0f;
	// BG用 塗られら時の輝度
	const static float maxLuminans = 2.0f;

}
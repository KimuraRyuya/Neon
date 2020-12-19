#pragma once
#include "../Library/Model.h"
#include "../Library/ModelRenderer.h"
#include "../Library/ModelResource.h"
#include "../Library/Billboard.h"

#include "OBJ3D.h"
#include "Collision.h"
#include "Player.h"
#include "EnemyManager.h"

#include <memory>
#include <iostream>
#include <float.h> 
#include <math.h> 

class BG
{
private:
	
	// 塗られた時のエフェクトのPram
	struct FillEffect
	{
		DirectX::XMFLOAT3 pos;
		float time;
		float maxTime;
		float scalar;
		bool isExist;
		void Clear()
		{
			pos = { 0.0f, 0.0f, 0.0f };
			scalar = 0.0f;
			time = 0.0f;
			maxTime = 0.0f;
			isExist = false;
		}
	};
	FillEffect fillEffect[Constant::MAX_FILL_EFFECT_LOCUS];


	std::unique_ptr<ModelRenderer> modelRenderer;
	std::unique_ptr<Billboard> hexTex;
	
	int nowFillNum;
	int fillNum;
	int ultimateFillDestroy;
	DirectX::XMFLOAT2 fillPos[Constant::MAP_WIDTH * Constant::MAP_HEIGHT];

	int tmpMapChipNum[2][100];
	int tmpMapChipSize;
	bool isCheck[Constant::MAP_WIDTH][Constant::MAP_HEIGHT];
	bool checkPreparation[2];

	// 危険信号
	int warningNum;
	float warningTime;
	bool isWarningChange;
	bool isWarning;


public:
	int fillMaxSize;

	OBJ3D bgObj;
	OBJ3D hexObj[Constant::MAP_WIDTH][Constant::MAP_HEIGHT];
	DirectX::XMFLOAT4 stageLuminans;
	DirectX::XMFLOAT4 luminans[Constant::MAP_WIDTH][Constant::MAP_HEIGHT];
public:
	BG() {}
	~BG() { Finalize(); }
	void Initialize();
	int Update(std::shared_ptr<Player> p, std::shared_ptr<EnemyManager> enm, float elapsedTime);
	// checkフラグの初期化
	void CheckInitialize();
	void Update(float elapsedTime) {}
	void FillEffectUpdate(float elapsedTime);
	/// <summary>
	/// 塗られた時のエフェクト生成関数
	/// </summary>
	/// <param name="p"> 塗られたタイツの座標 </param>
	void EmitFillEffext(std::vector<DirectX::XMFLOAT3> p);
	void Render();
	void Finalize();

	void Reset();

	void SetPlayerBase(DirectX::XMFLOAT3 pos);

	void UltimateEffectAndCalcFill(std::shared_ptr<Player> p);

	// アルティメット時に床を消す量
	int GetUltimateFillDestroy() { return ultimateFillDestroy; }

	// プレイヤーの座標が村れているかどうか
	bool CheckPlayerFillPos(DirectX::XMFLOAT3 p);

	//エネミーの場所を塗りつぶす
	void EnemySpawnFill(std::shared_ptr<EnemyManager> enm);

	void Warning() { isWarning = true; }
	
	// 塗っていいかCheckする関数
	bool CheckFill(int wid, int hei);
	
	/// <summary>
	/// 囲われた場所を塗る
	/// </summary>
	/// <param name="enm"> 囲んだ場所にエネミーがいたらkillさせるために </param>
	/// <returns></returns>
	DirectX::XMFLOAT2 Fill(std::shared_ptr<EnemyManager> enm);
	
	// 囲われているか探査
	bool Probe(int wid, int hei);

	int GetNowFillNum() { return nowFillNum; }
};
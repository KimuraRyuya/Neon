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
	
	// �h��ꂽ���̃G�t�F�N�g��Pram
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

	// �댯�M��
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
	// check�t���O�̏�����
	void CheckInitialize();
	void Update(float elapsedTime) {}
	void FillEffectUpdate(float elapsedTime);
	/// <summary>
	/// �h��ꂽ���̃G�t�F�N�g�����֐�
	/// </summary>
	/// <param name="p"> �h��ꂽ�^�C�c�̍��W </param>
	void EmitFillEffext(std::vector<DirectX::XMFLOAT3> p);
	void Render();
	void Finalize();

	void Reset();

	void SetPlayerBase(DirectX::XMFLOAT3 pos);

	void UltimateEffectAndCalcFill(std::shared_ptr<Player> p);

	// �A���e�B���b�g���ɏ���������
	int GetUltimateFillDestroy() { return ultimateFillDestroy; }

	// �v���C���[�̍��W������Ă��邩�ǂ���
	bool CheckPlayerFillPos(DirectX::XMFLOAT3 p);

	//�G�l�~�[�̏ꏊ��h��Ԃ�
	void EnemySpawnFill(std::shared_ptr<EnemyManager> enm);

	void Warning() { isWarning = true; }
	
	// �h���Ă�����Check����֐�
	bool CheckFill(int wid, int hei);
	
	/// <summary>
	/// �͂�ꂽ�ꏊ��h��
	/// </summary>
	/// <param name="enm"> �͂񂾏ꏊ�ɃG�l�~�[��������kill�����邽�߂� </param>
	/// <returns></returns>
	DirectX::XMFLOAT2 Fill(std::shared_ptr<EnemyManager> enm);
	
	// �͂��Ă��邩�T��
	bool Probe(int wid, int hei);

	int GetNowFillNum() { return nowFillNum; }
};
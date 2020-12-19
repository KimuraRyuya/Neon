#include "BG.h"

#include "Constant.h"
#include "Collision.h"
#include "PlayerShot.h"
#include "ParticleSystem.h"
#include "../Library/MyLib.h"
#include "../Library/Camera.h"
#include "../Library/FbxLoader.h"
#include "../Library/FrameWork.h"
#include "../Library/Easing.h"
#include "Sound.h"
#include <imgui.h>

void BG::Initialize()
{
	// Stage Load
	{
		const char* fbx_filename = "Data/BG/Bg.fbx";
		std::unique_ptr<ModelData> model_data = std::make_unique<ModelData>();
		FbxLoader fbx_loader;
		fbx_loader.Load(fbx_filename, *model_data);

		std::shared_ptr<ModelResource> modelResource = std::make_shared<ModelResource>(pFrameWork.GetDevice(), std::move(model_data), true);
		bgObj.Initialize(modelResource);
		bgObj.SetPosY(-1.5f);
		bgObj.SetScale({ 0.8f, 0.8f, 0.8f });
		modelRenderer = std::make_unique<ModelRenderer>(pFrameWork.GetDevice());
	}

	// Floor tail Load
	{
		const char* fbx_filename = "Data/BG/Hexgon.fbx";
		std::unique_ptr<ModelData> model_data = std::make_unique<ModelData>();
		FbxLoader fbx_loader;
		fbx_loader.Load(fbx_filename, *model_data);

		std::shared_ptr<ModelResource> modelResource = std::make_shared<ModelResource>(pFrameWork.GetDevice(), std::move(model_data), true);

		hexTex = std::make_unique<Billboard>(L"Data/Texture/floorHex.png");

		// 地面の設置
		int k = 0;
		for (int i = 0; i < Constant::MAP_HEIGHT; i++)
		{
			for (int j = 0; j < Constant::MAP_WIDTH; j++)
			{
				fillPos[k] = DirectX::XMFLOAT2(0.0f, 0.0f);
				k++;
				isCheck[j][i] = false;
				// 半分ずらす
				int adjust = i % 2;
				hexObj[j][i].Initialize(modelResource);
				hexObj[j][i].SetPos({ -47.0f + 2.0f * j + adjust, -1.5f, -47.0f + 2.0f * i });
				luminans[j][i] = DirectX::XMFLOAT4(Constant::minLuminans, Constant::minLuminans, Constant::minLuminans, 1.0f);
				DirectX::XMFLOAT2 _p = DirectX::XMFLOAT2(hexObj[j][i].GetPos().x, hexObj[j][i].GetPos().z);
			}
		}
	}

	for (auto& s : fillEffect)
	{
		s.Clear();
	}

	fillNum = 0;

	for (int i = 0; i < 100; i++)
	{
		tmpMapChipNum[0][i] = 0;
		tmpMapChipNum[1][i] = 0;
	}
	tmpMapChipSize = 0;

	warningTime = 0.0f;
	warningNum = 0;
	isWarningChange = false;
	isWarning = false;
	
	fillMaxSize = 0;
	nowFillNum = 0;
	ultimateFillDestroy = 0;

	checkPreparation[0] = false;
	checkPreparation[1] = false;

	stageLuminans = DirectX::XMFLOAT4(20.0f, 20.0f, 25.0f, 1.0f);
}

void BG::Reset()
{
	bgObj.SetPosY(-1.5f);
	bgObj.SetScale({ 0.8f, 0.8f, 0.8f });

	// 地面の配置
	int k = 0;
	for (int i = 0; i < Constant::MAP_HEIGHT; i++)
	{
		for (int j = 0; j < Constant::MAP_WIDTH; j++)
		{
			fillPos[k] = DirectX::XMFLOAT2(0.0f, 0.0f);
			k++;
			isCheck[j][i] = false;
			// 半分ずらす
			int adjust = i % 2;
			hexObj[j][i].SetPos({ -47.0f + 2.0f * j + adjust, -1.5f, -47.0f + 2.0f * i });
			luminans[j][i] = DirectX::XMFLOAT4(Constant::minLuminans, Constant::minLuminans, Constant::minLuminans, 1.0f);
			DirectX::XMFLOAT2 _p = DirectX::XMFLOAT2(hexObj[j][i].GetPos().x, hexObj[j][i].GetPos().z);
		}
	}

	for (auto& s : fillEffect)
	{
		s.Clear();
	}

	for (int i = 0; i < 100; i++)
	{
		tmpMapChipNum[0][i] = 0;
		tmpMapChipNum[1][i] = 0;
	}
	tmpMapChipSize = 0;
	ultimateFillDestroy = 0;
	
	warningNum = 0;
	warningTime = 0.0f;
	isWarning = false;
	isWarningChange = false;

	fillMaxSize = 0;
	fillNum = 0;
	nowFillNum = 0;

	checkPreparation[0] = false;
	checkPreparation[1] = false;

	stageLuminans = DirectX::XMFLOAT4(20.0f, 20.0f, 25.0f, 1.0f);
}

int BG::Update(std::shared_ptr<Player> p, std::shared_ptr<EnemyManager> enm, float elapsedTime)
{
	DirectX::XMFLOAT2 _fillScore = { 0.0f, 0.0f };

	// Enemyが通ったら床を消す
	{
		for (int k = 0; k < Constant::MAX_ENEMY; k++)
		{
			if (enm->GetEnemyInfo(k).hp <= 0) continue;

			DirectX::XMFLOAT2 _p[1] = {
				DirectX::XMFLOAT2(enm->GetEnemyInfo(k).pos.x + 47.0f, enm->GetEnemyInfo(k).pos.z + 47.0f),
			};

			int i, j;
			for (int k = 0; k < 1; k++)
			{
				i = static_cast<int>(_p[k].y / 2);
				j = static_cast<int>(_p[k].x / 2);
				fillNum++;
				luminans[j][i] = DirectX::XMFLOAT4(Constant::minLuminans, Constant::minLuminans, Constant::minLuminans, 1.0f);
			}
		}

	}


	// Playerが通ったら床を表示する
	{
		if (!p->GetNewSpawn())
		{

			DirectX::XMFLOAT2 _p[5] = {
				DirectX::XMFLOAT2(p->GetPos().x + 47.0f, p->GetPos().z + 47.0f),
				DirectX::XMFLOAT2(p->GetPos().x + 0.5f + 47.0f, p->GetPos().z + 47.0f),
				DirectX::XMFLOAT2(p->GetPos().x - 0.5f + 47.0f, p->GetPos().z + 47.0f),
				DirectX::XMFLOAT2(p->GetPos().x + 47.0f, p->GetPos().z + 0.5f + 47.0f),
				DirectX::XMFLOAT2(p->GetPos().x + 47.0f, p->GetPos().z - 0.5f + 47.0f)
			};
			int i, j;
			for (int k = 0; k < 5; k++)
			{
				i = static_cast<int>(_p[k].y / 2);
				j = static_cast<int>(_p[k].x / 2);
				if (luminans[j][i].x < Constant::maxLuminans)
				{
					fillNum++;
					p->AddDushGauge(1.0f);
					luminans[j][i] = DirectX::XMFLOAT4(Constant::maxLuminans, Constant::maxLuminans, Constant::maxLuminans, 1.0f);
				}
			}
		}
	}

	// 現在何マス塗られてるか
	{

		int _fillCnt = 0;
		for (int i = 0; i < Constant::MAP_HEIGHT; i++)
		{
			for (int j = 0; j < Constant::MAP_WIDTH; j++)
			{
				if (luminans[j][i].x >= Constant::maxLuminans)
				{
					_fillCnt++;
				}
			}
		}

		nowFillNum = _fillCnt;

		// 400以上塗られていたらPlayerの必殺技フラグon
		if (_fillCnt > 400)
		{
			p->SetFillChipDestory(true);
		}
		else if (_fillCnt < 350)
		{
			p->SetFillChipDestory(false);
		}

	}

	// ステージ点滅
	if(isWarning)
	{
		if (!isWarningChange)
		{
			stageLuminans.z = Easing::InSine(warningTime, 1.0f, 2.0f, 25.0f);
			stageLuminans.y = Easing::InSine(warningTime, 1.0f, 2.0f, 20.0f);
		}
		else
		{
			stageLuminans.z = Easing::InSine(warningTime, 1.0f, 25.0f, 2.0f);
			stageLuminans.y = Easing::InSine(warningTime, 1.0f, 20.0f, 2.0f);
		}
		if (warningTime > 1.0f)
		{
			if (!isWarningChange) isWarningChange = true;
			else isWarningChange = false;
			warningTime = 0.0f;
			warningNum++;
		}
		warningTime += 1.0f * elapsedTime;
		if (warningNum > 3)
		{
			stageLuminans.z = 25.0f;
			stageLuminans.y = 20.0f;
			warningNum = 0;
			warningTime = 0.0f;
			isWarning = false;
			isWarningChange = false;
		}
	}
	else
	{
		stageLuminans.z = 25.0f;
		stageLuminans.y = 20.0f;
		warningNum = 0;
		warningTime = 0.0f;
		isWarning = false;
		isWarningChange = false;
	}


	// Checkの初期化
	{
		CheckInitialize();
		for (int i = 0; i < tmpMapChipSize; i++)
		{
			tmpMapChipNum[0][i] = 0;
			tmpMapChipNum[1][i] = 0;
		}
		tmpMapChipSize = 0;
	}

	// 探査処理
	{
		for (int i = Constant::MAP_HEIGHT - 1; i >= 0; i--)
		{
			// 横Lineのチェック準備フラグ
			checkPreparation[0] = false;
			checkPreparation[1] = false;
			fillMaxSize = 0;

			for (int j = Constant::MAP_WIDTH - 1; j >= 0; j--)
			{
				if (luminans[j][i].x == Constant::minLuminans && checkPreparation[0])
				{
					// Fill箇所発見後にFillされてないとこ発見
					checkPreparation[1] = true;
				}
				// 捜査対象発見
				if (luminans[j][i].x >= Constant::maxLuminans && checkPreparation[0] && checkPreparation[1])
				{
					int leftAdd = 1;
					while (1)
					{
						checkPreparation[1] = false;
						if (luminans[j + leftAdd][i].x >= Constant::maxLuminans)
						{
							{
								tmpMapChipNum[0][tmpMapChipSize] = j + leftAdd - 1;
								tmpMapChipNum[1][tmpMapChipSize] = i;
							}
							tmpMapChipSize++;
							break;
						}
						leftAdd++;
					}
				}
				else if (luminans[j][i].x >= Constant::maxLuminans)
				{
					// 一個目のFill箇所発見
					checkPreparation[0] = true;
				}
			}
		}
	}

	float _tmpScore = 0.0f;
	//探索対象が塗れるかCheck
	{
		for (int i = 0; i < tmpMapChipSize; i++)
		{
			if (CheckFill(tmpMapChipNum[0][i], tmpMapChipNum[1][i]))
			{
				_fillScore = Fill(enm);
				if (_fillScore.x > 400.0f)
				{
					float _soundV = fillMaxSize / 100.0f;
					if (_soundV > 1.0f) _soundV = 1.0f;
					Sound::GetInstance()->floorFill->SetVolume(_soundV);
					Sound::GetInstance()->floorFill->PlayWAV();
					p->AddDushGauge(_fillScore.y);
					_tmpScore += _fillScore.x;
				}
				fillMaxSize = 0;
				CheckInitialize();
			}
			else
			{
				CheckInitialize();
				fillMaxSize = 0;
			}
		}
	}

	FillEffectUpdate(elapsedTime);
	//EnemySpawnFill(enm);
	
	return static_cast<int>(_tmpScore);
}

void BG::CheckInitialize()
{
	for (int i = 0; i < Constant::MAP_HEIGHT; i++)
	{
		for (int j = 0; j < Constant::MAP_WIDTH; j++)
		{
			isCheck[j][i] = false;
		}
	}
}

void BG::Render()
{
	modelRenderer->Begin(pFrameWork.GetContext());
	bgObj.GiveModelInfo();
	modelRenderer->Draw(pFrameWork.GetContext(), bgObj.GetModel(), stageLuminans);
	modelRenderer->End(pFrameWork.GetContext());

	hexTex->Begin();
	for (auto& s : fillEffect)
	{
		if (!s.isExist) continue;
		hexTex->Render(s.pos, 0.0f, 0.0f, 1024.0f, 1024.0f, { -90.0f * 0.01745f, 0.0f, 0.0f }, { 2.0f, 2.0f }, { 6.0f, 6.0f, 6.0f, s.scalar }, { 1.0f, 1.0f, 1.0f, s.scalar }, false);
	}
	hexTex->End();


	DirectX::XMFLOAT4X4 _v;
	DirectX::XMStoreFloat4x4(&_v, Camera::GetInstance().GetViewMatrix());

	modelRenderer->Begin(pFrameWork.GetContext());
	for (int i = 0; i < Constant::MAP_HEIGHT; i++)
	{
		for (int j = 0; j < Constant::MAP_WIDTH; j++)
		{
			if (luminans[j][i].w <= 0.0f) continue;
			hexObj[j][i].GiveModelInfo();
			modelRenderer->Draw(pFrameWork.GetContext(), hexObj[j][i].GetModel(), luminans[j][i]);
		}
	}
	modelRenderer->End(pFrameWork.GetContext());

}

void BG::Finalize()
{

}

void BG::SetPlayerBase(DirectX::XMFLOAT3 pos)
{
	DirectX::XMFLOAT2 _p[5] = {
	DirectX::XMFLOAT2(pos.x + 47.0f, pos.z + 47.0f),
	DirectX::XMFLOAT2(pos.x + 0.5f + 47.0f, pos.z + 47.0f),
	DirectX::XMFLOAT2(pos.x - 0.5f + 47.0f, pos.z + 47.0f),
	DirectX::XMFLOAT2(pos.x + 47.0f, pos.z + 0.5f + 47.0f),
	DirectX::XMFLOAT2(pos.x + 47.0f, pos.z - 0.5f + 47.0f)
	};
	int i, j;
	for (int k = 0; k < 5; k++)
	{
		i = static_cast<int>(_p[k].y / 2);
		j = static_cast<int>(_p[k].x / 2);
		if (luminans[j][i].x < Constant::maxLuminans)
		{
			fillNum++;
			luminans[j][i] = DirectX::XMFLOAT4(Constant::maxLuminans, Constant::maxLuminans, Constant::maxLuminans, 1.0f);
		}
	}
}

bool BG::CheckFill(int wid, int hei)
{
	// ずらし
	if (hei % 2 == 0) 
	{
		if (luminans[wid - 1][hei + 1].x < Constant::maxLuminans || luminans[wid][hei + 1].x < Constant::maxLuminans) return false;
	}
	else if (hei % 2 == 1)
	{
		if (luminans[wid][hei + 1].x < Constant::maxLuminans || luminans[wid + 1][hei + 1].x < Constant::maxLuminans) return false;
	}

	//luminans[wid + j][hei + i] = DirectX::XMFLOAT4(10.0f, 10.0f, 10.0f, 1.0f);// 中心
	//luminans[wid + j][hei + i + 1] = DirectX::XMFLOAT4(10.0f, 10.0f, 10.0f, 1.0f); // 右上
	//luminans[wid + j - 1][hei + i] = DirectX::XMFLOAT4(10.0f, 10.0f, 10.0f, 1.0f); // 右
	//luminans[wid + j][hei + i - 1] = DirectX::XMFLOAT4(10.0f, 10.0f, 10.0f, 1.0f); // 右下
	//luminans[wid + j + 1][hei + i - 1] = DirectX::XMFLOAT4(10.0f, 10.0f, 10.0f, 1.0f); // 左下
	//luminans[wid + j + 1][hei + i] = DirectX::XMFLOAT4(10.0f, 10.0f, 10.0f, 1.0f); // 左
	//luminans[wid + j + 1][hei + i + 1] = DirectX::XMFLOAT4(10.0f, 10.0f, 10.0f, 1.0f); // 左上

	isCheck[wid][hei] = true;
	if (!Probe(wid, hei))
	{
		fillPos[fillMaxSize] = DirectX::XMFLOAT2(static_cast<float>(wid), static_cast<float>(hei));
		fillMaxSize++;
		return true;
	}



	for (int i = 0; i < fillMaxSize; i++)
	{
		if (static_cast<int>(fillPos[i].y) == Constant::MAP_HEIGHT - 1) return false;
		if (static_cast<int>(fillPos[i].x) == Constant::MAP_WIDTH - 1) return false;
		if (static_cast<int>(fillPos[i].y) == 0) return false;
		if (static_cast<int>(fillPos[i].x) == 0) return false;
		Probe(static_cast<int>(fillPos[i].x), static_cast<int>(fillPos[i].y));
	}
	fillPos[fillMaxSize] = DirectX::XMFLOAT2(static_cast<float>(wid), static_cast<float>(hei));
	fillMaxSize++;
	return true;

}

bool BG::Probe(int wid, int hei)
{
	bool closed[6] = {};
	int adjustL = 0, adjustR = 0;
	if (hei % 2 == 0)
	{
		adjustL = wid;
		adjustR = wid - 1;
	}
	else if (hei % 2 == 1)
	{
		adjustL = wid + 1;
		adjustR = wid;
	}

	// 右上

	if (luminans[adjustR][hei + 1].x < Constant::maxLuminans && !isCheck[adjustR][hei + 1])
	{
		fillPos[fillMaxSize] = DirectX::XMFLOAT2(static_cast<float>(adjustR), static_cast<float>(hei + 1));
		fillMaxSize++;
		isCheck[adjustR][hei + 1] = true;
		closed[0] = true;
	}
	else
	{
		closed[0] = false;
	}

	// 右
	if (luminans[wid - 1][hei].x < Constant::maxLuminans && !isCheck[wid - 1][hei])
	{
		fillPos[fillMaxSize] = DirectX::XMFLOAT2(static_cast<float>(wid - 1), static_cast<float>(hei));
		fillMaxSize++;
		isCheck[wid - 1][hei] = true;
		closed[1] = true;
	}
	else
	{
		closed[1] = false;
	}

	// 右下
	if (luminans[adjustR][hei - 1].x < Constant::maxLuminans && !isCheck[adjustR][hei - 1])
	{
		fillPos[fillMaxSize] = DirectX::XMFLOAT2(static_cast<float>(adjustR), static_cast<float>(hei - 1));
		fillMaxSize++;
		isCheck[adjustR][hei - 1] = true;
		closed[2] = true;
	}
	else
	{
		closed[2] = false;
	}

	// 左下
	if (luminans[adjustL][hei - 1].x < Constant::maxLuminans && !isCheck[adjustL][hei - 1])
	{
		fillPos[fillMaxSize] = DirectX::XMFLOAT2(static_cast<float>(adjustL), static_cast<float>(hei - 1));
		fillMaxSize++;
		isCheck[adjustL][hei - 1] = true;
		closed[3] = true;
	}
	else
	{
		closed[3] = false;
	}

	// 左
	if (luminans[wid + 1][hei].x < Constant::maxLuminans && !isCheck[wid + 1][hei])
	{
		fillPos[fillMaxSize] = DirectX::XMFLOAT2(static_cast<float>(wid + 1), static_cast<float>(hei));
		fillMaxSize++;
		isCheck[wid + 1][hei] = true;
		closed[4] = true;
	}
	else
	{
		closed[4] = false;
	}

	// 左上
	if (luminans[adjustL][hei + 1].x < Constant::maxLuminans && !isCheck[adjustL][hei + 1])
	{
		fillPos[fillMaxSize] = DirectX::XMFLOAT2(static_cast<float>(adjustL), static_cast<float>(hei + 1));
		fillMaxSize++;
		isCheck[adjustL][hei + 1] = true;
		closed[5] = true;
	}
	else
	{
		closed[5] = false;
	}

	for (int i = 0; i < 6; i++)
	{
		if (closed[i])
		{
			// 探査対象対象発見
			return true;
		}
	}
	// 探査対象未発見
	return false;

}

DirectX::XMFLOAT2 BG::Fill(std::shared_ptr<EnemyManager> enm)
{
	std::vector<DirectX::XMFLOAT3> tmpP;
	int _fillNum = 0;
	int _fillScore = 0;
	for (int k = 0; k < fillMaxSize; k++)
	{
		int i = static_cast<int>(fillPos[k].y), j = static_cast<int>(fillPos[k].x);
		luminans[j][i] = DirectX::XMFLOAT4(Constant::maxLuminans * 2.0f, Constant::maxLuminans * 2.0f, Constant::maxLuminans * 2.0f, 1.0f);
		_fillNum++;

		// 塗られる床のPosをtmp
		tmpP.emplace_back(hexObj[j][i].GetPos());

		float _addScore = 10.0f * (1.01f * static_cast<float>(_fillNum));
		_fillScore += static_cast<int>(_addScore);

		for (int l = 0; l < Constant::MAX_ENEMY; l++)
		{
			if (enm->GetEnemyInfo(l).hp <= 0) continue;

			DirectX::XMFLOAT2 _e = DirectX::XMFLOAT2(enm->GetEnemyInfo(l).pos.x + 47.0f, enm->GetEnemyInfo(l).pos.z + 47.0f);
			int n, m;
			n = static_cast<int>(_e.y / 2);
			m = static_cast<int>(_e.x / 2);
			if (n == i && m == j)
			{
				enm->GetHp(l, 0);
			}
		}
	}

	if(tmpP.size() > 1) EmitFillEffext(tmpP);

	return DirectX::XMFLOAT2(static_cast<float>(_fillScore), static_cast<float>(_fillNum));
}

bool BG::CheckPlayerFillPos(DirectX::XMFLOAT3 p)
{
	DirectX::XMFLOAT2 _p = DirectX::XMFLOAT2(p.x + 47.0f, p.z + 47.0f);
	int i = static_cast<int>(_p.y / 2);
	int j = static_cast<int>(_p.x / 2);

	return luminans[j][i].x >= Constant::maxLuminans * 2.0f;
}

void BG::EnemySpawnFill(std::shared_ptr<EnemyManager> enm)
{

	for (int k = 0; k < Constant::MAX_ENEMY; k++)
	{
		if (!enm->GetEnemyExist(k) || !enm->GetEnemyNowSpawn(k)) continue;
		DirectX::XMFLOAT2 _p = DirectX::XMFLOAT2(enm->GetEnemyInfo(k).pos.x + 47.0f, enm->GetEnemyInfo(k).pos.z + 47.0f);
		int i, j;
		i = static_cast<int>(_p.y / 2);
		j = static_cast<int>(_p.x / 2);

		luminans[j][i] = DirectX::XMFLOAT4(Constant::minLuminans, Constant::minLuminans, Constant::minLuminans, 1.0f);

	}

}

void BG::FillEffectUpdate(float elapsedTime)
{
	for (auto& s : fillEffect)
	{
		if (!s.isExist) continue;

		float e = Easing::OutSine(s.time, s.maxTime, 20.0f, -1.0f);
		s.pos.y = e;
		s.scalar = Easing::OutSine(s.time, s.maxTime, 0.0f, 1.0f);

		s.time += elapsedTime;

		if (s.time > s.maxTime)
		{
			s.Clear();
		}
	}
}

void BG::EmitFillEffext(std::vector<DirectX::XMFLOAT3> p)
{
	int i = 0;
	for (auto&s : fillEffect)
	{
		if (s.isExist) continue;
		s.pos = p.at(i);
		s.pos.z *= -1.0f;
		s.time = 0.0f;
		s.scalar = 1.0f;
		s.maxTime = 1.0f;
		s.isExist = true;
		i++;
		if (i > p.size() - 1) break;

	}

}

void BG::UltimateEffectAndCalcFill(std::shared_ptr<Player> p)
{
	if (p->GetEnemyAllKillFlg())
	{
		for (int i = 0; i < Constant::MAP_HEIGHT; i++)
		{
			for (int j = 0; j < Constant::MAP_WIDTH; j++)
			{
				if (luminans[j][i].x >= Constant::maxLuminans)
				{
					luminans[j][i] = DirectX::XMFLOAT4(Constant::minLuminans, Constant::minLuminans, Constant::minLuminans, 1.0f);
					ultimateFillDestroy++;
				}
			}
		}
		ParticleSystem::GetInstance()->floorParticle->Burst(100000, { 0.0f, 0.0f, 0.0f }, pFrameWork.GetElapsedTime(), 0, 0);
	}
}

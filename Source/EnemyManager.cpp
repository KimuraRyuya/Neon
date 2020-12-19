#include "../Library/Camera.h"
#include "../Library/FbxLoader.h"
#include "../Library/FrameWork.h"
#include "../Library/CommonInclude.h"
#include "../Library/Mapper.h"
#include "ParticleSystem.h"
#include "Sound.h"
#include "EnemyManager.h"
#include "Constant.h"

void EnemyManager::Initialize()
{
	enemyBuffer = std::make_unique<GPUParticleBuffers::GPUBuffer>();

	// Enemy Character Model Load
	{
		const char* fbx_filename = "Data/Enemy/enemyRef.fbx";
		std::unique_ptr<ModelData> model_data = std::make_unique<ModelData>();
		FbxLoader fbx_loader;
		fbx_loader.Load(fbx_filename, *model_data);

		modelResource = std::make_shared<ModelResource>(pFrameWork.GetDevice(), std::move(model_data), true);
		model[0] = std::make_shared<Model>(modelResource);
	}
	{
		const char* fbx_filename = "Data/Enemy/enemyLine.fbx";
		std::unique_ptr<ModelData> model_data = std::make_unique<ModelData>();
		FbxLoader fbx_loader;
		fbx_loader.Load(fbx_filename, *model_data);

		modelResource = std::make_shared<ModelResource>(pFrameWork.GetDevice(), std::move(model_data), true);
		model[1] = std::make_shared<Model>(modelResource);
	}
	{
		const char* fbx_filename = "Data/Enemy/enemyChase.fbx";
		std::unique_ptr<ModelData> model_data = std::make_unique<ModelData>();
		FbxLoader fbx_loader;
		fbx_loader.Load(fbx_filename, *model_data);

		modelResource = std::make_shared<ModelResource>(pFrameWork.GetDevice(), std::move(model_data), true);
		model[2] = std::make_shared<Model>(modelResource);
	}
	{
		const char* fbx_filename = "Data/Enemy/Spawn/spawnEnemyRef.fbx";
		std::unique_ptr<ModelData> model_data = std::make_unique<ModelData>();
		FbxLoader fbx_loader;
		fbx_loader.Load(fbx_filename, *model_data);

		modelResource = std::make_shared<ModelResource>(pFrameWork.GetDevice(), std::move(model_data), true);
		spawnModel[0] = std::make_shared<Model>(modelResource);
	}
	{
		const char* fbx_filename = "Data/Enemy/Spawn/spawnEnemyLine.fbx";
		std::unique_ptr<ModelData> model_data = std::make_unique<ModelData>();
		FbxLoader fbx_loader;
		fbx_loader.Load(fbx_filename, *model_data);

		modelResource = std::make_shared<ModelResource>(pFrameWork.GetDevice(), std::move(model_data), true);
		spawnModel[1] = std::make_shared<Model>(modelResource);
	}
	{
		const char* fbx_filename = "Data/Enemy/Spawn/spawnEnemyChase.fbx";
		std::unique_ptr<ModelData> model_data = std::make_unique<ModelData>();
		FbxLoader fbx_loader;
		fbx_loader.Load(fbx_filename, *model_data);

		modelResource = std::make_shared<ModelResource>(pFrameWork.GetDevice(), std::move(model_data), true);
		spawnModel[2] = std::make_shared<Model>(modelResource);
	}

	for (int i = 0; i < Constant::MAX_ENEMY; i++)
	{
		enemy[i].Initialize(enemyBufferInfo[i]);
	}

	popTime = 0;
	popElapsedTime = 0;
	enemyExistNum = 0;
	enemyPopNum = 0;
	canNextEnemyPop = true;
	actionType = Enemy::ActionType::Chase;
	modelRenderer = std::make_unique<ModelRenderer>(pFrameWork.GetDevice());

	isWarning = false;


}

void EnemyManager::Reset()
{
	for (int i = 0; i < Constant::MAX_ENEMY; i++)
	{
		enemy[i].Initialize(enemyBufferInfo[i]);
	}

	popTime = 0;
	popElapsedTime = 0;
	enemyExistNum = 0;
	enemyPopNum = 0;
	actionType = Enemy::ActionType::Chase;
	canNextEnemyPop = true;
	isWarning = false;
}

void EnemyManager::Update(int lev, float elapsedTime)
{
	for (int i = 0; i < Constant::MAX_ENEMY; i++)
	{
		if (!enemy[i].isExist) continue;

		// ターゲットにプレイヤーを指定して更新
		enemy[i].target = targetPos;
		enemy[i].Move(enemyBufferInfo[i], elapsedTime);
	}

	// 時間経過でスポーンさせる
	if (popTime > popElapsedTime)
	{
		// 行動タイプをランダムにさせる
		ActionSelect(actionType, lev);
		if (actionType == 6 && lev == 3)
		{
			isWarning = true;
		}
		popTime = 0;
		canNextEnemyPop = true;
	}
	popTime += elapsedTime;

	//EnemyInfoUpdate();

#ifdef _DEBUG
	ImGui::Begin("Enemy");

	ImGui::Text("Enemy##time : %f", popTime);

	ImGui::End();
#endif // _DEBUG

}

void EnemyManager::Render()
{
	modelRenderer->Begin(pFrameWork.GetContext());
	for (int i = 0; i < Constant::MAX_ENEMY; i++)
	{
		if (!enemy[i].isExist) continue;
		enemy[i].GiveModelInfo(enemyBufferInfo[i]);
		modelRenderer->Draw(pFrameWork.GetContext(), *enemy[i].model, { 100.0f, 100.0f, 100.0f, 1.0f });
	}
	modelRenderer->End(pFrameWork.GetContext());

}

void EnemyManager::EnemyExistJudge(float elapsedTime)
{
	enemyExistNum = 0;
	for (int i = 0; i < Constant::MAX_ENEMY; i++)
	{
		if (!enemy[i].isExist) continue;

		// 存在していてHPが0ならParticleを出す
		if (enemyBufferInfo[i].hp <= 0)
		{
			Sound::GetInstance()->enemyDestory->PlayWAV();
			Sound::GetInstance()->enemyDestory->SetVolume(0.1f);
			if (enemy[i].colorType == Enemy::ColorType::Red)
			{
				ParticleSystem::GetInstance()->destroyEmitParticle[1]->Burst(300, enemyBufferInfo[i].pos, elapsedTime, 0, 0);
				ParticleSystem::GetInstance()->destroyEmitParticle[0]->Burst(500, enemyBufferInfo[i].pos, elapsedTime, 0, 0);
			}
			else if (enemy[i].colorType == Enemy::ColorType::Purple)
			{
				ParticleSystem::GetInstance()->destroyEmitParticle[3]->Burst(300, enemyBufferInfo[i].pos, elapsedTime, 0, 0);
				ParticleSystem::GetInstance()->destroyEmitParticle[2]->Burst(300, enemyBufferInfo[i].pos, elapsedTime, 0, 0);
			}
			else
			{
				ParticleSystem::GetInstance()->destroyEmitParticle[5]->Burst(500, enemyBufferInfo[i].pos, elapsedTime, 0, 0);
				ParticleSystem::GetInstance()->destroyEmitParticle[4]->Burst(500, enemyBufferInfo[i].pos, elapsedTime, 0, 0);
			}

			enemy[i].isExist = false;
			enemy[i].hasNowSpawn = false;
		}
		else
		{
			enemyExistNum++;
		}
	}
}

void EnemyManager::ActionSelect(int type, int lev)
{
	int popNum = 0;
	int loopCnt = 0;
	float rad = 0.0f;
	int hei = 0;
	int wid = 0;
	DirectX::XMFLOAT2 enmPosConvertMapChip;
	switch (type)
	{
	case Enemy::ActionType::Chase:
	{
		switch (lev)
		{
		case 1:
			for (int i = 0; i < Constant::MAX_ENEMY; i++)
			{
				if (enemy[i].isExist) continue;
				enemy[i].ChasePop(enemyBufferInfo[i], Enemy::ActionType::Chase, lev, model[2], spawnModel[2]);

				enmPosConvertMapChip = { enemyBufferInfo[i].pos.x + 47.0f, enemyBufferInfo[i].pos.z + 47.0f };
				hei = static_cast<int>(enmPosConvertMapChip.x / 2.0f);
				wid = static_cast<int>(enmPosConvertMapChip.y / 2.0f);
				// 塗られてるところ以外を探しす
				if (bgLuminans[wid * Constant::MAP_WIDTH + hei].x < Constant::maxLuminans)
				{
					popNum++;
				}
				else
				{
					enemy[i].EnemyInfClear(enemyBufferInfo[i]);
				}
				loopCnt++;
				if (loopCnt > 30) break;
				if (popNum > enemyPopNum) break;
			}
			break;

		case 2:
			for (int i = 0; i < Constant::MAX_ENEMY; i++)
			{
				if (enemy[i].isExist) continue;
				enemy[i].ChasePop(enemyBufferInfo[i], Enemy::ActionType::Chase, lev, model[2], spawnModel[2]);
				enmPosConvertMapChip = { enemyBufferInfo[i].pos.x + 47.0f, enemyBufferInfo[i].pos.z + 47.0f };
				hei = static_cast<int>(enmPosConvertMapChip.x / 2.0f);
				wid = static_cast<int>(enmPosConvertMapChip.y / 2.0f);
				// 塗られてるところ以外を探しす
				if (bgLuminans[wid * Constant::MAP_WIDTH + hei].x < Constant::maxLuminans)
				{
					popNum++;
				}
				else
				{
					enemy[i].EnemyInfClear(enemyBufferInfo[i]);
				}
				loopCnt++;
				if (loopCnt > 30) break;
				if (popNum > enemyPopNum) break;
			}
			break;

		case 3:
			for (int i = 0; i < Constant::MAX_ENEMY; i++)
			{
				if (enemy[i].isExist) continue;
				enemy[i].ChasePop(enemyBufferInfo[i], Enemy::ActionType::Chase, lev, model[2], spawnModel[2]);
				enmPosConvertMapChip = { enemyBufferInfo[i].pos.x + 47.0f, enemyBufferInfo[i].pos.z + 47.0f };
				hei = static_cast<int>(enmPosConvertMapChip.x / 2.0f);
				wid = static_cast<int>(enmPosConvertMapChip.y / 2.0f);
				// 塗られてるところ以外を探しす
				if (bgLuminans[wid * Constant::MAP_WIDTH + hei].x < Constant::maxLuminans)
				{
					popNum++;
				}
				else
				{
					enemy[i].EnemyInfClear(enemyBufferInfo[i]);
				}
				loopCnt++;
				if (loopCnt > 30) break;
				if (popNum > enemyPopNum) break;
			}
			break;
		}

		break;
	}

	case Enemy::ActionType::Top_to_Bottom_Liner:
	{
		switch (lev)
		{
		case 1:
		{
			rad = 360 / 8.0f * 0.01745f;
			for (int i = 0; i < Constant::MAX_ENEMY; i++)
			{
				if (enemy[i].isExist) continue;

				float x = sinf(rad * (popNum + 7)) * 34.0f;
				float z = cosf(rad * (popNum + 7)) * 34.0f;
				DirectX::XMFLOAT3 p = { x, 0, z };
				enemy[i].LinearPop(enemyBufferInfo[i], p, lev, Enemy::ActionType::Top_to_Bottom_Liner, model[1], spawnModel[1]);
				popNum++;

				if (popNum > 2) break;
			}
		}
		case 2:
		{
			rad = 360 / 16.0f * 0.01745f;
			int angNum = 13;
			for (int i = 0; i < Constant::MAX_ENEMY; i++)
			{
				if (enemy[i].isExist) continue;

				float x = sinf(rad * (angNum)) * 34.0f;
				float z = cosf(rad * (angNum)) * 34.0f;
				DirectX::XMFLOAT3 p = { x, 0, z };
				enemy[i].LinearPop(enemyBufferInfo[i], p, lev, Enemy::ActionType::Top_to_Bottom_Liner, model[1], spawnModel[1]);
				popNum++;
				angNum += 2;
				if (popNum > 3) break;
			}
		}
		case 3:
		{
			rad = 360 / 16.0f * 0.01745f;
			int angNum = 13;
			for (int i = 0; i < Constant::MAX_ENEMY; i++)
			{
				if (enemy[i].isExist) continue;

				float x = sinf(rad * (angNum)) * 34.0f;
				float z = cosf(rad * (angNum)) * 34.0f;
				DirectX::XMFLOAT3 p = { x, 0, z };
				enemy[i].LinearPop(enemyBufferInfo[i], p, lev, Enemy::ActionType::Top_to_Bottom_Liner, model[1], spawnModel[1]);
				popNum++;
				angNum++;
				if (angNum == 16) angNum++;
				if (popNum > 5) break;
			}
		}
		break;
		}
	}
	case Enemy::ActionType::Bottom_to_Top_Liner:
	{
		switch (lev)
		{
		case 1:
			rad = 360 / 8.0f * 0.01745f;
			for (int i = 0; i < Constant::MAX_ENEMY; i++)
			{
				if (enemy[i].isExist) continue;

				float x = sinf(rad * (popNum + 3)) * 34.0f;
				float z = cosf(rad * (popNum + 3)) * 34.0f;
				DirectX::XMFLOAT3 p = { x, 0, z };
				enemy[i].LinearPop(enemyBufferInfo[i], p, lev, Enemy::ActionType::Bottom_to_Top_Liner, model[1], spawnModel[1]);
				popNum++;

				if (popNum > 2) break;
			}
		case 2:
		{
			rad = 360 / 16.0f * 0.01745f;
			int angNum = 5;
			for (int i = 0; i < Constant::MAX_ENEMY; i++)
			{
				if (enemy[i].isExist) continue;

				float x = sinf(rad * (angNum)) * 34.0f;
				float z = cosf(rad * (angNum)) * 34.0f;
				DirectX::XMFLOAT3 p = { x, 0, z };
				enemy[i].LinearPop(enemyBufferInfo[i], p, lev, Enemy::ActionType::Bottom_to_Top_Liner, model[1], spawnModel[1]);
				popNum++;
				angNum += 2;
				if (popNum > 3) break;
			}
		}
		case 3:
		{
			rad = 360 / 16.0f * 0.01745f;
			int angNum = 5;
			for (int i = 0; i < Constant::MAX_ENEMY; i++)
			{
				if (enemy[i].isExist) continue;

				float x = sinf(rad * (angNum)) * 34.0f;
				float z = cosf(rad * (angNum)) * 34.0f;
				DirectX::XMFLOAT3 p = { x, 0, z };
				enemy[i].LinearPop(enemyBufferInfo[i], p, lev, Enemy::ActionType::Bottom_to_Top_Liner, model[1], spawnModel[1]);
				popNum++;
				angNum++;
				if (angNum == 8) angNum++;
				if (popNum > 5) break;
			}
		}
		break;
		}
	}
	case Enemy::ActionType::Right_to_LeftLiner:
	{
		switch (lev)
		{
		case 1:
			rad = 360 / 8.0f * 0.01745f;
			for (int i = 0; i < Constant::MAX_ENEMY; i++)
			{
				if (enemy[i].isExist) continue;

				float x = sinf(rad * (popNum + 1)) * 34.0f;
				float z = cosf(rad * (popNum + 1)) * 34.0f;
				DirectX::XMFLOAT3 p = { x, 0, z };
				enemy[i].LinearPop(enemyBufferInfo[i], p, lev, Enemy::ActionType::Right_to_LeftLiner, model[1], spawnModel[1]);
				popNum++;

				if (popNum > 2) break;
			}
		case 2:
		{
			rad = 360 / 16.0f * 0.01745f;
			int angNum = 1;
			for (int i = 0; i < Constant::MAX_ENEMY; i++)
			{
				if (enemy[i].isExist) continue;

				float x = sinf(rad * (angNum)) * 34.0f;
				float z = cosf(rad * (angNum)) * 34.0f;
				DirectX::XMFLOAT3 p = { x, 0, z };
				enemy[i].LinearPop(enemyBufferInfo[i], p, lev, Enemy::ActionType::Right_to_LeftLiner, model[1], spawnModel[1]);
				popNum++;
				angNum += 2;
				if (popNum > 3) break;
			}
		}
		case 3:
		{
			rad = 360 / 16.0f * 0.01745f;
			int angNum = 1;
			for (int i = 0; i < Constant::MAX_ENEMY; i++)
			{
				if (enemy[i].isExist) continue;

				float x = sinf(rad * (angNum)) * 34.0f;
				float z = cosf(rad * (angNum)) * 34.0f;
				DirectX::XMFLOAT3 p = { x, 0, z };
				enemy[i].LinearPop(enemyBufferInfo[i], p, lev, Enemy::ActionType::Right_to_LeftLiner, model[1], spawnModel[1]);
				popNum++;
				angNum++;
				if (angNum == 4) angNum++;
				if (popNum > 5) break;
			}
		}
		break;
		}
	}
	case Enemy::ActionType::Left_to_Right_Liner:
	{
		switch (lev)
		{
		case 1:
			rad = 360 / 8.0f * 0.01745f;
			for (int i = 0; i < Constant::MAX_ENEMY; i++)
			{
				if (enemy[i].isExist) continue;

				float x = sinf(rad * (popNum + 5)) * 34.0f;
				float z = cosf(rad * (popNum + 5)) * 34.0f;
				DirectX::XMFLOAT3 p = { x, 0, z };
				enemy[i].LinearPop(enemyBufferInfo[i], p, lev, Enemy::ActionType::Left_to_Right_Liner, model[1], spawnModel[1]);
				popNum++;

				if (popNum > 2) break;
			}
		case 2:
		{
			rad = 360 / 16.0f * 0.01745f;
			int angNum = 9;
			for (int i = 0; i < Constant::MAX_ENEMY; i++)
			{
				if (enemy[i].isExist) continue;

				float x = sinf(rad * (angNum)) * 34.0f;
				float z = cosf(rad * (angNum)) * 34.0f;
				DirectX::XMFLOAT3 p = { x, 0, z };
				enemy[i].LinearPop(enemyBufferInfo[i], p, lev, Enemy::ActionType::Left_to_Right_Liner, model[1], spawnModel[1]);
				popNum++;
				angNum += 2;
				if (popNum > 3) break;
			}
		}
		case 3:
		{
			rad = 360 / 16.0f * 0.01745f;
			int angNum = 9;
			for (int i = 0; i < Constant::MAX_ENEMY; i++)
			{
				if (enemy[i].isExist) continue;

				float x = sinf(rad * (angNum)) * 34.0f;
				float z = cosf(rad * (angNum)) * 34.0f;
				DirectX::XMFLOAT3 p = { x, 0, z };
				enemy[i].LinearPop(enemyBufferInfo[i], p, lev, Enemy::ActionType::Left_to_Right_Liner, model[1], spawnModel[1]);
				popNum++;
				angNum++;
				if (angNum == 12) angNum++;
				if (popNum > 5) break;
			}
		}
		break;
		}
	}

	case Enemy::ActionType::WARNING:
	{
		switch (lev)
		{
		case 1:
		{
			rad = 360 / 4.0f * 0.01745f;
			for (int i = 0; i < Constant::MAX_ENEMY; i++)
			{
				if (enemy[i].isExist) continue;

				float x = sinf(rad * i) * 34.0f;
				float z = cosf(rad * i) * 34.0f;
				DirectX::XMFLOAT3 p = { x, 0, z };
				enemy[i].LinearPop(enemyBufferInfo[i], p, lev, Enemy::ActionType::WARNING, model[1], spawnModel[1]);
				popNum++;

				if (popNum > 3) break;
			}
			break;
		}
		case 2:
		{
			rad = 360 / 10.0f * 0.01745f;
			for (int i = 0; i < Constant::MAX_ENEMY; i++)
			{
				if (enemy[i].isExist) continue;

				float x = sinf(rad * i) * 34.0f;
				float z = cosf(rad * i) * 34.0f;
				DirectX::XMFLOAT3 p = { x, 0, z };
				enemy[i].LinearPop(enemyBufferInfo[i], p, lev, Enemy::ActionType::WARNING, model[1], spawnModel[1]);
				popNum++;

				if (popNum > 9) break;
			}
			break;
		}
		case 3:
			rad = 360 / 36.0f * 0.01745f;
			for (int i = 0; i < Constant::MAX_ENEMY; i++)
			{
				if (enemy[i].isExist) continue;

				float x = sinf(rad * i) * 34.0f;
				float z = cosf(rad * i) * 34.0f;
				DirectX::XMFLOAT3 p = { x, 0, z };
				enemy[i].LinearPop(enemyBufferInfo[i], p, lev, Enemy::ActionType::WARNING, model[1], spawnModel[1]);
				popNum++;

				if (popNum > 35) break;
			}
			break;
		}
		break;
	}

	case Enemy::ActionType::Reflection:
	{

		switch (lev)
		{
		case 1:
			for (int i = 0; i < Constant::MAX_ENEMY; i++)
			{
				if (enemy[i].isExist) continue;
				enemy[i].ReflexPop(enemyBufferInfo[i], Enemy::ActionType::Reflection, lev, model[0], spawnModel[0]);
				enmPosConvertMapChip = { enemyBufferInfo[i].pos.x + 47.0f, enemyBufferInfo[i].pos.z + 47.0f };
				hei = static_cast<int>(enmPosConvertMapChip.x / 2.0f);
				wid = static_cast<int>(enmPosConvertMapChip.y / 2.0f);
				if (bgLuminans[wid * Constant::MAP_WIDTH + hei].x < Constant::maxLuminans)
				{
					popNum++;
				}
				else
				{
					enemy[i].EnemyInfClear(enemyBufferInfo[i]);
				}
				loopCnt++;
				if (loopCnt > 30) break;
				if (popNum > enemyPopNum) break;
			}
			break;
		case 2:
			for (int i = 0; i < Constant::MAX_ENEMY; i++)
			{
				if (enemy[i].isExist) continue;
				enemy[i].ReflexPop(enemyBufferInfo[i], Enemy::ActionType::Reflection, lev, model[0], spawnModel[0]);
				enmPosConvertMapChip = { enemyBufferInfo[i].pos.x + 47.0f, enemyBufferInfo[i].pos.z + 47.0f };
				hei = static_cast<int>(enmPosConvertMapChip.x / 2.0f);
				wid = static_cast<int>(enmPosConvertMapChip.y / 2.0f);
				if (bgLuminans[wid * Constant::MAP_WIDTH + hei].x < Constant::maxLuminans)
				{
					popNum++;
				}
				else
				{
					enemy[i].EnemyInfClear(enemyBufferInfo[i]);
				}
				loopCnt++;
				if (loopCnt > 30) break;
				if (popNum > enemyPopNum) break;
			}
			break;
		case 3:
			for (int i = 0; i < Constant::MAX_ENEMY; i++)
			{
				if (enemy[i].isExist) continue;
				enemy[i].ReflexPop(enemyBufferInfo[i], Enemy::ActionType::Reflection, lev, model[0], spawnModel[0]);
				enmPosConvertMapChip = { enemyBufferInfo[i].pos.x + 47.0f, enemyBufferInfo[i].pos.z + 47.0f };
				hei = static_cast<int>(enmPosConvertMapChip.x / 2.0f);
				wid = static_cast<int>(enmPosConvertMapChip.y / 2.0f);
				if (bgLuminans[wid * Constant::MAP_WIDTH + hei].x < Constant::maxLuminans)
				{
					popNum++;
				}
				else
				{
					enemy[i].EnemyInfClear(enemyBufferInfo[i]);
				}
				loopCnt++;
				if (loopCnt > 30) break;
				if (popNum > enemyPopNum) break;
			}
			break;
		}
		break;
	}
	}
}

void EnemyManager::Finalize()
{
}

void EnemyManager::AllDown()
{
	for (int i = 0; i < Constant::MAX_ENEMY; i++)
	{
		if (!enemy[i].isExist) continue;
		enemy[i].isExist = false;
		for (int j = 0; j < 3; j++)
		{
			ParticleSystem::GetInstance()->destroyEmitParticle[1 + (2 * j)]->Burst(40, enemyBufferInfo[i].pos, pFrameWork.GetElapsedTime(), 0, 0);
			ParticleSystem::GetInstance()->destroyEmitParticle[0 + (2 * j)]->Burst(100, enemyBufferInfo[i].pos, pFrameWork.GetElapsedTime(), 0, 0);
		}
	}
}

void EnemyManager::SetNextEnemyParamerter(Enemy::ActionType type, int pN, int pT)
{
	if (!canNextEnemyPop) return;
	popElapsedTime = pT;
	enemyPopNum = pN;
	actionType = type;
	canNextEnemyPop = false;
}

void EnemyManager::SetBgLuminans(DirectX::XMFLOAT4* lum)
{
	bgLuminans = lum;
}


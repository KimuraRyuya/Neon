#include "CollisionJudge.h"
#include "Collision.h"
#include "ParticleSystem.h"
#include "Sound.h"
#include "../Library/Framework.h"
#include "../Library/Camera.h"
#include "../Library/MyLib.h"

namespace CollisionJudge
{

	bool PlayerVSStage(std::shared_ptr<Player> player, std::shared_ptr<BG> bg)
	{
		// プレイヤーの座標とスケールを取得
		DirectX::XMFLOAT2 playerPos = { player->GetPlayerCollision()->GetPos().x, player->GetPlayerCollision()->GetPos().z };
		float playerScale = player->GetPlayerCollision()->GetScale().x;

		// bgの座標は固定でサイズも固定だからマジックナンバー (よくないので直します)
		DirectX::XMFLOAT2 bgPos = { 0.0f, 0.0f };
		float bgScale = 44 * 0.8f;
		// 2次元の円の当たり判定
		if (Collision::CircleVsCircleAndExtrusion(playerPos, playerScale, bgPos, bgScale))
		{
			// 衝突していたら何もしない
			return true;
		}
		// 衝突してたら円の中に押し戻す
		Collision::CircleExtrusion(playerPos, playerScale, bgPos, 44 * 0.8f);
		player->SetPos({ playerPos.x, player->GetPos().y, playerPos.y });
		return false;
	}

	bool PlayerShotVSStage(PlayerShotSystem* playerShot, std::shared_ptr<BG> bg, CharacterSystem* characterSystem)
	{
		// bgの座標は固定でサイズも固定だからマジックナンバー (よくないので直します)
		DirectX::XMFLOAT2 bgPos = { 0.0f, 0.0f };
		float bgScale = 46 * 0.8f;
		
		// ショットの数for文
		for (int i = 0; i < Constant::SHOT_MAX; i++)
		{
			// 存在してなかったら continue
			if (!playerShot->GetExist(i))continue;

			// ショットの座標とスケールを取得
			DirectX::XMFLOAT2 shotPos = { playerShot->GetPos(i).x, playerShot->GetPos(i).z };
			float shotScale = 0.5f;

			// 2次元の円の当たり判定
			if (!Collision::CircleVsCircleAndExtrusion(shotPos, shotScale, bgPos, bgScale))
			{
				// 衝突してたら当たった地点からショットのベクトルを作る
				DirectX::XMVECTOR dirVec = DirectX::XMLoadFloat3(&playerShot->GetVelocity(i));
				dirVec = DirectX::XMVector3Normalize(-dirVec);
				DirectX::XMFLOAT3 dir = {};
				DirectX::XMStoreFloat3(&dir, dirVec);
				//ParticleSystem::GetInstance()->playerShotDestroyParticle->vel = dir;
				DirectX::XMFLOAT3 _emtPos =
				{
					playerShot->GetPos(i).x + (dir.x *	5.0f),
					playerShot->GetPos(i).y + 15.0f,
					playerShot->GetPos(i).z + (dir.z * 5.0f)
				};
				// 上記のベクトルを使用してParticleを出す
				if (characterSystem->GetPlayerAddress()->GetReinforce()) ParticleSystem::GetInstance()->playerShotDestroyParticle[1]->Burst(10, _emtPos, pFrameWork.GetElapsedTime(), 0, 0);
				else ParticleSystem::GetInstance()->playerShotDestroyParticle[0]->Burst(10, _emtPos, pFrameWork.GetElapsedTime(), 0, 0);

				// ショットは消す
				playerShot->SetExist(false, i);
			}
		}
		return false;
	}

	bool PlayerShotVSEnemy(PlayerShotSystem* playerShot, CharacterSystem* characterSystem, Combo* combo, Score* score, std::shared_ptr<BG> bg, TrackingScore* trackingScore, bool reinforce)
	{
		// ショットの数for文
		for (int i = 0; i < Constant::SHOT_MAX; i++)
		{
			//存在していなかったら continue
			if (!playerShot->GetExist(i)) continue;
			// ショットの座標とサイズ取得
			DirectX::XMFLOAT2 shotPos = { playerShot->GetPos(i).x, playerShot->GetPos(i).z };
			float shotScale = 0.5f * playerShot->GetScale(i);

			// エネミーの数分for文
			for (int j = 0; j < Constant::MAX_ENEMY; j++)
			{
				// 存在してなかったら continue
				if (!characterSystem->GetEnemyManagerAddress()->GetEnemyExist(j)) continue;
				// エネミーの座標とサイズ取得
				DirectX::XMFLOAT2 enmPos = { characterSystem->GetEnemyManagerAddress()->GetEnemyInfo(j).pos.x, characterSystem->GetEnemyManagerAddress()->GetEnemyInfo(j).pos.z };
				float enmScale = characterSystem->GetEnemyManagerAddress()->GetEnemyInfo(j).size.x * characterSystem->GetEnemyManagerAddress()->GetEnemyInfo(j).scale.x;

				// 2次元の円の当たり判定
				if (Collision::CircleVsCircleAndExtrusion(shotPos, shotScale, enmPos, enmScale) && characterSystem->GetEnemyManagerAddress()->GetEnemyInfo(j).hp > 0)
				{
					// 衝突していたらショットからParticleを出す
					if(characterSystem->GetPlayerAddress()->GetReinforce()) ParticleSystem::GetInstance()->playerShotDestroyParticle[1]->Burst(10, playerShot->GetPos(i), pFrameWork.GetElapsedTime(), 0, 0);
					else ParticleSystem::GetInstance()->playerShotDestroyParticle[0]->Burst(10, playerShot->GetPos(i), pFrameWork.GetElapsedTime(), 0, 0);

					// ショットを消す
					playerShot->SetExist(false, i);
					// エネミーのHP取得
					float hp = characterSystem->GetEnemyManagerAddress()->GetEnemyInfo(j).hp;

					// バフされているときとされていないときでif文
					if(reinforce) characterSystem->GetEnemyManagerAddress()->GetHp(j, hp - 40.0f);
					else if(!characterSystem->GetEnemyManagerAddress()->GetEnemyNowSpawn(j)) characterSystem->GetEnemyManagerAddress()->GetHp(j, hp - 1);
					
					// エネミーが死んだらParticleやらなんやらする
					if (characterSystem->GetEnemyManagerAddress()->GetEnemyInfo(j).hp <= 0)
					{
						trackingScore->EmitTrackingScore(characterSystem->GetEnemyManagerAddress()->GetEnemyInfo(j).pos);
						characterSystem->GetPlayerAddress()->AddDushGauge(6.0f);
						bg->SetPlayerBase(characterSystem->GetEnemyManagerAddress()->GetEnemyInfo(j).pos);
						combo->AddCombo();
						score->AddScore(combo->GetNowCombo());
						Camera::GetInstance().OnCameraShake(0.5, 0.015f);
					}
				}
			}
		}
		return false;
	}

	bool PlayerVSTrakingScore(std::shared_ptr<Player> player, TrackingScore* trackingScore)
	{
		// プレイヤーがスポーン中なら continue
		if (player->GetNewSpawn()) return false;

		// プレイヤーの座標とサイズ取得
		DirectX::XMFLOAT2 playerPos = { player->GetPlayerCollision()->GetPos().x, player->GetPlayerCollision()->GetPos().z };
		float playerScale = player->GetPlayerCollision()->GetScale().x;

		// スコアボールの数for文
		for (int i = 0; i < Constant::MAX_TRACKINGSCORE; i++)
		{
			// 存在してなかったら continue
			if (!trackingScore->GetTrakingScoreInf(i).isExist) continue;
			// スコアボールの座標とサイズ取得
			DirectX::XMFLOAT2 trackPos = { trackingScore->GetTrakingScoreInf(i).pos.x, trackingScore->GetTrakingScoreInf(i).pos.z * -1.0f };
			float trackScale = 2.0f;
			// 2次元の円の当たり判定
			if (Collision::CircleVsCircleAndExtrusion(playerPos, playerScale, trackPos, trackScale, true))
			{
				// スコアボールを消す
				trackingScore->Destory(i);
			}
		}
		return false;
	}

	bool PlayerVSEnemy(std::shared_ptr<Player> player, std::shared_ptr<EnemyManager> enmManager, Combo* combo)
	{
		// プレイヤーがスポーン中なら continue
		if (player->GetNewSpawn()) return false;

		// プレイヤーの座標とサイズを取得
		DirectX::XMFLOAT2 playerPos = { player->GetPlayerCollision()->GetPos().x, player->GetPlayerCollision()->GetPos().z };
		float playerScale = player->GetPlayerCollision()->GetScale().x;

		// エネミーの数for文
		for (int i = 0; i < Constant::MAX_ENEMY; i++)
		{
			// エネミーのスポーン中または存在してなかったら continue
			if (enmManager->GetEnemyNowSpawn(i)) continue;
			if (!enmManager->GetEnemyExist(i)) continue;
			// エネミーの座標とサイズ取得
			DirectX::XMFLOAT2 enmPos = { enmManager->GetEnemyInfo(i).pos.x, enmManager->GetEnemyInfo(i).pos.z };
			float enmScale = enmManager->GetEnemyInfo(i).size.x * enmManager->GetEnemyInfo(i).scale.x;
			// 2次元の円の当たり判定
			if (Collision::CircleVsCircleAndExtrusion(playerPos, playerScale, enmPos, enmScale, true))
			{
				//衝突したらParticeと出したりする
				ParticleSystem::GetInstance()->playerDestroyParticle->Burst(10000, player->GetPos(), pFrameWork.GetElapsedTime(), player->GetModelResource(), 0.0f, 0.0f);
				Camera::GetInstance().OnCameraShake(0.5f, 0.1f);
				player->SubHp();
				combo->ComboReset();
				StartVibration(30000);
				break;
			}
		}
		return false;
	}

	bool PlayerDestroyEffectVSEnemy(std::shared_ptr<Player> player, std::shared_ptr<EnemyManager> enmManager)
	{
		if (!player->GetNewSpawn()) return false;
		DirectX::XMFLOAT2 playerDestoryEffectPos = { player->GetPlayerDestoryEffectPos().x, player->GetPlayerDestoryEffectPos().z };
		float playerDestoryEffectScale = player->GetPlayerDestoryEffectScale();

		for (int i = 0; i < Constant::MAX_ENEMY; i++)
		{
			if (!enmManager->GetEnemyExist(i)) continue;
			DirectX::XMFLOAT2 enmPos = { enmManager->GetEnemyInfo(i).pos.x, enmManager->GetEnemyInfo(i).pos.z };
			float enmScale = enmManager->GetEnemyInfo(i).size.x * enmManager->GetEnemyInfo(i).scale.x;
			if (Collision::CircleVsCircleAndExtrusion(playerDestoryEffectPos, playerDestoryEffectScale, enmPos, enmScale, true))
			{
				ParticleSystem::GetInstance()->destroyEmitParticle[1]->Burst(300, enmManager->GetEnemyInfo(i).pos, pFrameWork.GetElapsedTime(), 0, 0);
				ParticleSystem::GetInstance()->destroyEmitParticle[0]->Burst(500, enmManager->GetEnemyInfo(i).pos, pFrameWork.GetElapsedTime(), 0, 0);
				ParticleSystem::GetInstance()->destroyEmitParticle[3]->Burst(300, enmManager->GetEnemyInfo(i).pos, pFrameWork.GetElapsedTime(), 0, 0);
				ParticleSystem::GetInstance()->destroyEmitParticle[2]->Burst(300, enmManager->GetEnemyInfo(i).pos, pFrameWork.GetElapsedTime(), 0, 0);
				ParticleSystem::GetInstance()->destroyEmitParticle[5]->Burst(500, enmManager->GetEnemyInfo(i).pos, pFrameWork.GetElapsedTime(), 0, 0);
				ParticleSystem::GetInstance()->destroyEmitParticle[4]->Burst(500, enmManager->GetEnemyInfo(i).pos, pFrameWork.GetElapsedTime(), 0, 0);
				enmManager->GetHp(i, 0);
			}
		}
		return false;
	}

}

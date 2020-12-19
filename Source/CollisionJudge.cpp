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
		// �v���C���[�̍��W�ƃX�P�[�����擾
		DirectX::XMFLOAT2 playerPos = { player->GetPlayerCollision()->GetPos().x, player->GetPlayerCollision()->GetPos().z };
		float playerScale = player->GetPlayerCollision()->GetScale().x;

		// bg�̍��W�͌Œ�ŃT�C�Y���Œ肾����}�W�b�N�i���o�[ (�悭�Ȃ��̂Œ����܂�)
		DirectX::XMFLOAT2 bgPos = { 0.0f, 0.0f };
		float bgScale = 44 * 0.8f;
		// 2�����̉~�̓����蔻��
		if (Collision::CircleVsCircleAndExtrusion(playerPos, playerScale, bgPos, bgScale))
		{
			// �Փ˂��Ă����牽�����Ȃ�
			return true;
		}
		// �Փ˂��Ă���~�̒��ɉ����߂�
		Collision::CircleExtrusion(playerPos, playerScale, bgPos, 44 * 0.8f);
		player->SetPos({ playerPos.x, player->GetPos().y, playerPos.y });
		return false;
	}

	bool PlayerShotVSStage(PlayerShotSystem* playerShot, std::shared_ptr<BG> bg, CharacterSystem* characterSystem)
	{
		// bg�̍��W�͌Œ�ŃT�C�Y���Œ肾����}�W�b�N�i���o�[ (�悭�Ȃ��̂Œ����܂�)
		DirectX::XMFLOAT2 bgPos = { 0.0f, 0.0f };
		float bgScale = 46 * 0.8f;
		
		// �V���b�g�̐�for��
		for (int i = 0; i < Constant::SHOT_MAX; i++)
		{
			// ���݂��ĂȂ������� continue
			if (!playerShot->GetExist(i))continue;

			// �V���b�g�̍��W�ƃX�P�[�����擾
			DirectX::XMFLOAT2 shotPos = { playerShot->GetPos(i).x, playerShot->GetPos(i).z };
			float shotScale = 0.5f;

			// 2�����̉~�̓����蔻��
			if (!Collision::CircleVsCircleAndExtrusion(shotPos, shotScale, bgPos, bgScale))
			{
				// �Փ˂��Ă��瓖�������n�_����V���b�g�̃x�N�g�������
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
				// ��L�̃x�N�g�����g�p����Particle���o��
				if (characterSystem->GetPlayerAddress()->GetReinforce()) ParticleSystem::GetInstance()->playerShotDestroyParticle[1]->Burst(10, _emtPos, pFrameWork.GetElapsedTime(), 0, 0);
				else ParticleSystem::GetInstance()->playerShotDestroyParticle[0]->Burst(10, _emtPos, pFrameWork.GetElapsedTime(), 0, 0);

				// �V���b�g�͏���
				playerShot->SetExist(false, i);
			}
		}
		return false;
	}

	bool PlayerShotVSEnemy(PlayerShotSystem* playerShot, CharacterSystem* characterSystem, Combo* combo, Score* score, std::shared_ptr<BG> bg, TrackingScore* trackingScore, bool reinforce)
	{
		// �V���b�g�̐�for��
		for (int i = 0; i < Constant::SHOT_MAX; i++)
		{
			//���݂��Ă��Ȃ������� continue
			if (!playerShot->GetExist(i)) continue;
			// �V���b�g�̍��W�ƃT�C�Y�擾
			DirectX::XMFLOAT2 shotPos = { playerShot->GetPos(i).x, playerShot->GetPos(i).z };
			float shotScale = 0.5f * playerShot->GetScale(i);

			// �G�l�~�[�̐���for��
			for (int j = 0; j < Constant::MAX_ENEMY; j++)
			{
				// ���݂��ĂȂ������� continue
				if (!characterSystem->GetEnemyManagerAddress()->GetEnemyExist(j)) continue;
				// �G�l�~�[�̍��W�ƃT�C�Y�擾
				DirectX::XMFLOAT2 enmPos = { characterSystem->GetEnemyManagerAddress()->GetEnemyInfo(j).pos.x, characterSystem->GetEnemyManagerAddress()->GetEnemyInfo(j).pos.z };
				float enmScale = characterSystem->GetEnemyManagerAddress()->GetEnemyInfo(j).size.x * characterSystem->GetEnemyManagerAddress()->GetEnemyInfo(j).scale.x;

				// 2�����̉~�̓����蔻��
				if (Collision::CircleVsCircleAndExtrusion(shotPos, shotScale, enmPos, enmScale) && characterSystem->GetEnemyManagerAddress()->GetEnemyInfo(j).hp > 0)
				{
					// �Փ˂��Ă�����V���b�g����Particle���o��
					if(characterSystem->GetPlayerAddress()->GetReinforce()) ParticleSystem::GetInstance()->playerShotDestroyParticle[1]->Burst(10, playerShot->GetPos(i), pFrameWork.GetElapsedTime(), 0, 0);
					else ParticleSystem::GetInstance()->playerShotDestroyParticle[0]->Burst(10, playerShot->GetPos(i), pFrameWork.GetElapsedTime(), 0, 0);

					// �V���b�g������
					playerShot->SetExist(false, i);
					// �G�l�~�[��HP�擾
					float hp = characterSystem->GetEnemyManagerAddress()->GetEnemyInfo(j).hp;

					// �o�t����Ă���Ƃ��Ƃ���Ă��Ȃ��Ƃ���if��
					if(reinforce) characterSystem->GetEnemyManagerAddress()->GetHp(j, hp - 40.0f);
					else if(!characterSystem->GetEnemyManagerAddress()->GetEnemyNowSpawn(j)) characterSystem->GetEnemyManagerAddress()->GetHp(j, hp - 1);
					
					// �G�l�~�[�����񂾂�Particle���Ȃ��炷��
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
		// �v���C���[���X�|�[�����Ȃ� continue
		if (player->GetNewSpawn()) return false;

		// �v���C���[�̍��W�ƃT�C�Y�擾
		DirectX::XMFLOAT2 playerPos = { player->GetPlayerCollision()->GetPos().x, player->GetPlayerCollision()->GetPos().z };
		float playerScale = player->GetPlayerCollision()->GetScale().x;

		// �X�R�A�{�[���̐�for��
		for (int i = 0; i < Constant::MAX_TRACKINGSCORE; i++)
		{
			// ���݂��ĂȂ������� continue
			if (!trackingScore->GetTrakingScoreInf(i).isExist) continue;
			// �X�R�A�{�[���̍��W�ƃT�C�Y�擾
			DirectX::XMFLOAT2 trackPos = { trackingScore->GetTrakingScoreInf(i).pos.x, trackingScore->GetTrakingScoreInf(i).pos.z * -1.0f };
			float trackScale = 2.0f;
			// 2�����̉~�̓����蔻��
			if (Collision::CircleVsCircleAndExtrusion(playerPos, playerScale, trackPos, trackScale, true))
			{
				// �X�R�A�{�[��������
				trackingScore->Destory(i);
			}
		}
		return false;
	}

	bool PlayerVSEnemy(std::shared_ptr<Player> player, std::shared_ptr<EnemyManager> enmManager, Combo* combo)
	{
		// �v���C���[���X�|�[�����Ȃ� continue
		if (player->GetNewSpawn()) return false;

		// �v���C���[�̍��W�ƃT�C�Y���擾
		DirectX::XMFLOAT2 playerPos = { player->GetPlayerCollision()->GetPos().x, player->GetPlayerCollision()->GetPos().z };
		float playerScale = player->GetPlayerCollision()->GetScale().x;

		// �G�l�~�[�̐�for��
		for (int i = 0; i < Constant::MAX_ENEMY; i++)
		{
			// �G�l�~�[�̃X�|�[�����܂��͑��݂��ĂȂ������� continue
			if (enmManager->GetEnemyNowSpawn(i)) continue;
			if (!enmManager->GetEnemyExist(i)) continue;
			// �G�l�~�[�̍��W�ƃT�C�Y�擾
			DirectX::XMFLOAT2 enmPos = { enmManager->GetEnemyInfo(i).pos.x, enmManager->GetEnemyInfo(i).pos.z };
			float enmScale = enmManager->GetEnemyInfo(i).size.x * enmManager->GetEnemyInfo(i).scale.x;
			// 2�����̉~�̓����蔻��
			if (Collision::CircleVsCircleAndExtrusion(playerPos, playerScale, enmPos, enmScale, true))
			{
				//�Փ˂�����Partice�Əo�����肷��
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

#pragma once
#include "../Library/Model.h"
#include "../Library/ModelRenderer.h"

#include "Player.h"

#include "../Library/MyLib.h"
#include "../Library/Camera.h"
#include "../Library/FbxLoader.h"
#include "../Library/FrameWork.h"

#include "Sound.h"
#include "PlayerShot.h"
#include "ParticleSystem.h"
#include "EditParameter.h"
#include <imgui.h>

#define XBOX


void Player::Initialize()
{

	const char* fbx_filename = "Data/Player/playerCharacter.fbx";
	std::unique_ptr<ModelData> model_data = std::make_unique<ModelData>();
	FbxLoader fbx_loader;
	fbx_loader.Load(fbx_filename, *model_data);

	modelResource = std::make_shared<ModelResource>(pFrameWork.GetDevice(), std::move(model_data), true);
	playerObj.Initialize(modelResource);
	modelRenderer = std::make_unique<ModelRenderer>(pFrameWork.GetDevice());

	// Geometry Collision Set
	playerCollision = std::make_unique<CollisionPrimitive>(1, true, DirectX::XMFLOAT3(1, 1, 1));
	playerCollision->SetColor({ 0, 1, 0, 1 });

	playerDestoryEffect = std::make_unique<PlayerDestoryEffect>();
	playerDestoryEffect->Initialize();

	floorFillEffect = std::make_unique<FloorFillEffect>();
	floorFillEffect->Initialize();

	ultimate = std::make_unique<Ultimate>();
	ultimate->Initialize();

	DirectX::XMMATRIX _rotateMat = DirectX::XMMatrixIdentity();
	DirectX::XMStoreFloat4x4(&rotateMat, _rotateMat);

	quaternion = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);

	accel = 1.0f;
	hp = 3;
	burstElapsedTime = 0.0f;
	vibValue = 0.0f;
	luminansAnimationElapsedTime = 0.0f;
	luminansAnimationAddTime = 1.0f;
	luminansAnimationTime = 1.0f;
	dushGauge = 360.0f;
	hasNewSpawn = true;
	emitSpawnEffect = false;
	newSpawnTime = 0.0f;
	isReinforce = false;
	isVib = false;
	luminans = { 50.0f, 50.0f, 60.0f, 1.0f };
	emitPowerUpEffectElapsedTime = 0.0f;
	hasFillChipDestory = false;
	hasEnemyAllKill = false;
	isBuffShotSound = false;
	isShotSound = false;
}

void Player::Reset()
{
	playerObj.Initialize();
	DirectX::XMMATRIX _rotateMat = DirectX::XMMatrixIdentity();
	DirectX::XMStoreFloat4x4(&rotateMat, _rotateMat);
	quaternion = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	dushGauge = 360.0f;
	playerObj.SetPos({ 0.0f, 0.5f, 0.0f });
	accel = 1.0f;
	hp = 3;
	burstElapsedTime = 0.0f;
	vibValue = 0.0f;
	isReinforce = false;
	hasNewSpawn = true;
	emitSpawnEffect = false;
	hasFillChipDestory = false;
	hasEnemyAllKill = false;
	isVib = false;
	isBuffShotSound = false;
	isShotSound = false;
	newSpawnTime = 0.0f;
	luminansAnimationAddTime = 1.0f;
	luminansAnimationElapsedTime = 0.0f;
	luminansAnimationTime = 1.0f;
	luminans = { 50.0f, 50.0f, 60.0f, 1.0f };
	emitPowerUpEffectElapsedTime = 0.0f;
	playerDestoryEffect->Reset();
	floorFillEffect->Reset();
	ultimate->Reset();
}

void Player::Update(int lev, float elapsedTime)
{
	GetInputXbox();
	GetInputMouse();

	// �A���e�B���b�g�̓���
	ultimate->SetOnUltimateCountDouwn(hasFillChipDestory);
	ultimate->Update(elapsedTime);

	if (ultimate->GetOnUltimate())
	{
		Sound::GetInstance()->enemyAllDestory->PlayWAV();
		Sound::GetInstance()->enemyAllDestory->SetVolume(1.5f);
		hasEnemyAllKill = true;
		ultimate->SetOnUltimate(false);
	}

	// ���݃X�|�[�������ǂ���
	if (!hasNewSpawn)
	{
		// �ړ�����
		if (PAD->xb.LEFT_STICK_X != 0 || PAD->xb.LEFT_STICK_Y != 0)
		{
			// �_�b�V���Q�[�W�ɗ]�T�����肩��LB����������_�b�V��
			if (dushGauge > 5 && PAD->xb.LEFT_BUTTON)
			{
				playerObj.SetVelocityX(PAD->xb.LEFT_STICK_X * -editPlayerParam->dashSpeed);
				playerObj.SetVelocityY(0.0f);
				playerObj.SetVelocityZ(PAD->xb.LEFT_STICK_Y * -editPlayerParam->dashSpeed);
				AddDushGauge(editPlayerParam->subDashGauge * elapsedTime);
				ParticleSystem::GetInstance()->playerDashParticle->Burst(dushGauge, playerObj.GetPos(), pFrameWork.GetElapsedTime(), modelResource, 0.0f, 0.0f);
			}
			// ���ʂ̈ړ�
			else
			{
				playerObj.SetVelocityX(PAD->xb.LEFT_STICK_X * -editPlayerParam->normalSpeed);
				playerObj.SetVelocityY(0.0f);
				playerObj.SetVelocityZ(PAD->xb.LEFT_STICK_Y * -editPlayerParam->normalSpeed);
			}
		}
		// �~�܂����班���������c���~�߂�
		else
		{
			playerObj.SetVelocityX(playerObj.GetVelocity().x * 0.9f);
			playerObj.SetVelocityY(0.0f);
			playerObj.SetVelocityZ(playerObj.GetVelocity().z * 0.9f);
		}
		// �V���b�g�̐���
		if (PAD->xb.RIGHT_STICK_X != 0 || PAD->xb.RIGHT_STICK_Y != 0)
		{
			// �V���b�g�̃f�B���C����
			if (burstElapsedTime >= 0.075f)
			{
				// �T�E���h����
				if (!isShotSound)
				{
					if (isReinforce)
					{
						Sound::GetInstance()->playerShot->StopWAV();
						Sound::GetInstance()->buffPlayerShot->StopWAV();
						Sound::GetInstance()->buffPlayerShot->PlayWAV();
						Sound::GetInstance()->buffPlayerShot->SetVolume(0.1f);
					}
					else
					{
						Sound::GetInstance()->playerShot->StopWAV();
						Sound::GetInstance()->buffPlayerShot->StopWAV();
						Sound::GetInstance()->playerShot->PlayWAV();
						Sound::GetInstance()->playerShot->SetVolume(0.1f);
					}
					isShotSound = true;
				}

				// �ǂ̕����ɑł��X�e�B�b�N�̓��͂Ōv�Z
				float ang = atan2(0 - PAD->xb.RIGHT_STICK_X, 0 - PAD->xb.RIGHT_STICK_Y);
				{
					// ����΂点�邽�߂ɗ������g��
					int r = rand() % editPlayerParam->shotRandomValue1 - (editPlayerParam->shotRandomValue1 / 2.0f);
					DirectX::XMFLOAT3 shotVec =
					{
						sinf(ang + r * 0.01745f) * 50.0f,
						0.0f,
						cosf(ang + r * 0.01745f) * 50.0f
					};
					PlayerShotSystem::GetInstance()->ShotBurst(playerObj.GetPos(), shotVec, isReinforce);
				}

				{
					int r = rand() % editPlayerParam->shotRandomValue1 - (editPlayerParam->shotRandomValue1 / 2.0f);
					DirectX::XMFLOAT3 shotVec =
					{
						sinf(ang + r * 0.01745f) * 50.0f,
						0.0f,
						cosf(ang + r * 0.01745f) * 50.0f
					};
					PlayerShotSystem::GetInstance()->ShotBurst(playerObj.GetPos(), shotVec, isReinforce);
				}

				{
					int r = rand() % editPlayerParam->shotRandomValue1 - (editPlayerParam->shotRandomValue1 / 2.0f);
					DirectX::XMFLOAT3 shotVec =
					{
						sinf(ang + r * 0.01745f) * 50.0f,
						0.0f,
						cosf(ang + r * 0.01745f) * 50.0f
					};
					PlayerShotSystem::GetInstance()->ShotBurst(playerObj.GetPos(), shotVec, isReinforce);
				}

				if (isReinforce)
				{
					if (!isBuffShotSound)
					{
						isBuffShotSound = true;
						isShotSound = false;
					}
					{
						int r = rand() % editPlayerParam->shotRandomValue2 - (editPlayerParam->shotRandomValue2 / 2.0f);
						DirectX::XMFLOAT3 shotVec =
						{
							sinf(ang + r * 0.01745f) * 50.0f,
							0.0f,
							cosf(ang + r * 0.01745f) * 50.0f
						};
						PlayerShotSystem::GetInstance()->ShotBurst(playerObj.GetPos(), shotVec, isReinforce);
					}

					{
						int r = rand() % editPlayerParam->shotRandomValue2 - (editPlayerParam->shotRandomValue2 / 2.0f);
						DirectX::XMFLOAT3 shotVec =
						{
							sinf(ang + r * 0.01745f) * 50.0f,
							0.0f,
							cosf(ang + r * 0.01745f) * 50.0f
						};
						PlayerShotSystem::GetInstance()->ShotBurst(playerObj.GetPos(), shotVec, isReinforce);
					}

					{
						int r = rand() % editPlayerParam->shotRandomValue3 - (editPlayerParam->shotRandomValue3 / 2.0f);
						DirectX::XMFLOAT3 shotVec =
						{
							sinf(ang + r * 0.01745f) * 50.0f,
							0.0f,
							cosf(ang + r * 0.01745f) * 50.0f
						};
						PlayerShotSystem::GetInstance()->ShotBurst(playerObj.GetPos(), shotVec, isReinforce);
					}

					{
						int r = rand() % editPlayerParam->shotRandomValue3 - (editPlayerParam->shotRandomValue3 / 2.0f);
						DirectX::XMFLOAT3 shotVec =
						{
							sinf(ang + r * 0.01745f) * 50.0f,
							0.0f,
							cosf(ang + r * 0.01745f) * 50.0f
						};
						PlayerShotSystem::GetInstance()->ShotBurst(playerObj.GetPos(), shotVec, isReinforce);
					}

				}
				else
				{
					// �o�t��Ԃƕ��ʂ̎��ŉ���ς��邽�߂̃t���O�Ǘ�
					if (isBuffShotSound)
					{
						isBuffShotSound = false;
						isShotSound = false;
					}
				}

				burstElapsedTime = 0.0f;
			}
			burstElapsedTime += elapsedTime;

		}
		else if (PAD->mb.left)
		{
			// �V���b�g�̃f�B���C����
			if (burstElapsedTime >= 0.075f)
			{
				// �T�E���h����
				if (!isShotSound)
				{
					if (isReinforce)
					{
						Sound::GetInstance()->playerShot->StopWAV();
						Sound::GetInstance()->buffPlayerShot->StopWAV();
						Sound::GetInstance()->buffPlayerShot->PlayWAV();
						Sound::GetInstance()->buffPlayerShot->SetVolume(0.1f);
					}
					else
					{
						Sound::GetInstance()->playerShot->StopWAV();
						Sound::GetInstance()->buffPlayerShot->StopWAV();
						Sound::GetInstance()->playerShot->PlayWAV();
						Sound::GetInstance()->playerShot->SetVolume(0.1f);
					}
					isShotSound = true;
				}

				// �ǂ̕����ɑł��}�E�X�̓��͂Ōv�Z
				float ang = atan2((1920 / 2.0f) - PAD->mb.X, (1080 / 2.0f) - PAD->mb.Y);
				{
					// ����΂点�邽�߂ɗ������g��
					int r = rand() % editPlayerParam->shotRandomValue1 - (editPlayerParam->shotRandomValue1 / 2.0f);
					DirectX::XMFLOAT3 shotVec =
					{
						sinf(ang + r * 0.01745f) * 50.0f,
						0.0f,
						cosf(ang + r * 0.01745f) * 50.0f
					};
					PlayerShotSystem::GetInstance()->ShotBurst(playerObj.GetPos(), shotVec, isReinforce);
				}

				{
					int r = rand() % editPlayerParam->shotRandomValue1 - (editPlayerParam->shotRandomValue1 / 2.0f);
					DirectX::XMFLOAT3 shotVec =
					{
						sinf(ang + r * 0.01745f) * 50.0f,
						0.0f,
						cosf(ang + r * 0.01745f) * 50.0f
					};
					PlayerShotSystem::GetInstance()->ShotBurst(playerObj.GetPos(), shotVec, isReinforce);
				}

				{
					int r = rand() % editPlayerParam->shotRandomValue1 - (editPlayerParam->shotRandomValue1 / 2.0f);
					DirectX::XMFLOAT3 shotVec =
					{
						sinf(ang + r * 0.01745f) * 50.0f,
						0.0f,
						cosf(ang + r * 0.01745f) * 50.0f
					};
					PlayerShotSystem::GetInstance()->ShotBurst(playerObj.GetPos(), shotVec, isReinforce);
				}

				if (isReinforce)
				{
					if (!isBuffShotSound)
					{
						isBuffShotSound = true;
						isShotSound = false;
					}
					{
						int r = rand() % editPlayerParam->shotRandomValue2 - (editPlayerParam->shotRandomValue2 / 2.0f);
						DirectX::XMFLOAT3 shotVec =
						{
							sinf(ang + r * 0.01745f) * 50.0f,
							0.0f,
							cosf(ang + r * 0.01745f) * 50.0f
						};
						PlayerShotSystem::GetInstance()->ShotBurst(playerObj.GetPos(), shotVec, isReinforce);
					}

					{
						int r = rand() % editPlayerParam->shotRandomValue2 - (editPlayerParam->shotRandomValue2 / 2.0f);
						DirectX::XMFLOAT3 shotVec =
						{
							sinf(ang + r * 0.01745f) * 50.0f,
							0.0f,
							cosf(ang + r * 0.01745f) * 50.0f
						};
						PlayerShotSystem::GetInstance()->ShotBurst(playerObj.GetPos(), shotVec, isReinforce);
					}

					{
						int r = rand() % editPlayerParam->shotRandomValue3 - (editPlayerParam->shotRandomValue3 / 2.0f);
						DirectX::XMFLOAT3 shotVec =
						{
							sinf(ang + r * 0.01745f) * 50.0f,
							0.0f,
							cosf(ang + r * 0.01745f) * 50.0f
						};
						PlayerShotSystem::GetInstance()->ShotBurst(playerObj.GetPos(), shotVec, isReinforce);
					}

					{
						int r = rand() % editPlayerParam->shotRandomValue3 - (editPlayerParam->shotRandomValue3 / 2.0f);
						DirectX::XMFLOAT3 shotVec =
						{
							sinf(ang + r * 0.01745f) * 50.0f,
							0.0f,
							cosf(ang + r * 0.01745f) * 50.0f
						};
						PlayerShotSystem::GetInstance()->ShotBurst(playerObj.GetPos(), shotVec, isReinforce);
					}

				}
				else
				{
					// �o�t��Ԃƕ��ʂ̎��ŉ���ς��邽�߂̃t���O�Ǘ�
					if (isBuffShotSound)
					{
						isBuffShotSound = false;
						isShotSound = false;
					}
				}

				burstElapsedTime = 0.0f;
			}
			burstElapsedTime += elapsedTime;
		}
		// �V���b�g��ł��Ă��Ȃ�
		else
		{
			// �T�E���h���~�߂�
			if (isShotSound)
			{
				Sound::GetInstance()->playerShot->StopWAV();
				Sound::GetInstance()->buffPlayerShot->StopWAV();
			}
			isBuffShotSound = false;
			isShotSound = false;
			vibValue = 0.0f;
			burstElapsedTime = 0.0f;
		}

		// �o�t��Ԃ̎��̃G�t�F�N�g����
		if (isReinforce)
		{
			if (emitPowerUpEffectElapsedTime > 0.1f)
			{
				floorFillEffect->EmitEffect(playerObj.GetPos(), 10.0f);
				emitPowerUpEffectElapsedTime = 0.0f;
			}
			emitPowerUpEffectElapsedTime += elapsedTime;
		}
		else
		{
			emitPowerUpEffectElapsedTime = 0.0f;
		}

		// �ړ������ɉ�]������
		{
			DirectX::XMMATRIX _rotetaMat;
			DirectX::XMVECTOR _forward, _right, _q, q;
			// ���݂̃N�H�[�^�j�I�����x�N�^�[�ɕϊ�
			q = DirectX::XMLoadFloat4(&quaternion);

			// ���̐ݒ�
			_forward = DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f));
			_right = DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f));

			if (abs(PAD->xb.LEFT_STICK_X) > 100)
			{
				_q = DirectX::XMQuaternionRotationAxis(_forward, (PAD->xb.LEFT_STICK_X / editPlayerParam->moveRotation) * elapsedTime);
				q = DirectX::XMQuaternionMultiply(q, _q);
				//playerObj.setRotateZ(playerObj.getRotate().z - (PAD->xb.LEFT_STICK_X / - 100.0f) * elapsedTime); // �c
			}
			if (abs(PAD->xb.LEFT_STICK_Y) > 100)
			{
				_q = DirectX::XMQuaternionRotationAxis(_right, (PAD->xb.LEFT_STICK_Y / -editPlayerParam->moveRotation) * elapsedTime);
				q = DirectX::XMQuaternionMultiply(q, _q);
				//playerObj.setRotateX(playerObj.getRotate().x + (PAD->xb.LEFT_STICK_Y / -100.0f) * elapsedTime); // �c
			}

			// �v�Z���ʂ���]�s��ɕϊ�
			DirectX::XMStoreFloat4(&quaternion, q);
			_rotetaMat = DirectX::XMMatrixRotationQuaternion(q);
			DirectX::XMStoreFloat4x4(&rotateMat, _rotetaMat);
		}

		// ��Ƀ_�b�V���Q�[�W�𑝂₷
		AddDushGauge(editPlayerParam->addDashGauge * elapsedTime);
		
	}
	else
	{
		// �X�|�[����
		float e = Easing::InSine(newSpawnTime, 1.0f, 1.0f, 0.0f);

		// ���X�ɏ����ʒu�ɖ߂�
		playerObj.SetPosX(playerObj.GetPos().x + ((0.0f - playerObj.GetPos().x) * e));
		playerObj.SetPosZ(playerObj.GetPos().z + ((0.0f - playerObj.GetPos().z) * e));

		newSpawnTime += elapsedTime;
		if (newSpawnTime >= 1.0f)
		{
			// �^�񒆂ɖ߂�����R���g���[���[�̃o�C�u���~�߂�
			if (!isVib)
			{
				isVib = true;
				StopVibration();
			}

			// ������
			e = Easing::InSine(1.0f, 1.0f, 1.0f, 0.0f);

			playerObj.SetPosX(playerObj.GetPos().x + ((0.0f - playerObj.GetPos().x) * e));
			playerObj.SetPosZ(playerObj.GetPos().z + ((0.0f - playerObj.GetPos().z) * e));

			// �X�|�[���G�t�F�N�g����
			if (!emitSpawnEffect)
			{
				Sound::GetInstance()->playerSpawn->PlayWAV();
				ParticleSystem::GetInstance()->playerSpawnParticle->Burst(10000, playerObj.GetWorldMatrix(), pFrameWork.GetElapsedTime(), modelResource, 0.0f, 0.0f);
				emitSpawnEffect = true;
			}
			playerObj.SetVelocity({ 0.0f, 0.0f, 0.0f });
			if (newSpawnTime >= 2.0f)
			{
				isVib = false;
				emitSpawnEffect = false;
				hasNewSpawn = false;
				newSpawnTime = 0.0f;
			}
		}
	}

	DirectX::XMFLOAT3 nextPos;
	{
		nextPos.x = playerObj.GetPos().x + playerObj.GetVelocity().x * elapsedTime;
		nextPos.y = 0.5f;
		nextPos.z = playerObj.GetPos().z + playerObj.GetVelocity().z * elapsedTime;
	}


	playerObj.SetPos(nextPos);
	playerCollision->SetPos(playerObj.GetPos());

	{
		playerDestoryEffect->Update(playerObj.GetPos(), hasNewSpawn, elapsedTime);
	}

	// Set Camera Target 
	{
		// ���S����v���C���[�ւ̃x�N�g���𐶐�
		DirectX::XMFLOAT3 targetPos = { playerObj.GetPos().x, playerObj.GetPos().y , playerObj.GetPos().z };
		DirectX::XMVECTOR targetPosVec = DirectX::XMLoadFloat3(&targetPos);
		DirectX::XMVECTOR targetPosNmlVec = DirectX::XMVector3Normalize(targetPosVec);
		DirectX::XMVECTOR len = DirectX::XMVector3Length(targetPosVec);

		float targetLen = 0;
		DirectX::XMStoreFloat(&targetLen, len);
		DirectX::XMStoreFloat3(&targetPos, targetPosNmlVec);
		// ��L�̃x�N�g������J�����̃^�[�Q�b�g���v�Z
		targetPos = { targetPos.x * targetLen * 0.8f, targetPos.y, targetPos.z * targetLen * 0.8f * -1.0f };

		Camera::GetInstance().SetTarget(targetPos);
	}

	// luninansAnimation
	{
		// �v���C���[�̋P�x��_�ł�����
		float e = Easing::OutQuad(luminansAnimationElapsedTime, luminansAnimationTime, 2.0f, 0.25f);

		luminans.x = 50.0f * e;
		luminans.y = 50.0f * e;
		luminans.z = 60.0f * e;
		luminans.w = 1.0f;

		luminansAnimationElapsedTime += luminansAnimationAddTime * elapsedTime;
		if (luminansAnimationTime < luminansAnimationElapsedTime)
		{
			luminansAnimationElapsedTime = luminansAnimationTime;
			luminansAnimationAddTime = -1.0f;
		}
		else if (luminansAnimationElapsedTime < 0.0f)
		{
			luminansAnimationElapsedTime = 0.0f;
			luminansAnimationAddTime = 1.0f;
		}
	}

	floorFillEffect->Update(playerObj.GetPos(), elapsedTime);


}

void Player::Render()
{
	if (!hasNewSpawn)
	{
		modelRenderer->Begin(pFrameWork.GetContext());
		playerObj.GiveModelInfo(rotateMat);
		modelRenderer->Draw(pFrameWork.GetContext(), playerObj.GetModel(), luminans);
		modelRenderer->End(pFrameWork.GetContext());
	}

	//playerCollision->CalcWorldMatrix();
	//playerCollision->Render();

	playerDestoryEffect->Render();
	floorFillEffect->Render();
}

void Player::Finalize()
{

}

void Player::SubHp()
{
	hp--;
	hasNewSpawn = true;
	newSpawnTime = 0.0f;
}

void Player::SetEditPlayerParam(std::shared_ptr<Edit::Player::EditPlayerParameter> ptr)
{
	editPlayerParam = ptr; 
}
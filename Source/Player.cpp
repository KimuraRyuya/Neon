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

	// アルティメットの入力
	ultimate->SetOnUltimateCountDouwn(hasFillChipDestory);
	ultimate->Update(elapsedTime);

	if (ultimate->GetOnUltimate())
	{
		Sound::GetInstance()->enemyAllDestory->PlayWAV();
		Sound::GetInstance()->enemyAllDestory->SetVolume(1.5f);
		hasEnemyAllKill = true;
		ultimate->SetOnUltimate(false);
	}

	// 現在スポーン中かどうか
	if (!hasNewSpawn)
	{
		// 移動処理
		if (PAD->xb.LEFT_STICK_X != 0 || PAD->xb.LEFT_STICK_Y != 0)
		{
			// ダッシュゲージに余裕がありかつLBを押したらダッシュ
			if (dushGauge > 5 && PAD->xb.LEFT_BUTTON)
			{
				playerObj.SetVelocityX(PAD->xb.LEFT_STICK_X * -editPlayerParam->dashSpeed);
				playerObj.SetVelocityY(0.0f);
				playerObj.SetVelocityZ(PAD->xb.LEFT_STICK_Y * -editPlayerParam->dashSpeed);
				AddDushGauge(editPlayerParam->subDashGauge * elapsedTime);
				ParticleSystem::GetInstance()->playerDashParticle->Burst(dushGauge, playerObj.GetPos(), pFrameWork.GetElapsedTime(), modelResource, 0.0f, 0.0f);
			}
			// 普通の移動
			else
			{
				playerObj.SetVelocityX(PAD->xb.LEFT_STICK_X * -editPlayerParam->normalSpeed);
				playerObj.SetVelocityY(0.0f);
				playerObj.SetVelocityZ(PAD->xb.LEFT_STICK_Y * -editPlayerParam->normalSpeed);
			}
		}
		// 止まったら少し慣性を残しつつ止める
		else
		{
			playerObj.SetVelocityX(playerObj.GetVelocity().x * 0.9f);
			playerObj.SetVelocityY(0.0f);
			playerObj.SetVelocityZ(playerObj.GetVelocity().z * 0.9f);
		}
		// ショットの生成
		if (PAD->xb.RIGHT_STICK_X != 0 || PAD->xb.RIGHT_STICK_Y != 0)
		{
			// ショットのディレイ処理
			if (burstElapsedTime >= 0.075f)
			{
				// サウンド処理
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

				// どの方向に打つかスティックの入力で計算
				float ang = atan2(0 - PAD->xb.RIGHT_STICK_X, 0 - PAD->xb.RIGHT_STICK_Y);
				{
					// ちらばらせるために乱数を使う
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
					// バフ状態と普通の時で音を変えるためのフラグ管理
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
			// ショットのディレイ処理
			if (burstElapsedTime >= 0.075f)
			{
				// サウンド処理
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

				// どの方向に打つかマウスの入力で計算
				float ang = atan2((1920 / 2.0f) - PAD->mb.X, (1080 / 2.0f) - PAD->mb.Y);
				{
					// ちらばらせるために乱数を使う
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
					// バフ状態と普通の時で音を変えるためのフラグ管理
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
		// ショットを打っていない
		else
		{
			// サウンドを止める
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

		// バフ状態の時のエフェクト生成
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

		// 移動方向に回転させる
		{
			DirectX::XMMATRIX _rotetaMat;
			DirectX::XMVECTOR _forward, _right, _q, q;
			// 現在のクォータニオンをベクターに変換
			q = DirectX::XMLoadFloat4(&quaternion);

			// 軸の設定
			_forward = DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f));
			_right = DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f));

			if (abs(PAD->xb.LEFT_STICK_X) > 100)
			{
				_q = DirectX::XMQuaternionRotationAxis(_forward, (PAD->xb.LEFT_STICK_X / editPlayerParam->moveRotation) * elapsedTime);
				q = DirectX::XMQuaternionMultiply(q, _q);
				//playerObj.setRotateZ(playerObj.getRotate().z - (PAD->xb.LEFT_STICK_X / - 100.0f) * elapsedTime); // 縦
			}
			if (abs(PAD->xb.LEFT_STICK_Y) > 100)
			{
				_q = DirectX::XMQuaternionRotationAxis(_right, (PAD->xb.LEFT_STICK_Y / -editPlayerParam->moveRotation) * elapsedTime);
				q = DirectX::XMQuaternionMultiply(q, _q);
				//playerObj.setRotateX(playerObj.getRotate().x + (PAD->xb.LEFT_STICK_Y / -100.0f) * elapsedTime); // 縦
			}

			// 計算結果を回転行列に変換
			DirectX::XMStoreFloat4(&quaternion, q);
			_rotetaMat = DirectX::XMMatrixRotationQuaternion(q);
			DirectX::XMStoreFloat4x4(&rotateMat, _rotetaMat);
		}

		// 常にダッシュゲージを増やす
		AddDushGauge(editPlayerParam->addDashGauge * elapsedTime);
		
	}
	else
	{
		// スポーン中
		float e = Easing::InSine(newSpawnTime, 1.0f, 1.0f, 0.0f);

		// 徐々に初期位置に戻す
		playerObj.SetPosX(playerObj.GetPos().x + ((0.0f - playerObj.GetPos().x) * e));
		playerObj.SetPosZ(playerObj.GetPos().z + ((0.0f - playerObj.GetPos().z) * e));

		newSpawnTime += elapsedTime;
		if (newSpawnTime >= 1.0f)
		{
			// 真ん中に戻ったらコントローラーのバイブを止める
			if (!isVib)
			{
				isVib = true;
				StopVibration();
			}

			// 微調整
			e = Easing::InSine(1.0f, 1.0f, 1.0f, 0.0f);

			playerObj.SetPosX(playerObj.GetPos().x + ((0.0f - playerObj.GetPos().x) * e));
			playerObj.SetPosZ(playerObj.GetPos().z + ((0.0f - playerObj.GetPos().z) * e));

			// スポーンエフェクト生成
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
		// 中心からプレイヤーへのベクトルを生成
		DirectX::XMFLOAT3 targetPos = { playerObj.GetPos().x, playerObj.GetPos().y , playerObj.GetPos().z };
		DirectX::XMVECTOR targetPosVec = DirectX::XMLoadFloat3(&targetPos);
		DirectX::XMVECTOR targetPosNmlVec = DirectX::XMVector3Normalize(targetPosVec);
		DirectX::XMVECTOR len = DirectX::XMVector3Length(targetPosVec);

		float targetLen = 0;
		DirectX::XMStoreFloat(&targetLen, len);
		DirectX::XMStoreFloat3(&targetPos, targetPosNmlVec);
		// 上記のベクトルからカメラのターゲットを計算
		targetPos = { targetPos.x * targetLen * 0.8f, targetPos.y, targetPos.z * targetLen * 0.8f * -1.0f };

		Camera::GetInstance().SetTarget(targetPos);
	}

	// luninansAnimation
	{
		// プレイヤーの輝度を点滅させる
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
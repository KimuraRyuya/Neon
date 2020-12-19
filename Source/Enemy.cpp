#include <iostream>
#include "../Library/Camera.h"
#include "Enemy.h"

void Enemy::Initialize(EnemyBufferInfo& enm)
{
	enm.pos = { 0.0f, 0.0f, 0.0f };
	enm.scale = { 0.0f, 0.0f, 0.0f };
	enm.size = { 0.0f, 0.0f, 0.0f };
	enm.hp = 0;

	isExist = false;

	// Geometry Collision Set
	enmCollision = std::make_unique<CollisionPrimitive>(1, true, DirectX::XMFLOAT3(1, 1, 1));
	enmCollision->SetColor({ 0, 1, 0, 1 });
}

bool Enemy::Pop(EnemyBufferInfo& enm)
{
	if (isExist) return false;
	enm.pos = { 10, 0, 10};
	enm.scale = { 1.0f, 1.0f, 1.0f };
	enm.size = { 1.0f, 1.0f, 1.0f };
	enm.hp = 20;
	isExist = true;
	GiveModelInfo(enm);
	return true;
}

bool Enemy::ChasePop(EnemyBufferInfo& enm, Enemy::ActionType type, int level, std::shared_ptr<Model> _model, std::shared_ptr<Model> _spawnModel)
{
	// 引数を使用してパラメーターの設定
	// レベルによってパラメータ－を変える
	int h = 0;
	switch (level)
	{
	case 1:
		speed = 3.0f;
		h = 40;
		break;
	case 2:
		speed = 6.0f;
		h = 80;
		break;
	case 3:
		speed = 12.0f;
		h = 60;
		break;
	}

	colorType = ColorType::Yellow;

	int emitAngle = rand() % 360;
	int emitDist = rand() % 84 - 42;
	enm.pos = { sinf(static_cast<float>(emitAngle) * 0.01745f) * emitDist, 0.5f, cosf(static_cast<float>(emitAngle) * 0.01745f) * emitDist };
	enm.scale = { 1.0f, 1.0f, 1.0f };
	enm.size = { 1.0f, 1.0f, 1.0f };
	enm.rotate = { 0.0f, 0.0f, 0.0f };
	enm.hp = h;
	angle = 0;
	isExist = true;
	spawnTimer = 0.0f;
	spawnMaxTimer = 3.0f;
	hasNowSpawn = true;
	actionType = type;
	model = _spawnModel;
	tmpModel = _model;
	GiveModelInfo(enm);
	return true;
}

bool Enemy::LinearPop(EnemyBufferInfo& enm, DirectX::XMFLOAT3 p, int level, Enemy::ActionType type, std::shared_ptr<Model> _model, std::shared_ptr<Model> _spawnModel)
{
	// 引数を使用してパラメーターの設定
	// レベルによってパラメータ－を変える
	int h = 0;
	switch (level)
	{
	case 1:
		speed = 3.0f;
		h = 30;
		break;
	case 2:
		speed = 6.0f;
		h = 60;
		break;
	case 3:
		speed = 12.0f;
		h = 60;
		break;
	}

	colorType = ColorType::Purple;

	switch (type)
	{
	case Enemy::ActionType::Top_to_Bottom_Liner:
		{
			velocity = { 0.0f * speed, 0.0f * speed, -1.0f * speed };
			enm.hp = h;
			break;
		}
		case Enemy::ActionType::Bottom_to_Top_Liner:
		{
			velocity = { 0.0f * speed, 0.0f * speed, 1.0f * speed };
			enm.hp = h;
			break;
		}
		case Enemy::ActionType::Right_to_LeftLiner:
		{

			velocity = { -1.0f * speed, 0.0f * speed, 0.0f * speed };
			enm.hp = h;
			break;
		} 
		case Enemy::ActionType::Left_to_Right_Liner:
		{
			velocity = { 1.0f * speed, 0.0f * speed, 0.0f * speed };
			enm.hp = h;
		break;
		}
		case Enemy::ActionType::WARNING:
		{
			DirectX::XMFLOAT3 vel = { 0.0f - p.x, 0.0f , 0.0f - p.z };
			DirectX::XMVECTOR _vel = DirectX::XMLoadFloat3(&vel);
			_vel = DirectX::XMVector3Normalize(_vel);
			DirectX::XMStoreFloat3(&vel, _vel);
			velocity = { vel.x * speed, vel.y * speed, vel.z * speed };
			enm.hp = h;
			break;
		}
	}

	enm.pos = p;
	enm.pos.y = 0.5f;
	enm.scale = { 1.0f, 1.0f, 1.0f };
	enm.size = { 1.0f, 1.0f, 1.0f };
	enm.rotate = { 0.0f, 0.0f, 0.0f };
	isExist = true;
	spawnTimer = 0.0f;
	spawnMaxTimer = 2.0f;
	hasNowSpawn = true;
	actionType = type;
	model = _spawnModel;
	tmpModel = _model;
	GiveModelInfo(enm);
	return true;
}

bool Enemy::ReflexPop(EnemyBufferInfo& enm, Enemy::ActionType type, int level, std::shared_ptr<Model> _model, std::shared_ptr<Model> _spawnModel)
{
	// 引数を使用してパラメーターの設定
	// レベルによってパラメータ－を変える
	switch (level)
	{
	case 1:
		speed = 3.0f;
		break;
	case 2:
		speed = 6.0f;
		break;
	case 3:
		speed = 12.0f;
		break;
	}

	colorType = ColorType::Red;

	int emitAngle = rand() % 360;
	int emitDist = rand() % 80 - 40;
	enm.pos = { sinf(static_cast<float>(emitAngle) * 0.01745f) * emitDist, 0.5f, cosf(static_cast<float>(emitAngle) * 0.01745f) * emitDist };
	enm.scale = { 1.0f, 1.0f, 1.0f };
	enm.size = { 1.0f, 1.0f, 1.0f };
	enm.rotate = { 0.0f, 0.0f, 0.0f };
	velocity = { (rand() % 100 / 100.0f) * speed, 0,  (rand() % 100 / 100.0f) * speed };
	enm.hp = 40;
	isExist = true;
	spawnTimer = 0.0f;
	spawnMaxTimer = 3.0f;
	hasNowSpawn = true;
	actionType = type;
	model = _spawnModel;
	tmpModel = _model;
	GiveModelInfo(enm);
	return true;
}

void Enemy::Move(EnemyBufferInfo& enm, float elapsedTime)
{
	float crossAngle = 0;

	// スポーン中
	if (hasNowSpawn)
	{
		spawnTimer += 1.5f * elapsedTime;
		if (spawnTimer >= spawnMaxTimer)
		{
			spawnTimer = 0.0f;
			hasNowSpawn = false;
			model = tmpModel;
		}
	}
	else
	{
		// ActionTypeによって別々の動くをさせる
		switch (actionType)
		{
		case Enemy::ActionType::Chase:
			// 進行方向ベクトル
			DirectX::XMFLOAT2 enmVec = { enm.pos.x + velocity.x - enm.pos.x, enm.pos.z + velocity.z - enm.pos.z };
			DirectX::XMVECTOR _enmVec = DirectX::XMLoadFloat2(&enmVec);
			_enmVec = DirectX::XMVector2Normalize(_enmVec);

			// プレイヤーとのベクトル
			DirectX::XMFLOAT2 targetVec = { target.x - enm.pos.x, target.z - enm.pos.z };
			DirectX::XMVECTOR _targetVec = DirectX::XMLoadFloat2(&targetVec);
			_targetVec = DirectX::XMVector2Normalize(_targetVec);

			// 外積して向かないといけない角度を算術
			DirectX::XMVECTOR cross = DirectX::XMVector2Cross(_targetVec, _enmVec);
			DirectX::XMStoreFloat(&crossAngle, cross);

			// 少しずつ角度を変える
			if (crossAngle > 0)
			{
				angle += 1 * 0.01745f;
			}
			else
			{
				angle -= 1 * 0.01745f;
			}
			velocity = { sin(angle) * speed , 0, cos(angle) * speed };
			enm.pos = { enm.pos.x + velocity.x * elapsedTime, enm.pos.y + velocity.y * elapsedTime, enm.pos.z + velocity.z * elapsedTime };

			break;
		case Enemy::ActionType::Top_to_Bottom_Liner:
		case Enemy::ActionType::Bottom_to_Top_Liner:
		case Enemy::ActionType::Right_to_LeftLiner:
		case Enemy::ActionType::Left_to_Right_Liner:
		case Enemy::ActionType::WARNING:
			// 直進のみ
			enm.pos = { enm.pos.x + velocity.x * elapsedTime , enm.pos.y + velocity.y * elapsedTime, enm.pos.z + velocity.z * elapsedTime };
			enmCollision->SetPos(enm.pos);
			{
				DirectX::XMFLOAT2 _p = { enm.pos.x, enm.pos.z };
				// 壁に当たると跳ね返る
				if (!Collision::CircleVsCircleAndExtrusion(_p, 1.0f, { 0, 0 }, 44 * 0.8f))
				{
					velocity.x *= -1;
					velocity.z *= -1;
				}
			}
			
			break;
		case Enemy::ActionType::Reflection:
			// 直進のみ
			enm.pos = { enm.pos.x + velocity.x * elapsedTime, enm.pos.y + velocity.y * elapsedTime, enm.pos.z + velocity.z * elapsedTime };
			{
				DirectX::XMFLOAT2 _p = { enm.pos.x, enm.pos.z };
				// 壁に当たると跳ね返る
				if (!Collision::CircleVsCircleAndExtrusion(_p, 1.0f, { 0, 0 }, 44 * 0.8f))
				{
					velocity.x *= -1;
					velocity.z *= -1;
				}
			}
			break;
		}
		// Calc Rotate
		{
			enm.rotate.x += velocity.z / speed * 4.0f * elapsedTime; // 縦

			enm.rotate.z += velocity.x / speed * 4.0f * elapsedTime; // 縦
		}
		enm.scale = { 1.5f, 1.5f, 1.5f };
	}
}

void Enemy::EnemyInfClear(EnemyBufferInfo& enm)
{
	enm.pos = {0.0f, 0.0f, 0.0f};
	enm.scale = { 1.0f, 1.0f, 1.0f };
	enm.size = { 1.0f, 1.0f, 1.0f };
	enm.rotate = { 0.0f, 0.0f, 0.0f };
	velocity = {0.0f, 0.0f, 0.0f};
	enm.hp = 40;
	isExist = false;
	spawnTimer = 0.0f;
	spawnMaxTimer = 3.0f;
	hasNowSpawn = true;
	model = nullptr;
	tmpModel = nullptr;
}

void Enemy::GiveModelInfo(EnemyBufferInfo& enm)
{
	if (!model) return;
	// ワールド行列を作成
	DirectX::XMMATRIX W;
	{
		DirectX::XMMATRIX S, R, Rx, Ry, Rz, T;
		S = DirectX::XMMatrixScaling(enm.scale.x, enm.scale.y, enm.scale.z);

		//	回転
		Rx = DirectX::XMMatrixRotationX(enm.rotate.x);	// X軸を基準とした回転行列
		Ry = DirectX::XMMatrixRotationY(enm.rotate.y);	// Y軸を基準とした回転行列
		Rz = DirectX::XMMatrixRotationZ(enm.rotate.z);		// Z軸を基準とした回転行列
		R = DirectX::XMMatrixRotationRollPitchYaw(0.0f, 0.0f, 0.0f);
		R = Rz * Ry * Rx;

		T = DirectX::XMMatrixTranslation(enm.pos.x, enm.pos.y, enm.pos.z);
		W = S * R * T;
	}

	model->UpdateAnimation(1.0f / 60.0f);
	model->CalculateLocalTransform();
	model->CalculateWorldTransform(W);

}

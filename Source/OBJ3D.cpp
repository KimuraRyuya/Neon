#include "OBJ3D.h"
#include "../Library/Camera.h"
#include "../Library/FbxLoader.h"
#include "../Library/FrameWork.h"

void OBJ3D::Initialize(std::shared_ptr<ModelResource>& resource)
{
	pos = { 0.0f, 0.0f, 0.0f };
	scale = { 1.0f, 1.0f, 1.0f };
	rotate = { 0.0f, 0.0f, 0.0f };
	color = { 1.0f, 1.0f, 1.0f, 1.0f };
	model = std::make_unique<Model>(resource);

	// ���[���h�s����쐬
	DirectX::XMMATRIX W;
	{
		DirectX::XMMATRIX S, R, T;
		S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
		R = DirectX::XMMatrixRotationRollPitchYaw(rotate.x, rotate.y, rotate.z);	// ZXY��]
		T = DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);
		W = S * R * T;
		DirectX::XMFLOAT4X4 _world;
		DirectX::XMStoreFloat4x4(&_world, W);
		model->SetWorldMatrix(_world);
	}

}

void OBJ3D::Initialize()
{
	pos = { 0.0f, 0.0f, 0.0f };
	scale = { 1.0f, 1.0f, 1.0f };
	rotate = { 0.0f, 0.0f, 0.0f };
	color = { 1.0f, 1.0f, 1.0f, 1.0f };
	velocity = { 0.0f, 0.0f, 0.0f };
	// ���[���h�s����쐬
	DirectX::XMMATRIX W;
	{
		DirectX::XMMATRIX S, R, T;
		S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
		R = DirectX::XMMatrixRotationRollPitchYaw(rotate.x, rotate.y, rotate.z);	// ZXY��]
		T = DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);
		W = S * R * T;
		DirectX::XMFLOAT4X4 _world;
		DirectX::XMStoreFloat4x4(&_world, W);
		model->SetWorldMatrix(_world);
	}

}


void OBJ3D::GiveModelInfo()
{
	if (!model) return;
	// ���[���h�s����쐬
	DirectX::XMMATRIX W;
	{
		DirectX::XMMATRIX S, R, Rx, Ry, Rz, T;
		S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);

		//	��]
		Rx = DirectX::XMMatrixRotationX(rotate.x);	// X������Ƃ�����]�s��
		Ry = DirectX::XMMatrixRotationY(rotate.y);	// Y������Ƃ�����]�s��
		Rz = DirectX::XMMatrixRotationZ(rotate.z);	// Z������Ƃ�����]�s��
		R = DirectX::XMMatrixRotationRollPitchYaw(0.0f, 0.0f, 0.0f);
		R = Rz * Ry * Rx;

		T = DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);
		W = S * R * T;
	}

	//model->UpdateAnimation(1.0f / 60.0f);
	model->CalculateLocalTransform();
	model->CalculateWorldTransform(W);

}

void OBJ3D::GiveModelInfo(DirectX::XMFLOAT4X4 rotateMat)
{
	if (!model) return;
	// ���[���h�s����쐬
	DirectX::XMMATRIX W;
	{
		DirectX::XMMATRIX S, R, Rx, Ry, Rz, T;
		S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);

		//	��]
		R = DirectX::XMLoadFloat4x4(&rotateMat);

		T = DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);
		W = S * R * T;
	}

	//model->UpdateAnimation(1.0f / 60.0f);
	model->CalculateLocalTransform();
	model->CalculateWorldTransform(W);

}

void OBJ3D::CalcWorldMatrix()
{
	if (!model) return;
	// ���[���h�s����쐬
	DirectX::XMMATRIX W;
	{
		DirectX::XMMATRIX S, R, T;
		S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
		R = DirectX::XMMatrixRotationRollPitchYaw(rotate.x, rotate.y, rotate.z);	// ZXY��]
		T = DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);
		W = S * R * T;
		DirectX::XMFLOAT4X4 _world;
		DirectX::XMStoreFloat4x4(&_world, W);
		model->SetWorldMatrix(_world);
	}

}

void OBJ3D::CalcWorldMatrix(DirectX::XMFLOAT4X4 rotateMat)
{
	if (!model) return;
	// ���[���h�s����쐬
	DirectX::XMMATRIX W;
	{
		DirectX::XMMATRIX S, R, T;
		S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
		R = DirectX::XMLoadFloat4x4(&rotateMat);
		T = DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);
		W = S * R * T;
		DirectX::XMFLOAT4X4 _world;
		DirectX::XMStoreFloat4x4(&_world, W);
		model->SetWorldMatrix(_world);
	}

}

void OBJ3DCollision::Initialize()
{
	pos = { 0.0f, 0.0f, 0.0f };
	scale = { 1.0f, 1.0f, 1.0f };
	rotate = { 0.0f, 0.0f, 0.0f };
	color = { 1.0f, 1.0f, 1.0f, 1.0f };
	CalcWorldMatrix();
}

void OBJ3DCollision::CalcWorldMatrix()
{
	// ���[���h�s����쐬
	DirectX::XMMATRIX W;
	{
		DirectX::XMMATRIX S, R, T;
		S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
		R = DirectX::XMMatrixRotationRollPitchYaw(rotate.x, rotate.y, rotate.z);	// ZXY��]
		T = DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z*-1);
		W = S * R * T;
		DirectX::XMStoreFloat4x4(&world, W);
	}

}

DirectX::XMFLOAT4X4 OBJ3D::GetWorldMatrix()
{
	// ���[���h�s����쐬
	DirectX::XMFLOAT4X4 world;
	DirectX::XMMATRIX W;
	{
		DirectX::XMMATRIX S, R, Rx, Ry, Rz, T;
		S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);

		//	��]
		Rx = DirectX::XMMatrixRotationX(rotate.x);	// X������Ƃ�����]�s��
		Ry = DirectX::XMMatrixRotationY(rotate.y);	// Y������Ƃ�����]�s��
		Rz = DirectX::XMMatrixRotationZ(rotate.z);	// Z������Ƃ�����]�s��
		R = DirectX::XMMatrixRotationRollPitchYaw(0.0f, 0.0f, 0.0f);
		R = Rz * Ry * Rx;

		T = DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);
		W = S * R * T;
		DirectX::XMStoreFloat4x4(&world, W);
		return world;
	}
}

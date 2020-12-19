#pragma once
#include "../Library/model.h"

#include <memory>
#include <iostream>

class OBJ3D
{
protected:
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT3 velocity;
	DirectX::XMFLOAT3 scale;
	DirectX::XMFLOAT3 rotate;
	DirectX::XMFLOAT4 color;
	std::unique_ptr<Model> model;

public:
	OBJ3D() 
	{
		pos = { 0.0f, 0.0f, 0.0f };
		velocity = { 0.0f, 0.0f, 0.0f };
		scale = { 1.0f, 1.0f, 1.0f };
		rotate = { 0.0f, 0.0f, 0.0f };
		color = { 1.0f, 1.0f, 1.0f, 1.0f };
		model = nullptr;
	}
	~OBJ3D() {}
	//--------------------------------------------------------------
	// Position
	//--------------------------------------------------------------
	DirectX::XMFLOAT3 GetPos() { return pos; }
	void SetPos(DirectX::XMFLOAT3 _p) { pos = _p; }
	void SetPosX(float _x) { pos.x = _x; }
	void SetPosY(float _y) { pos.y = _y; }
	void SetPosZ(float _z) { pos.z = _z; }

	//--------------------------------------------------------------
	// velocity
	//--------------------------------------------------------------
	DirectX::XMFLOAT3 GetVelocity() { return velocity; }
	void SetVelocity(DirectX::XMFLOAT3 _s) { velocity = _s; }
	void SetVelocityX(float _x) { velocity.x = _x; }
	void SetVelocityY(float _y) { velocity.y = _y; }
	void SetVelocityZ(float _z) { velocity.z = _z; }

	//--------------------------------------------------------------
	// Scale
	//--------------------------------------------------------------
	DirectX::XMFLOAT3 GetScale() { return scale; }
	void SetScale(DirectX::XMFLOAT3 _s) { scale = _s; }
	void SetScaleX(float _x) { scale.x = _x; }
	void SetScaleY(float _y) { scale.y = _y; }
	void SetScaleZ(float _z) { scale.z = _z; }

	//--------------------------------------------------------------
	// Rotate
	//--------------------------------------------------------------
	DirectX::XMFLOAT3 GetRotate() { return rotate; }
	void SetRotate(DirectX::XMFLOAT3 _r) { rotate = _r; }
	void SetRotateX(float _x) { rotate.x = _x; }
	void SetRotateY(float _y) { rotate.y = _y;}
	void SetRotateZ(float _z){rotate.z = _z ; }

	//--------------------------------------------------------------
	// Color
	//--------------------------------------------------------------
	DirectX::XMFLOAT4 GetColor() { return color; }
	void SetColor(DirectX::XMFLOAT4 _c) { color = _c; }
	void SetColorR(float _r) { color.x = _r; }
	void SetColorG(float _g) { color.y = _g; }
	void SetColorB(float _b) { color.z = _b; }
	void SetColorA(float _a) { color.w = _a; }

	//--------------------------------------------------------------
	// Model
	//--------------------------------------------------------------
	Model& GetModel() { return *model; }
	Model* GetModelAddress() { return model.get(); }

	//--------------------------------------------------------------
	// World
	//--------------------------------------------------------------
	DirectX::XMFLOAT4X4 GetWorldMatrix();
	void CalcWorldMatrix();
	void CalcWorldMatrix(DirectX::XMFLOAT4X4 rotateMat);

	void Initialize(std::shared_ptr<ModelResource>& resource);
	void Initialize();
	void GiveModelInfo();
	void GiveModelInfo(DirectX::XMFLOAT4X4 rotateMat);

};

class OBJ3DCollision
{
protected:
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT3 scale;
	DirectX::XMFLOAT3 rotate;
	DirectX::XMFLOAT4 color;
	DirectX::XMFLOAT4X4 world;
public:
	OBJ3DCollision()
	{
		pos = { 0.0f, 0.0f, 0.0f };
		scale = { 1.0f, 1.0f, 1.0f };
		rotate = { 0.0f, 0.0f, 0.0f };
		color = { 1.0f, 1.0f, 1.0f, 1.0f };
		CalcWorldMatrix();
	}
	~OBJ3DCollision() {}

	OBJ3DCollision(const OBJ3DCollision& it)
	{
		pos = it.pos;
		scale = it.scale;
		rotate = it.rotate;
		color = it.color;
		world = it.world;
	}

	//--------------------------------------------------------------
	// Position
	//--------------------------------------------------------------
	DirectX::XMFLOAT3 GetPos() { return pos; }
	void SetPos(DirectX::XMFLOAT3 _p) { pos = _p; }
	void SetPosX(float _x) { pos.x = _x; }
	void SetPosY(float _y) { pos.y = _y; }
	void SetPosZ(float _z) { pos.z = _z; }

	//--------------------------------------------------------------
	// Scale
	//--------------------------------------------------------------
	DirectX::XMFLOAT3 GetScale() { return scale; }
	void GetScale(DirectX::XMFLOAT3 _s) { scale = _s; }
	void SetScaleX(float _x) { scale.x = _x; }
	void SetScaleY(float _y) { scale.y = _y; }
	void SetScaleZ(float _z) { scale.z = _z; }

	//--------------------------------------------------------------
	// Rotate
	//--------------------------------------------------------------
	DirectX::XMFLOAT3 GetRotate() { return rotate; }
	void SetRotate(DirectX::XMFLOAT3 _r) { rotate = _r; }
	void SetRotateX(float _x) { rotate.x = _x; }
	void SetRotateY(float _y) { rotate.y = _y; }
	void SetRotateZ(float _z) { rotate.z = _z; }

	//--------------------------------------------------------------
	// Color
	//--------------------------------------------------------------
	DirectX::XMFLOAT4 GetColor() { return color; }
	void SetColor(DirectX::XMFLOAT4 _c) { color = _c; }
	void SetColorR(float _r) { color.x = _r; }
	void SetColorG(float _g) { color.y = _g; }
	void SetColorB(float _b) { color.z = _b; }
	void SetColorA(float _a) { color.w = _a; }

	//--------------------------------------------------------------
	// World
	//--------------------------------------------------------------
	DirectX::XMFLOAT4X4 GetWorldMatrix() { return world; }
	void CalcWorldMatrix();

	void Initialize();


};
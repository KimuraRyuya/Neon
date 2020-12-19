#pragma once
#include <d3d11.h>
#include <wrl.h>
#include <DirectXMath.h>


class Camera
{
public:

private:
	// èâä˙âªéûÇÃå¥ì_ç¿ïW
	const DirectX::XMFLOAT3	originPos = DirectX::XMFLOAT3(0.0f, 110.0f, 0.0f);
	const DirectX::XMFLOAT3	originTarget = DirectX::XMFLOAT3(0.2f, 1.0f, 0.9f);
	const DirectX::XMFLOAT3	originUp = DirectX::XMFLOAT3(0.06f, 0.99f, 0.01f);
	const DirectX::XMFLOAT3	originRight = DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f);

	DirectX::XMFLOAT3	standard_pos;
	DirectX::XMFLOAT3	pos;
	DirectX::XMFLOAT3	tmpPos;
	DirectX::XMFLOAT3	tmpTarget;
	DirectX::XMFLOAT3	target;
	DirectX::XMFLOAT3	up;
	DirectX::XMFLOAT3	right;

	DirectX::XMFLOAT4 lightDirection;
	float elapsedTimeMag;
	DirectX::XMMATRIX	projection;
	DirectX::XMMATRIX	view;
	DirectX::XMMATRIX view_projection;

	DirectX::XMFLOAT2 oldCursor;
	DirectX::XMFLOAT2 newCursor;

	float rotateX;
	float rotateY;

	float distance;

	// ViewçsóÒçÏê¨
	void CreateView();

	float hei;

public:

	void Init();
	void Update();
	void UpdateEdit();

public:
	void SetRotateX(float x) { rotateX = x; }
	void SetRotateY(float y) { rotateY = y; }
	void SetTarget(DirectX::XMFLOAT3 _p) { target = _p; }
	void SetTempPos(DirectX::XMFLOAT3 p) { tmpPos = p; }
	void SetTempTarget(DirectX::XMFLOAT3 p) { tmpTarget = p; }
	void	SetPos(DirectX::XMFLOAT3 p) { pos = p; }
	void SetElapsedTimeMag(float mag) { elapsedTimeMag = mag; }

	DirectX::XMFLOAT3 GetTempPos() { return tmpPos; }
	DirectX::XMFLOAT3 GetTempTarget() { return tmpTarget; }
	DirectX::XMMATRIX	SetOrthographicMatrix(float w, float h, float znear, float zfar);
	DirectX::XMMATRIX	SetPerspectiveMatrix(float fov, float aspect, float znear, float zfar);
	DirectX::XMMATRIX	GetViewMatrix();
	DirectX::XMMATRIX	GetViewProjectionMatrix();
	DirectX::XMMATRIX	GetProjectionMatrix() { return	projection;}
	DirectX::XMFLOAT3	GetPos() { return pos; }
	DirectX::XMFLOAT3	GetTarget() { return target; }
	DirectX::XMFLOAT4	GetLightDirection() { return lightDirection; }
	float GetElapsedTimeMag() { return elapsedTimeMag; }

	static Camera& GetInstance()
	{
		static Camera instance;
		return instance;
	}
};

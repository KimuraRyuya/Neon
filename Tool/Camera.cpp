#include "Camera.h"
#include "FrameWork.h"

void Camera::Init()
{
	pos = originPos;
	target = originTarget;
	up = originUp;
	right = originRight;
	elapsedTimeMag = 0;

	tmpPos = {0.0f, 0.0f ,0.0f};
	tmpTarget = {0.0f, 0.0f ,0.0f};

	lightDirection = DirectX::XMFLOAT4(0, -1, 0, 0);

	float x = target.x - pos.x;
	float y = target.y - pos.y;
	float z = target.z - pos.z;

	distance = sqrtf(x * x + y * y + z * z);

	float	fov = 30.0f * 0.01745f;
	float	aspect = (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT;
	hei = 50.0f;
	//SetOrthographicMatrix(hei * aspect, hei, 0.1f, 1000000.0f);
	SetPerspectiveMatrix(fov, aspect, 0.1f, 10000.0f);

	standard_pos = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);

	rotateX = 0.0f;
	rotateY = 0.0f;
}


DirectX::XMMATRIX	Camera::SetOrthographicMatrix(float w, float h, float znear, float zfar)
{
	projection = DirectX::XMMatrixOrthographicLH(w, h, znear, zfar);
	return	projection;
}


DirectX::XMMATRIX	Camera::SetPerspectiveMatrix(float fov, float aspect, float znear, float zfar)
{
	projection = DirectX::XMMatrixPerspectiveFovLH(fov, aspect, znear, zfar);

	return	projection;
}


DirectX::XMMATRIX	Camera::GetViewMatrix()
{
	DirectX::XMVECTOR _pos = DirectX::XMVectorSet(pos.x, pos.y, pos.z, 1.0f);
	DirectX::XMVECTOR _target = DirectX::XMVectorSet(target.x, target.y, target.z, 1.0f);
	DirectX::XMVECTOR _up = DirectX::XMVectorSet(up.x, up.y, up.z, 0.0f);
	view = DirectX::XMMatrixLookAtLH(_pos, _target, _up);
	return	view;
}

DirectX::XMMATRIX	Camera::GetViewProjectionMatrix()
{
	// ビュー行列、プロジェクション行列を合成し行列データを取り出す。
	DirectX::XMMATRIX C = DirectX::XMMatrixSet(
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, -1, 0,
		0, 0, 0, 1
	);

	view_projection = C * GetViewMatrix() * GetProjectionMatrix();

	return view_projection;
}


void Camera::Update()
{

	UpdateEdit();
}

void Camera::UpdateEdit()
{
	if (GetKeyState('1') < 0) Init();

	POINT cursor;
	GetCursorPos(&cursor);

	oldCursor = newCursor;
	newCursor = DirectX::XMFLOAT2(static_cast<float>(cursor.x), static_cast<float>(cursor.y));

	float moveX = (newCursor.x - oldCursor.x) * 0.02f;
	float moveY = (newCursor.y - oldCursor.y) * 0.02f;

	if (GetKeyState(VK_MENU) < 0)
	{
		if (GetKeyState(VK_LBUTTON) < 0)
		{
			rotateY += moveX * 0.5f;
			rotateX += moveY * 0.5f;
		}
		else if (GetKeyState(VK_MBUTTON) < 0)
		{
			float s = distance * 0.035f;
			float x = moveX * s;
			float y = moveY * s;

			target.x += right.x * x;
			target.y += right.y * x;
			target.z += right.z * x;

			target.x += up.x * y;
			target.y += up.y * y;
			target.z += up.z * y;
		}
		else if (GetKeyState(VK_RBUTTON) < 0)
		{
			distance += (-moveY - moveX) * distance * 0.1f;
		}
	}

	float xSin = sinf(rotateX);
	float xCos = cosf(rotateX);
	float ySin = sinf(rotateY);
	float yCos = cosf(rotateY);


	DirectX::XMVECTOR front = DirectX::XMVectorSet(-xCos * ySin, -xSin, -xCos * yCos, 0.0f);
	DirectX::XMVECTOR _right = DirectX::XMVectorSet(yCos, 0.0f, -ySin, 0.0f);
	DirectX::XMVECTOR _up = DirectX::XMVector3Cross(_right, front);

	DirectX::XMVECTOR _target = DirectX::XMLoadFloat3(&target);
	DirectX::XMVECTOR _distance = DirectX::XMVectorSet(distance, distance, distance, 0.0f);
	DirectX::XMVECTOR _pos = DirectX::XMVectorSubtract(_target, DirectX::XMVectorMultiply(front, _distance));

	DirectX::XMStoreFloat3(&pos, _pos);
	DirectX::XMStoreFloat3(&up, _up);
	DirectX::XMStoreFloat3(&right, _right);

}


void Camera::CreateView()
{
	DirectX::XMVECTOR _pos = DirectX::XMVectorSet(pos.x, pos.y, pos.z, 1.0f);
	DirectX::XMVECTOR _target = DirectX::XMVectorSet(target.x, target.y, target.z, 1.0f);
	DirectX::XMVECTOR _up = DirectX::XMVectorSet(up.x, up.y, up.z, 0.0f);

	view = DirectX::XMMatrixLookAtLH(_pos, _target, _up);
}
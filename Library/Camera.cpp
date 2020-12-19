#include "Camera.h"
#include "FrameWork.h"
#include "Easing.h"
#include "Misc.h"
#include"Mapper.h"

#include "MyLib.h"

void Camera::create_constnts_buffer()
{
	// シーン用バッファ
	D3D11_BUFFER_DESC desc;
	::memset(&desc, 0, sizeof(desc));
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;
	desc.ByteWidth = sizeof(scene_constants);

	HRESULT hr = pFrameWork.GetDevice()->CreateBuffer(&desc, 0, cb_scene.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
}


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

	isShake = false;
	shakeTotalTime = 0.0f;
	shakeTime = 0.0f;
	shakeValue = 0.0f;

	create_constnts_buffer();

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

	switch (cameraMode)
	{
	case Camera::GAME:
		UpdateModeGame();
		break;
	case Camera::EDIT:
		UpdateEdit();
		break;
	default:
		break;
	}

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

	MapConstantBuffer();
}

void Camera::UpdateModeGame()
{
	GetInputXbox();

	if (isShake)
	{
		CameraShake();
		shakeTime += 1 * pFrameWork.GetElapsedTime();
		if (shakeTime > shakeTotalTime)
		{
			shakeValue = 0.0f;
			pos.x = 0.0f;
			pos.z = 0.0f;
			isShake = false;
			shakeTotalTime = 0.0f;
			shakeTime = 0.0f;
		}
	}

	float x = target.x - pos.x;
	float y = target.y - pos.y;
	float z = target.z - pos.z;


	float xSin = sinf(rotateX);
	float xCos = cosf(rotateX);
	float ySin = sinf(rotateY);
	float yCos = cosf(rotateY);


	DirectX::XMVECTOR front = DirectX::XMVectorSet(-xCos * ySin, -xSin, -xCos * yCos, 0.0f);
	DirectX::XMVECTOR _right = DirectX::XMVectorSet(yCos, 0.0f, -ySin, 0.0f);
	DirectX::XMVECTOR _up = DirectX::XMVector3Cross(_right, front);

	DirectX::XMVECTOR _target = DirectX::XMLoadFloat3(&target);
	DirectX::XMStoreFloat3(&up, _up);
	DirectX::XMStoreFloat3(&right, _right);

	CreateView();

#ifdef _DEBUG
	ImGui::Begin("Camera");
	ImGui::Text("Camera##CameraPos : %f, %f,  %f", Camera::GetInstance().GetPos().x, Camera::GetInstance().GetPos().y, Camera::GetInstance().GetPos().z);
	ImGui::Text("Camera##Time : %f", elapsedTimeMag);
	ImGui::End();
#endif

}

void Camera::CreateView()
{
	DirectX::XMVECTOR _pos = DirectX::XMVectorSet(pos.x, pos.y, pos.z, 1.0f);
	DirectX::XMVECTOR _target = DirectX::XMVectorSet(target.x, target.y, target.z, 1.0f);
	DirectX::XMVECTOR _up = DirectX::XMVectorSet(up.x, up.y, up.z, 0.0f);

	view = DirectX::XMMatrixLookAtLH(_pos, _target, _up);
}

void Camera::CameraMove(DirectX::XMFLOAT3 oriPos, DirectX::XMFLOAT3 afterPos, DirectX::XMFLOAT3 oriTarget, DirectX::XMFLOAT3 afterTarget, float t, bool isRotate)
{
	float e = Easing::InSine(t, 2.0f, 1.0f, 0.0f);
	DirectX::XMFLOAT3 resPos = DirectX::XMFLOAT3(
		oriPos.x + ((afterPos.x - oriPos.x) * e),
		oriPos.y + ((afterPos.y - oriPos.y) * e),
		oriPos.z + ((afterPos.z - oriPos.z) * e)
	);
	DirectX::XMFLOAT3 resTarget = DirectX::XMFLOAT3(
		oriTarget.x + ((afterTarget.x - oriTarget.x) * e),
		oriTarget.y + ((afterTarget.y - oriTarget.y) * e),
		oriTarget.z + ((afterTarget.z - oriTarget.z) * e)
	);
	if (isRotate)
	{
		rotateY = 0.0f;
		rotateX = 90.0f * 0.01745f * e;
	}

	pos = resPos;
	target = resTarget;

}

void Camera::OnCameraShake(float t, float v)
{
	isShake = true;
	shakeTotalTime = t;
	shakeTime = 0.0f;
	shakeValue = v;
}

void Camera::OffCameraShake()
{
	isShake = false;
	shakeTotalTime = 0.0f;
	shakeValue = 0.0f;
	shakeTime = 0.0f;
	pos.x = 0.0f;
	pos.z = 0.0f;
}

void Camera::CameraShake()
{
	int r = rand() % 200 - 100;
	float _x = static_cast<float>(r) * shakeValue;
	r = rand() % 200 - 100;
	float _z = static_cast<float>(r) * shakeValue;

	pos.x = _x;
	pos.z = _z;
}

void Camera::MapConstantBuffer()
{
	// 定数バッファ設定
	{
		{
			// シーン用定数バッファ更新
			scene_constants cb;
			DirectX::XMStoreFloat4x4(&cb.rowView, GetViewMatrix());
			DirectX::XMStoreFloat4x4(&cb.rowProjection, GetProjectionMatrix());
			DirectX::XMStoreFloat4x4(&cb.rowViewProjection, GetViewProjectionMatrix());
			DirectX::XMStoreFloat4x4(&cb.view, GetViewMatrix());
			DirectX::XMStoreFloat4x4(&cb.projection, GetProjectionMatrix());
			DirectX::XMStoreFloat4x4(&cb.viewProjection, GetViewProjectionMatrix());
			cb.lightDirection = lightDirection;

			cb.cameraPos = { pos.x, pos.y, pos.z, 1.0f };
			cb.cameraTarget = { target.x, target.y, target.z, 1.0f };
			cb.cameraUp = { up.x, up.y, up.z, 1.0f };
			cb.cameraRight = { right.x, right.y, right.z, 1.0f };

			cb.elapsedTime.x = pFrameWork.GetElapsedTime();
			elapsedTimeMag += pFrameWork.GetElapsedTime();
			cb.elapsedTime.y = elapsedTimeMag;
			cb.elapsedTime.z = pFrameWork.GetElapsedTime();
			cb.elapsedTime.w = pFrameWork.GetElapsedTime();

			pFrameWork.GetContext()->UpdateSubresource(cb_scene.Get(), 0, 0, &cb, 0, 0);
		}
	}

}

void Camera::BindConstantBuffer(SceneShaderType type)
{
	if (SceneShaderType::VS == type)
	{
		pFrameWork.GetContext()->VSSetConstantBuffers(10, 1, Camera::GetInstance().cb_scene.GetAddressOf());
	}
	else if (SceneShaderType::PS == type)
	{
		pFrameWork.GetContext()->PSSetConstantBuffers(10, 1, Camera::GetInstance().cb_scene.GetAddressOf());
	}
	else if (SceneShaderType::CS == type)
	{
		pFrameWork.GetContext()->CSSetConstantBuffers(10, 1, Camera::GetInstance().cb_scene.GetAddressOf());
	}
	else
	{
		return;
	}
}

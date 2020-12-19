#include "Light.h"
#include "Camera.h"
#include <imgui.h>

void Light::Init()
{

	lightAngle = 4.16f;
	posY = -0.36f;
	Light::SetAmbient(DirectX::XMFLOAT3(2.0f, 2.0f, 2.0f));
	//ライト方向
	lightDir.x = sinf(lightAngle);
	lightDir.y = posY;
	lightDir.z = cosf(lightAngle);

}

void Light::Update()
{
	//if (GetKeyState('A') < 0) lightAngle -= 2.0f * 0.01745f;
	//if (GetKeyState('D') < 0) lightAngle += 2.0f * 0.01745f;
	//if (GetKeyState('W') < 0) posY -= 2.0f * 0.01745f;
	//if (GetKeyState('S') < 0) posY += 2.0f * 0.01745f;
	//SetAmbient(DirectX::XMFLOAT3(0.75f, 0.75f, 0.75f));
	
	//ライト方向
	lightDir.x = sinf(DirectX::XMConvertToRadians(lightAngle));
	lightDir.y = posY;
	lightDir.z = cosf(DirectX::XMConvertToRadians(lightAngle));
	SetDirLight(lightDir);

	DirectX::XMFLOAT4X4 _view = {};
	DirectX::XMStoreFloat4x4(&_view, Camera::GetInstance().GetViewMatrix());

	eyePos = {_view._41, _view._42, _view._43, 1.0f};

#ifdef _DEBUG

	ImGui();

#endif // _DEBUG


}

void Light::ImGui()
{
	ImGui::Begin("Light");
	ImGui::DragFloat("posY##Light", &posY);
	ImGui::DragFloat("angle##Light", &lightAngle);

	ImGui::ColorEdit4("Ambient##Light", &ambient.x);

	if (ambient.w != 0) ambient.w = 0;

	ImGui::ColorEdit4("LightColor##Light", &lightColor.x);

	ImGui::End();

}

void Light::SetDirLight(DirectX::XMFLOAT4 dir)
{
	float d = sqrtf(dir.x * dir.x + dir.y * dir.y + dir.z * dir.z);

	if (d > 0) { dir.x /= d, dir.y /= d, dir.z /= d; }
	lightDir = DirectX::XMFLOAT4(dir.x, dir.y, dir.z, 0);
}

void Light::SetAmbient(DirectX::XMFLOAT3 amb)
{
	ambient = { amb.x,amb.y,amb.z,0 };
}

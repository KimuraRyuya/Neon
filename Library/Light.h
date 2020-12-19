#pragma once

#include <d3d11.h>
#include <wrl.h>
#include <DirectXMath.h>

class Light
{
private:
	float lightAngle;
	float posY;
public:
	Light() {}
	~Light() {}

	static Light* GetInstance()
	{
		static Light instance;
		return &instance;
	}

	DirectX::XMFLOAT4 ambient			 = { 1.0f, 1.0f, 1.0f, 1.0f };
	DirectX::XMFLOAT4 lightDir			 = { 1.0f, 1.0f, 1.0f, 1.0f };
	DirectX::XMFLOAT4 lightColor		 = { 1.0f, 1.0f, 1.0f, 1.0f };
	DirectX::XMFLOAT4 eyePos			 = { 1.0f, 1.0f, 1.0f, 1.0f };

	void Init();
	void Update();

	void ImGui();

	void SetDirLight(DirectX::XMFLOAT4 dir);
	void SetAmbient(DirectX::XMFLOAT3 amb);
};
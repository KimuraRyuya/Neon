// UNIT.02
#pragma once

#include <d3d11.h>
#include <directxmath.h>
#include <wrl.h>
#include <memory>
#include "Shader.h"
#include "WICTextureLoader.h"
#include "Texture.h"
#include "FrameWork.h"
// UNIT.08
#include <string>


//スプライトインフォメーション
struct SpriteInf
{
	float sx;
	float sy;
	float sw;
	float sh;
};

enum ModeNo
{
	rect_solid = 0,
	rect_wire = 1,
	circle_solid = 2,
	circle_wire = 3,
	line = 4
};

class SpriteBatch
{
private:
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelDissolveShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> dissolveConstantBuffer;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerState;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> dissolveSRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> emissiveSRV;

	D3D11_TEXTURE2D_DESC texture2dDesc;
	D3D11_TEXTURE2D_DESC dissolveDesc;
	D3D11_TEXTURE2D_DESC emissiveDesc;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencilState;

	size_t MAX_INSTANCES = 10000;
	struct DissolveCBuffer
	{
		DirectX::XMFLOAT4 emissiveColor;
		float dissolveValue;
		float emissiveRange;
		float dummy01;
		float dummy02;
	};

	struct instance
	{
		DirectX::XMFLOAT4X4 ndcTransform;
		DirectX::XMFLOAT4 texcoordTransform;
		DirectX::XMFLOAT4 color;
	};
	Microsoft::WRL::ComPtr<ID3D11Buffer> instanceBuffer;

public:
	struct vertex_form
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT4 color;
	};
	struct vertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT2 texcoord;
	};
	SpriteBatch() {}
	SpriteBatch(ModeNo mode);
	void LineRender(float dx, float dy, float dw, float dh, float r, float g, float b, float a) const;
	void BoxRenderSolid(float dx, float dy, float dw, float dh, float angle/*degree*/, float r, float g, float b, float a) const;
	void BoxRenderWire(float dx, float dy, float dw, float dh, float angle/*degree*/, float r, float g, float b, float a) const;
	void CircleRenderSolid(float dx, float dy, float rad, float r, float g, float b, float a) const;
	void CircleRenderWire(float dx, float dy, float rad, float r, float g, float b, float a) const;
	SpriteBatch(const wchar_t *file_name,size_t max_instance = 512);
	void Begin();
	void BeginDissolve();
	void RenderDissolve(float dx, float dy, float dw, float dh, float angle/*degree*/, float r, float g, float b, float a, SpriteInf inf, float dissolve, float emissiveRange);
	void Render(float dx, float dy, float dw, float dh, float angle/*degree*/, float r, float g, float b, float a, SpriteInf  inf);
	void End();
	void DrawDissolve(float x, float y, float angle, float scale, SpriteInf  inf, float dissolve = 1.0f, float range = 1.0f, DirectX::XMFLOAT4 color = DirectX::XMFLOAT4(1, 1, 1, 1));
	void DrawRota(float x, float y, float angle, float scale, SpriteInf  inf, DirectX::XMFLOAT4 color = DirectX::XMFLOAT4(1, 1, 1, 1));
	void DrawRota2(float x, float y, float angle, float scale, SpriteInf  inf, DirectX::XMFLOAT4 color = DirectX::XMFLOAT4(1, 1, 1, 1));
	void DrawRota3(float x, float y, float angle, float scalex, float scaley, SpriteInf  inf, DirectX::XMFLOAT4 color = DirectX::XMFLOAT4(1, 1, 1, 1));
	
	void SpriteBatch::DissolveTextureLoad(const wchar_t* fileName)
	{
		Texture::LoadTextureFromFile(pFrameWork.GetDevice(), fileName, dissolveSRV.GetAddressOf(), &dissolveDesc);
	}

	void SpriteBatch::EmissiveTextureLoad(const wchar_t* fileName)
	{
		Texture::LoadTextureFromFile(pFrameWork.GetDevice(), fileName, emissiveSRV.GetAddressOf(), &emissiveDesc);
	}
private:
	D3D11_VIEWPORT viewport;

	UINT countInstance = 0;
	instance *instances = nullptr;
};


#pragma once

#include <d3d11.h>
#include <wrl.h>
#include <DirectXMath.h>
#include <string>
#include <vector>

#include "Shader.h"
#include "WICTextureLoader.h"
#include "Texture.h"
#include "FrameWork.h"

class Billboard
{
private:
	struct CBuffer
	{
		DirectX::XMFLOAT4X4	wvp;
		DirectX::XMFLOAT4 color;
		DirectX::XMFLOAT4 luminans;
		float angle;
		float width;
		float height;
		float dummy00;
	};	
	
	struct DissolveCBuffer
	{
		DirectX::XMFLOAT4 emissiveColor;
		float dissolveValue;
		float emissiveRange;
		float dummy01;
		float dummy02;
	};

	struct Vertex
	{
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT2 tex;
	};
	struct Subset
	{
		std::string usemtl;
		unsigned int indexStart = 0;
		unsigned int indexCount = 0;
	};

private:
	Microsoft::WRL::ComPtr<ID3D11VertexShader>		vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>		pixelShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>		pixelGaugeShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>		pixelGlitchShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>		pixelDissolveShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>		inputLayout;
	Microsoft::WRL::ComPtr<ID3D11Buffer>			constantBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer>			dissolveConstantBuffer;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState>	wireframeRasterizerState;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState>	solidRasterizerState;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencilState;
	Microsoft::WRL::ComPtr<ID3D11SamplerState>		samplerState;

	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;
	std::vector<Subset> subsets;
	D3D11_TEXTURE2D_DESC texture2dDesc;
	D3D11_TEXTURE2D_DESC dissolveDesc;
	D3D11_TEXTURE2D_DESC emissiveDesc;
	D3D11_TEXTURE2D_DESC randomNoiseDesc;
	D3D11_TEXTURE2D_DESC glitchDesc;

	wchar_t mapFileName[256];
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> dissolveSRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> emissiveSRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> randomNoiseSRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> glitchSRV;

	int numIndex = 0;

public:
	Billboard(const wchar_t* fileName)
	{

		// Create Vertex Shader and Input Layout *****************************************

		D3D11_INPUT_ELEMENT_DESC ieDesc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT   , 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		Descartes::CreateVSFromCso(pFrameWork.GetDevice(), "Shader/BillboardVS.cso", vertexShader.GetAddressOf(), inputLayout.GetAddressOf(), ieDesc, ARRAYSIZE(ieDesc));

		// *******************************************************************************

		// Create Pixel Shader ***********************************************************

		Descartes::CreatePSFromCso(pFrameWork.GetDevice(), "Shader/BillboardPS.cso", pixelShader.GetAddressOf());
		Descartes::CreatePSFromCso(pFrameWork.GetDevice(), "Shader/BillboardGaugePS.cso", pixelGaugeShader.GetAddressOf());
		Descartes::CreatePSFromCso(pFrameWork.GetDevice(), "Shader/BillboardGritchPS.cso", pixelGlitchShader.GetAddressOf());
		Descartes::CreatePSFromCso(pFrameWork.GetDevice(), "Shader/BillboardDissolve.cso", pixelDissolveShader.GetAddressOf());

		// *******************************************************************************

		Init(pFrameWork.GetDevice());

		for (int i = 0; i < 256; i++)
		{
			mapFileName[i] = fileName[i];
		}

		CreateShaderResourceView(pFrameWork.GetDevice(), fileName);

	}
	~Billboard() {}

	void Init(ID3D11Device* device);
	void Begin();
	void Begin2();
	void BeginDissolve();
	void BeginGlitch();
	void Render
	(
		const DirectX::XMFLOAT3& position,
		float tx,
		float ty,
		float tw,
		float th,
		const DirectX::XMFLOAT3& angle,
		const DirectX::XMFLOAT2& scale,
		const DirectX::XMFLOAT4& luminans,
		const DirectX::XMFLOAT4& color,
		bool billBoard = true
	);
	void RenderGauge
	(
		const DirectX::XMFLOAT3& position,
		float tx,
		float ty,
		float tw,
		float th,
		float angleGauge,
		const DirectX::XMFLOAT3& angle,
		const DirectX::XMFLOAT2& scale,
		const DirectX::XMFLOAT4& luminans,
		const DirectX::XMFLOAT4& color,
		bool billBoard = true
	);
	void RenderDissolve
	(
		const DirectX::XMFLOAT3& position,
		float tx,
		float ty,
		float tw,
		float th,
		float dissolve,
		const DirectX::XMFLOAT3& angle,
		const DirectX::XMFLOAT2& scale,
		const DirectX::XMFLOAT4& luminans,
		const DirectX::XMFLOAT4& emissiveColor,
		const DirectX::XMFLOAT4& color,
		float emissiveRange = 1.0f,
		bool billBoard = true
	);	
	void RenderGlitch
	(
		const DirectX::XMFLOAT3& position,
		float tx,
		float ty,
		float tw,
		float th,
		float dissolve,
		const DirectX::XMFLOAT3& angle,
		const DirectX::XMFLOAT2& scale,
		const DirectX::XMFLOAT4& luminans,
		const DirectX::XMFLOAT4& emissiveColor,
		const DirectX::XMFLOAT4& color,
		float emissiveRange = 1.0f,
		bool billBoard = true
	);
	void End();

	void DissolveTextureLoad(const wchar_t* fileName = L"Data/Texture/dissolve_animation1.png")
	{
		Texture::LoadTextureFromFile(pFrameWork.GetDevice(), fileName, dissolveSRV.GetAddressOf(), &dissolveDesc);
	}
	void EmissiveTextureLoad(const wchar_t* fileName = L"Data/Texture/dissolve_edgecolor.png")
	{
		Texture::LoadTextureFromFile(pFrameWork.GetDevice(), fileName, emissiveSRV.GetAddressOf(), &emissiveDesc);
	}

private:
	//void CreateBoard( ID3D11Device* device );
	void CreateBuffers(ID3D11Device* device, Vertex* vertices, int numVertices, u_int* indices, int numIndices);
	void CreateShaderResourceView(ID3D11Device* device, const wchar_t* fileName)
	{
		Texture::LoadTextureFromFile(device, fileName, shaderResourceView.GetAddressOf(), &texture2dDesc);	
		Texture::LoadTextureFromFile(device, L"Data/Texture/randamNoise.png", randomNoiseSRV.GetAddressOf(), &randomNoiseDesc);	
		Texture::LoadTextureFromFile(device, L"Data/Texture/glitch.png", glitchSRV.GetAddressOf(), &glitchDesc);	
	}
};


#include <d3dcompiler.h>
#ifdef _MSC_VER
#pragma comment(lib, "d3dcompiler") 
#endif

#include "Rasterizer.h"
#include "Misc.h"
#include "Shader.h"
#include "Texture.h"
#include <DirectXMath.h>

namespace Descartes
{
	Rasterizer::Rasterizer(ID3D11Device* device, D3D11_FILTER samplerFilter, D3D11_TEXTURE_ADDRESS_MODE samplerTextureAddressMode, DirectX::XMFLOAT4 samplerBoarderColour)
	{
		HRESULT hr = S_OK;

		vertex vertices[] =
		{
			{ DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT2(0, 0), DirectX::XMFLOAT4(1, 1, 1, 1) },
			{ DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT2(0, 0), DirectX::XMFLOAT4(1, 1, 1, 1) },
			{ DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT2(0, 0), DirectX::XMFLOAT4(1, 1, 1, 1) },
			{ DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT2(0, 0), DirectX::XMFLOAT4(1, 1, 1, 1) },
		};

		D3D11_BUFFER_DESC bufferDesc = {};
		bufferDesc.ByteWidth = sizeof(vertices);
		bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bufferDesc.MiscFlags = 0;
		bufferDesc.StructureByteStride = 0;
		D3D11_SUBRESOURCE_DATA subresourceData = {};
		subresourceData.pSysMem = vertices;
		subresourceData.SysMemPitch = 0;
		subresourceData.SysMemSlicePitch = 0;
		hr = device->CreateBuffer(&bufferDesc, &subresourceData, vertexBuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		const std::string rasterizerVS =
			"struct VS_OUT\n"
			"{\n"
			"	float4 position : SV_POSITION;\n"
			"	float2 texcoord : TEXCOORD;\n"
			"	float4 colour : COLOR;\n"
			"};\n"
			"VS_OUT VS(float4 position : POSITION, float2 texcoord : TEXCOORD, float4 colour : COLOR)\n"
			"{\n"
			"	VS_OUT vout;\n"
			"	vout.position = position;\n"
			"	vout.texcoord = texcoord;\n"
			"	vout.colour = colour;\n"
			"	return vout;\n"
			"}\n";
		static const D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		CreateVSFromSource(device, "rasterizer_vs", rasterizerVS, embeddedVertexShader.ReleaseAndGetAddressOf(), "VS", "vs_5_0",
			embeddedInputLayout.ReleaseAndGetAddressOf(), const_cast<D3D11_INPUT_ELEMENT_DESC*>(inputElementDesc), _countof(inputElementDesc));

		const std::string rasterizerPS =
			"Texture2D texture_map : register( t0 );\n"
			"SamplerState texture_map_sampler_state : register( s0 );\n"
			"struct VS_OUT\n"
			"{\n"
			"	float4 position : SV_POSITION;\n"
			"	float2 texcoord : TEXCOORD;\n"
			"	float4 colour : COLOR;\n"
			"};\n"
			"float4 PS(VS_OUT pin) : SV_TARGET\n"
			"{\n"
			"	return texture_map.Sample(texture_map_sampler_state, pin.texcoord) * pin.colour;\n"
			"}\n";
		CreatePSFromSource(device, "rasterizer_ps", rasterizerPS, embeddedPixelShaders[0].ReleaseAndGetAddressOf(), "PS");

		const std::string rasterizerPSMS =
			"Texture2DMS<float4> texture_map : register(t0);\n"
			"struct VS_OUT\n"
			"{\n"
			"	float4 position : SV_POSITION;\n"
			"	float2 texcoord : TEXCOORD;\n"
			"	float4 colour : COLOR;\n"
			"};\n"
			"float4 PS(VS_OUT pin, uint sample_index : SV_SAMPLEINDEX) : SV_TARGET\n"
			"{\n"
			"	uint width, height, number_of_samples;\n"
			"	texture_map.GetDimensions(width, height, number_of_samples);\n"
			"	return texture_map.Load(uint2(pin.texcoord.x * width, pin.texcoord.y * height), sample_index) * pin.colour;\n"
			"}\n";
		CreatePSFromSource(device, "rasterizer_ps_ms", rasterizerPSMS, embeddedPixelShaders[1].ReleaseAndGetAddressOf(), "PS");

		D3D11_SAMPLER_DESC samplerDesc;
		samplerDesc.Filter = samplerFilter; //D3D11_FILTER_MIN_MAG_MIP_POINT D3D11_FILTER_ANISOTROPIC
		samplerDesc.AddressU = samplerTextureAddressMode;
		samplerDesc.AddressV = samplerTextureAddressMode;
		samplerDesc.AddressW = samplerTextureAddressMode;
		samplerDesc.MipLODBias = 0;
		samplerDesc.MaxAnisotropy = 16;
		samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		memcpy(samplerDesc.BorderColor, &samplerBoarderColour, sizeof(DirectX::XMFLOAT4));
		samplerDesc.MinLOD = 0;
		samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
		hr = device->CreateSamplerState(&samplerDesc, embeddedSamplerState.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		D3D11_RASTERIZER_DESC rasterizerDesc = {};
		rasterizerDesc.FillMode = D3D11_FILL_SOLID;
		rasterizerDesc.CullMode = D3D11_CULL_NONE;
		rasterizerDesc.FrontCounterClockwise = FALSE;
		rasterizerDesc.DepthBias = 0;
		rasterizerDesc.DepthBiasClamp = 0;
		rasterizerDesc.SlopeScaledDepthBias = 0;
		rasterizerDesc.DepthClipEnable = FALSE;
		rasterizerDesc.ScissorEnable = FALSE;
		rasterizerDesc.MultisampleEnable = FALSE;
		rasterizerDesc.AntialiasedLineEnable = FALSE;
		hr = device->CreateRasterizerState(&rasterizerDesc, embeddedRasterizerState.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
		depthStencilDesc.DepthEnable = FALSE;
		depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		depthStencilDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;
		depthStencilDesc.StencilEnable = FALSE;
		depthStencilDesc.StencilReadMask = 0xFF;
		depthStencilDesc.StencilWriteMask = 0xFF;
		depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
		depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
		depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		hr = device->CreateDepthStencilState(&depthStencilDesc, embeddedDepthStencilState.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}
	void Rasterizer::Blit
	(
		ID3D11DeviceContext* immediateContext,
		ID3D11ShaderResourceView* shaderResourceView,
		float dx, float dy, float dw, float dh,
		float sx, float sy, float sw, float sh,
		float angle/*degree*/,
		float r, float g, float b, float a,
		bool useEmbeddedVertexShader,
		bool useEmbeddedPixelShader,
		bool useEmbeddedRasterizerState,
		bool useEmbeddedDepthStencilState,
		bool useEmbeddedSamplerState
	) const
		// dx, dy : Coordinate of sprite's left-top corner in screen space 
		// dw, dh : Size of sprite in screen space 
		// angle : Raotation angle (Rotation centre is sprite's centre), unit is degree
		// r, g, b : Color of sprite's each vertices 
	{
		HRESULT hr = S_OK;

		D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc = {};
		shaderResourceView->GetDesc(&shaderResourceViewDesc);
		bool multisampled = shaderResourceViewDesc.ViewDimension == D3D11_SRV_DIMENSION_TEXTURE2DMS;

		Microsoft::WRL::ComPtr<ID3D11Resource> resource;
		shaderResourceView->GetResource(resource.GetAddressOf());

		Microsoft::WRL::ComPtr<ID3D11Texture2D> texture2d;
		hr = resource.Get()->QueryInterface<ID3D11Texture2D>(texture2d.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		D3D11_TEXTURE2D_DESC texture2dDesc;
		texture2d->GetDesc(&texture2dDesc);

		D3D11_VIEWPORT viewport;
		UINT numViewports = 1;
		immediateContext->RSGetViewports(&numViewports, &viewport);
		float screenWidth = viewport.Width;
		float screenHeight = viewport.Height;

		// Set each sprite's vertices coordinate to screen spaceenum BLEND_STATE
		// left-top
		float x0 = dx;
		float y0 = dy;
		// right-top
		float x1 = dx + dw;
		float y1 = dy;
		// left-bottom
		float x2 = dx;
		float y2 = dy + dh;
		// right-bottom
		float x3 = dx + dw;
		float y3 = dy + dh;

		// Translate sprite's centre to origin (rotate centre)
		float mx = dx + dw * 0.5f;
		float my = dy + dh * 0.5f;
		x0 -= mx;
		y0 -= my;
		x1 -= mx;
		y1 -= my;
		x2 -= mx;
		y2 -= my;
		x3 -= mx;
		y3 -= my;

		// Rotate each sprite's vertices by angle
		float rx, ry;
		float cos = cosf(angle * 0.01745f);
		float sin = sinf(angle * 0.01745f);
		rx = x0;
		ry = y0;
		x0 = cos * rx + -sin * ry;
		y0 = sin * rx + cos * ry;
		rx = x1;
		ry = y1;
		x1 = cos * rx + -sin * ry;
		y1 = sin * rx + cos * ry;
		rx = x2;
		ry = y2;
		x2 = cos * rx + -sin * ry;
		y2 = sin * rx + cos * ry;
		rx = x3;
		ry = y3;
		x3 = cos * rx + -sin * ry;
		y3 = sin * rx + cos * ry;

		// Translate sprite's centre to original position
		x0 += mx;
		y0 += my;
		x1 += mx;
		y1 += my;
		x2 += mx;
		y2 += my;
		x3 += mx;
		y3 += my;

		// Convert to NDC space
		x0 = 2.0f * x0 / screenWidth - 1.0f;
		y0 = 1.0f - 2.0f * y0 / screenHeight;
		x1 = 2.0f * x1 / screenWidth - 1.0f;
		y1 = 1.0f - 2.0f * y1 / screenHeight;
		x2 = 2.0f * x2 / screenWidth - 1.0f;
		y2 = 1.0f - 2.0f * y2 / screenHeight;
		x3 = 2.0f * x3 / screenWidth - 1.0f;
		y3 = 1.0f - 2.0f * y3 / screenHeight;

		D3D11_MAP map = D3D11_MAP_WRITE_DISCARD;
		D3D11_MAPPED_SUBRESOURCE mappedBuffer;
		hr = immediateContext->Map(vertexBuffer.Get(), 0, map, 0, &mappedBuffer);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		vertex* vertices = static_cast<vertex*>(mappedBuffer.pData);
		vertices[0].position.x = x0;
		vertices[0].position.y = y0;
		vertices[1].position.x = x1;
		vertices[1].position.y = y1;
		vertices[2].position.x = x2;
		vertices[2].position.y = y2;
		vertices[3].position.x = x3;
		vertices[3].position.y = y3;
		vertices[0].position.z = vertices[1].position.z = vertices[2].position.z = vertices[3].position.z = 0.0f;

		DirectX::XMFLOAT4 colour(r, g, b, a);
		vertices[0].colour = vertices[1].colour = vertices[2].colour = vertices[3].colour = colour;

		vertices[0].texcoord.x = (sx) / texture2dDesc.Width;
		vertices[0].texcoord.y = (sy) / texture2dDesc.Height;
		vertices[1].texcoord.x = (sx + sw) / texture2dDesc.Width;
		vertices[1].texcoord.y = (sy) / texture2dDesc.Height;
		vertices[2].texcoord.x = (sx) / texture2dDesc.Width;
		vertices[2].texcoord.y = (sy + sh) / texture2dDesc.Height;
		vertices[3].texcoord.x = (sx + sw) / texture2dDesc.Width;
		vertices[3].texcoord.y = (sy + sh) / texture2dDesc.Height;

		immediateContext->Unmap(vertexBuffer.Get(), 0);

		UINT stride = sizeof(vertex);
		UINT offset = 0;
		immediateContext->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);
		immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		immediateContext->IASetInputLayout(embeddedInputLayout.Get());

		if (useEmbeddedVertexShader)
		{
			immediateContext->VSSetShader(embeddedVertexShader.Get(), 0, 0);
		}
		if (useEmbeddedPixelShader)
		{
			immediateContext->PSSetShader(multisampled ? embeddedPixelShaders[1].Get() : embeddedPixelShaders[0].Get(), 0, 0);
		}
		immediateContext->PSSetShaderResources(0, 1, &shaderResourceView);

		Microsoft::WRL::ComPtr<ID3D11RasterizerState> cachedRasterizerState;
		if (useEmbeddedRasterizerState)
		{
			immediateContext->RSGetState(cachedRasterizerState.ReleaseAndGetAddressOf());
			immediateContext->RSSetState(embeddedRasterizerState.Get());
		}
		Microsoft::WRL::ComPtr<ID3D11DepthStencilState> cachedDepthStencilState;
		if (useEmbeddedDepthStencilState)
		{
			immediateContext->OMGetDepthStencilState(cachedDepthStencilState.ReleaseAndGetAddressOf(), 0);
			immediateContext->OMSetDepthStencilState(embeddedDepthStencilState.Get(), 1);
		}
		Microsoft::WRL::ComPtr<ID3D11SamplerState> cachedSamplerState;
		if (useEmbeddedSamplerState)
		{
			immediateContext->PSGetSamplers(0, 1, cachedSamplerState.ReleaseAndGetAddressOf());
			immediateContext->PSSetSamplers(0, 1, embeddedSamplerState.GetAddressOf());
		}
		immediateContext->Draw(4, 0);

		immediateContext->IASetInputLayout(0);
		immediateContext->VSSetShader(0, 0, 0);
		immediateContext->PSSetShader(0, 0, 0);

		ID3D11ShaderResourceView* nullShaderResourceView = 0;
		immediateContext->PSSetShaderResources(0, 1, &nullShaderResourceView);

		if (cachedRasterizerState)
		{
			immediateContext->RSSetState(cachedRasterizerState.Get());
		}
		if (cachedDepthStencilState)
		{
			immediateContext->OMSetDepthStencilState(cachedDepthStencilState.Get(), 1);
		}
		if (cachedSamplerState)
		{
			immediateContext->PSSetSamplers(0, 1, cachedSamplerState.GetAddressOf());
		}
	}
	void Rasterizer::Blit
	(
		ID3D11DeviceContext* immediateContext,
		ID3D11ShaderResourceView* shaderResourceView,
		float dx, float dy, float dw, float dh,
		float angle/*degree*/,
		float r, float g, float b, float a,
		bool useEmbeddedVertexShader,
		bool useEmbeddedPixelShader,
		bool useEmbeddedRasterizerState,
		bool useEmbeddedDepthStencilState,
		bool useEmbeddedSamplerState
	) const
	{
		HRESULT hr = S_OK;

		Microsoft::WRL::ComPtr<ID3D11Resource> resource;
		shaderResourceView->GetResource(resource.GetAddressOf());

		Microsoft::WRL::ComPtr<ID3D11Texture2D> texture2d;
		hr = resource.Get()->QueryInterface<ID3D11Texture2D>(texture2d.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		D3D11_TEXTURE2D_DESC texture2dDesc;
		texture2d->GetDesc(&texture2dDesc);

		Blit(immediateContext, shaderResourceView, dx, dy, dw, dh, 0.0f, 0.0f, static_cast<float>(texture2dDesc.Width), static_cast<float>(texture2dDesc.Height), angle, r, g, b, a,
			useEmbeddedVertexShader, useEmbeddedPixelShader, useEmbeddedRasterizerState, useEmbeddedDepthStencilState, useEmbeddedSamplerState);
	}

	FullscreenQuad::FullscreenQuad(ID3D11Device* device)
	{
		HRESULT hr = S_OK;

		const std::string fullscreenQuadVS =
			"struct VS_OUT\n"
			"{\n"
			"	float4 position : SV_POSITION;\n"
			"	float2 texcoord : TEXCOORD;\n"
			"};\n"
			"VS_OUT VS(in uint vertex_id : SV_VERTEXID)\n"
			"{\n"
			"	VS_OUT vout;\n"
			"	vout.texcoord = float2((vertex_id << 1) & 2, vertex_id & 2);\n"
			"	vout.position = float4(vout.texcoord * float2(2.0f, -2.0f) + float2(-1.0f, 1.0f), 0.0f, 1.0f);\n"
			"	return vout;\n"
			"}\n";
		CreateVSFromSource(device, "fullscreen_quad_vs", fullscreenQuadVS, embeddedVertexShader.ReleaseAndGetAddressOf(), "VS", "vs_5_0", 0, 0, 0);

		const std::string fullscreenQuadPS =
			"Texture2D texture_map : register( t0 );\n"
			"SamplerState texture_map_sampler_state : register( s0 );\n"
			"struct VS_OUT\n"
			"{\n"
			"	float4 position : SV_POSITION;\n"
			"	float2 texcoord : TEXCOORD;\n"
			"};\n"
			"float4 PS(VS_OUT pin) : SV_TARGET\n"
			"{\n"
			"	return texture_map.Sample(texture_map_sampler_state, pin.texcoord);\n"
			"}\n";
		CreatePSFromSource(device, "fullscreen_quad_ps", fullscreenQuadPS, embeddedPixelShaders[0].ReleaseAndGetAddressOf(), "PS");

		const std::string fullscreenQuadPSMS =
			"Texture2DMS<float4> texture_map : register(t0);\n"
			"struct VS_OUT\n"
			"{\n"
			"	float4 position : SV_POSITION;\n"
			"	float2 texcoord : TEXCOORD;\n"
			"};\n"
			"float4 PS(VS_OUT pin, uint sample_index : SV_SAMPLEINDEX) : SV_TARGET\n"
			"{\n"
			"	uint width, height, number_of_samples;\n"
			"	texture_map.GetDimensions(width, height, number_of_samples);\n"
			"	return texture_map.Load(uint2(pin.texcoord.x * width, pin.texcoord.y * height), sample_index);\n"
			"}\n";
		CreatePSFromSource(device, "fullscreen_quad_ps_ms", fullscreenQuadPSMS, embeddedPixelShaders[1].ReleaseAndGetAddressOf(), "PS");

		D3D11_RASTERIZER_DESC rasterizerDesc = {};
		rasterizerDesc.FillMode = D3D11_FILL_SOLID;
		rasterizerDesc.CullMode = D3D11_CULL_BACK;
		rasterizerDesc.FrontCounterClockwise = FALSE;
		rasterizerDesc.DepthBias = 0;
		rasterizerDesc.DepthBiasClamp = 0;
		rasterizerDesc.SlopeScaledDepthBias = 0;
		rasterizerDesc.DepthClipEnable = FALSE;
		rasterizerDesc.ScissorEnable = FALSE;
		rasterizerDesc.MultisampleEnable = FALSE;
		rasterizerDesc.AntialiasedLineEnable = FALSE;
		hr = device->CreateRasterizerState(&rasterizerDesc, embeddedRasterizerState.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
		depthStencilDesc.DepthEnable = FALSE;
		depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		depthStencilDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;
		depthStencilDesc.StencilEnable = FALSE;
		depthStencilDesc.StencilReadMask = 0xFF;
		depthStencilDesc.StencilWriteMask = 0xFF;
		depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
		depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
		depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		hr = device->CreateDepthStencilState(&depthStencilDesc, embeddedDepthStencilState.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}
	void FullscreenQuad::Blit(ID3D11DeviceContext* immediateContext, bool useEmbeddedRasterizerState, bool useEmbeddedDepthStencilState, bool useEmbeddedPixelShader, bool enableMsaa)
	{
		Microsoft::WRL::ComPtr<ID3D11RasterizerState> cachedRasterizerState;
		if (useEmbeddedRasterizerState)
		{
			immediateContext->RSGetState(cachedRasterizerState.ReleaseAndGetAddressOf());
			immediateContext->RSSetState(embeddedRasterizerState.Get());
		}

		Microsoft::WRL::ComPtr<ID3D11DepthStencilState> cachedDepthStencilState;
		if (useEmbeddedDepthStencilState)
		{
			immediateContext->OMGetDepthStencilState(cachedDepthStencilState.ReleaseAndGetAddressOf(), 0);
			immediateContext->OMSetDepthStencilState(embeddedDepthStencilState.Get(), 1);
		}
		if (useEmbeddedPixelShader)
		{
			immediateContext->PSSetShader(enableMsaa ? embeddedPixelShaders[1].Get() : embeddedPixelShaders[0].Get(), 0, 0);
		}

		immediateContext->IASetVertexBuffers(0, 0, 0, 0, 0);
		immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		immediateContext->IASetInputLayout(0);

		immediateContext->VSSetShader(embeddedVertexShader.Get(), 0, 0);

		immediateContext->Draw(4, 0);

		//immediate_context->IASetInputLayout(0);
		immediateContext->VSSetShader(0, 0, 0);
		immediateContext->PSSetShader(0, 0, 0);

		if (cachedRasterizerState)
		{
			immediateContext->RSSetState(cachedRasterizerState.Get());
		}
		if (cachedDepthStencilState)
		{
			immediateContext->OMSetDepthStencilState(cachedDepthStencilState.Get(), 1);
		}
	}
}
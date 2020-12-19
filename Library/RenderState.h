#pragma once

#include <d3d11.h>
#include <wrl.h>
#include <cstdint>
#include "misc.h"

enum class BLEND_MODE { NONE, ALPHA, ALPHA2, ADD, SUBTRACT, REPLACE, MULTIPLY, LIGHTEN, DARKEN, SCREEN, ALPHA_TO_COVERAGE, END };

namespace Descartes
{
	ID3D11BlendState* CreateBlendState(ID3D11Device* device, BLEND_MODE mode);

	class BlendState
	{
#ifdef _DEBUG
#define ACTIVATE(x) ++(x)
#define DEACTIVATE(x) --(x)
		int watchdog = 0;
#else
#define ACTIVATE(x) ((void)0)
#define DEACTIVATE(x) ((void)0)
#endif
	public:
		Microsoft::WRL::ComPtr<ID3D11BlendState> stateObject;
		BlendState(ID3D11Device* device, BLEND_MODE mode)
		{
			stateObject.Attach(CreateBlendState(device, mode));
		}
		BlendState(ID3D11Device* device, const D3D11_BLEND_DESC* blendDesc)
		{
			HRESULT hr = device->CreateBlendState(blendDesc, stateObject.GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		}
		~BlendState() = default;
		BlendState(BlendState&) = delete;
		BlendState& operator =(BlendState&) = delete;

		void Activate(ID3D11DeviceContext* immediateContext)
		{
			ACTIVATE(watchdog);
			UINT sampleMask = 0xFFFFFFFF;
			immediateContext->OMGetBlendState(cachedStateObject.ReleaseAndGetAddressOf(), 0, &sampleMask);
			immediateContext->OMSetBlendState(stateObject.Get(), 0, 0xFFFFFFFF);
		}
		void DeActivate(ID3D11DeviceContext* immediateContext)
		{
			DEACTIVATE(watchdog);
			immediateContext->OMSetBlendState(cachedStateObject.Get(), 0, 0xFFFFFFFF);
		}
	private:
		Microsoft::WRL::ComPtr<ID3D11BlendState> cachedStateObject;
	};

	class RasterizerState
	{
#ifdef _DEBUG
#define ACTIVATE(x) ++(x)
#define DEACTIVATE(x) --(x)
		int watchdog = 0;
#else
#define ACTIVATE(x) ((void)0)
#define DEACTIVATE(x) ((void)0)
#endif
	public:
		Microsoft::WRL::ComPtr<ID3D11RasterizerState> stateObject;
		RasterizerState(ID3D11Device* device, D3D11_FILL_MODE fillMode = D3D11_FILL_SOLID, D3D11_CULL_MODE cullMode = D3D11_CULL_BACK, bool frontCounterClockwise = true, bool antialiasing = true, bool depthClipEnable = true)
		{
			D3D11_RASTERIZER_DESC rasterizerDesc = {};
			rasterizerDesc.FillMode = fillMode;
			rasterizerDesc.CullMode = cullMode;
			rasterizerDesc.FrontCounterClockwise = frontCounterClockwise;
			rasterizerDesc.DepthBias = D3D11_DEFAULT_DEPTH_BIAS;
			rasterizerDesc.DepthBiasClamp = D3D11_DEFAULT_DEPTH_BIAS_CLAMP;
			rasterizerDesc.SlopeScaledDepthBias = D3D11_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
			rasterizerDesc.DepthClipEnable = static_cast<BOOL>(depthClipEnable);
			rasterizerDesc.ScissorEnable = FALSE;
			rasterizerDesc.MultisampleEnable = antialiasing && fillMode == D3D11_FILL_SOLID ? TRUE : FALSE;
			rasterizerDesc.AntialiasedLineEnable = antialiasing ? TRUE : FALSE;
			HRESULT hr = device->CreateRasterizerState(&rasterizerDesc, stateObject.GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		}
		RasterizerState(ID3D11Device* device, const D3D11_RASTERIZER_DESC* rasterizerDesc)
		{
			HRESULT hr = device->CreateRasterizerState(rasterizerDesc, stateObject.GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		}
		~RasterizerState() = default;
		RasterizerState(RasterizerState&) = delete;
		RasterizerState& operator =(RasterizerState&) = delete;

		void Activate(ID3D11DeviceContext* immediateContext)
		{
			ACTIVATE(watchdog);
			immediateContext->RSGetState(cachedStateObject.ReleaseAndGetAddressOf());
			immediateContext->RSSetState(stateObject.Get());
		}
		void DeActivate(ID3D11DeviceContext* immediateContext)
		{
			DEACTIVATE(watchdog);
			immediateContext->RSSetState(cachedStateObject.Get());
		}
	private:
		Microsoft::WRL::ComPtr<ID3D11RasterizerState> cachedStateObject;
	};

	class DepthStencilState
	{
#ifdef _DEBUG
#define ACTIVATE(x) ++(x)
#define DEACTIVATE(x) --(x)
		int watchdog = 0;
#else
#define ACTIVATE(x) ((void)0)
#define DEACTIVATE(x) ((void)0)
#endif
	public:
		Microsoft::WRL::ComPtr<ID3D11DepthStencilState> stateObject;
		DepthStencilState(ID3D11Device* device, BOOL depthEnable = TRUE, D3D11_DEPTH_WRITE_MASK depthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL, D3D11_COMPARISON_FUNC depthFunc = D3D11_COMPARISON_LESS)
		{
			D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
			depthStencilDesc.DepthEnable = depthEnable;
			depthStencilDesc.DepthWriteMask = depthWriteMask;
			depthStencilDesc.DepthFunc = depthFunc;
			depthStencilDesc.StencilEnable = FALSE;
			depthStencilDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
			depthStencilDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
			depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
			depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
			depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
			depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
			depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
			depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
			depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
			depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
			HRESULT hr = device->CreateDepthStencilState(&depthStencilDesc, stateObject.GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		}
		DepthStencilState(ID3D11Device* device, const D3D11_DEPTH_STENCIL_DESC* depthStencilDesc)
		{
			HRESULT hr = device->CreateDepthStencilState(depthStencilDesc, stateObject.GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		}
		~DepthStencilState() = default;
		DepthStencilState(RasterizerState&) = delete;
		DepthStencilState& operator =(DepthStencilState&) = delete;

		void Activate(ID3D11DeviceContext* immediateContext)
		{
			ACTIVATE(watchdog);
			immediateContext->OMGetDepthStencilState(cachedStateObject.ReleaseAndGetAddressOf(), 0);
			immediateContext->OMSetDepthStencilState(stateObject.Get(), 1);
		}
		void DeActivate(ID3D11DeviceContext* immediateContext)
		{
			DEACTIVATE(watchdog);
			immediateContext->OMSetDepthStencilState(cachedStateObject.Get(), 1);
		}
	private:
		Microsoft::WRL::ComPtr<ID3D11DepthStencilState> cachedStateObject;
	};

	class SamplerState
	{
#ifdef _DEBUG
#define ACTIVATE(x) ++(x)
#define DEACTIVATE(x) --(x)
		int watchdog = 0;
#else
#define ACTIVATE(x) ((void)0)
#define DEACTIVATE(x) ((void)0)
#endif
	public:
		Microsoft::WRL::ComPtr<ID3D11SamplerState> stateObject;
		SamplerState(ID3D11Device* device, D3D11_FILTER filter = D3D11_FILTER_ANISOTROPIC, D3D11_TEXTURE_ADDRESS_MODE addressMode = D3D11_TEXTURE_ADDRESS_WRAP, D3D11_COMPARISON_FUNC comparisonFunc = D3D11_COMPARISON_NEVER, float r = 1, float g = 1, float b = 1, float a = 1)
		{
			//4096 unique sampler state objects can be created on a device at a time.
			//If an application attempts to create a sampler - state interface with the same state as an existing interface,
			//the same interface will be returned and the total number of unique sampler state objects will stay the same.
			D3D11_SAMPLER_DESC samplerDesc = {};
			samplerDesc.Filter = filter;
			samplerDesc.AddressU = addressMode;
			samplerDesc.AddressV = addressMode;
			samplerDesc.AddressW = addressMode;
			samplerDesc.MipLODBias = 0;
			samplerDesc.MaxAnisotropy = 16;
			samplerDesc.ComparisonFunc = comparisonFunc;
			float borderColour[4] = { r, g, b, a };
			memcpy(samplerDesc.BorderColor, borderColour, sizeof(float) * 4);
			samplerDesc.MinLOD = 0;
			samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
			HRESULT hr = device->CreateSamplerState(&samplerDesc, stateObject.GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		}
		SamplerState(ID3D11Device* device, const D3D11_SAMPLER_DESC* samplerDesc)
		{
			HRESULT hr = device->CreateSamplerState(samplerDesc, stateObject.GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		}
		~SamplerState() = default;
		SamplerState(SamplerState&) = delete;
		SamplerState& operator =(SamplerState&) = delete;

		void Activate(ID3D11DeviceContext* immediateContext, UINT slot, bool setInVs = false)
		{
			ACTIVATE(watchdog);
			usingSlot = slot;
			immediateContext->PSGetSamplers(usingSlot, 1, cachedStateObjects[0].ReleaseAndGetAddressOf());
			immediateContext->VSSetSamplers(usingSlot, 1, stateObject.GetAddressOf());

			immediateContext->PSSetSamplers(usingSlot, 1, stateObject.GetAddressOf());
			if (setInVs)
			{
				immediateContext->VSGetSamplers(usingSlot, 1, cachedStateObjects[1].ReleaseAndGetAddressOf());
			}
		}
		void DeActivate(ID3D11DeviceContext* immediate_context)
		{
			DEACTIVATE(watchdog);
			immediate_context->PSSetSamplers(usingSlot, 1, cachedStateObjects[0].GetAddressOf());
			immediate_context->VSSetSamplers(usingSlot, 1, cachedStateObjects[1].GetAddressOf());
		}
	private:
		UINT usingSlot = 0;
		Microsoft::WRL::ComPtr<ID3D11SamplerState> cachedStateObjects[2];
	};
}



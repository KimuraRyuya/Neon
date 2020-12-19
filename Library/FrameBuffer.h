#pragma once

#include <d3d11.h>
#include <wrl.h>
#include <assert.h>
#include <memory>
#include "misc.h"
#include "Rasterizer.h"
#include "Shader.h"

namespace Descartes
{
	class FrameBuffer
	{
	public:
		//'render_target_texture2d_format' : DXGI_FORMAT_R8G8B8A8_UNORM / DXGI_FORMAT_R16G16B16A16_FLOAT / DXGI_FORMAT_R32G32B32A32_FLOAT / DXGI_FORMAT_UNKNOWN(not create)
		//'depth_stencil_texture2d_format' : DXGI_FORMAT_R24G8_TYPELESS / DXGI_FORMAT_R32_TYPELESS / DXGI_FORMAT_R16_TYPELESS / DXGI_FORMAT_UNKNOWN(not create)
		FrameBuffer(ID3D11Device* device, std::uint32_t width, std::uint32_t height, bool enableMsaa = false, int subsamples = 1, DXGI_FORMAT renderTargetTexture2dFormat = DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT DepthStencilTexture2dFormat = DXGI_FORMAT_R24G8_TYPELESS,
			bool needRenderTargetShaderResourceView = true, bool needDepthStencilShaderResourceView = true, bool generateMips = false);

		virtual ~FrameBuffer() = default;
		FrameBuffer(FrameBuffer&) = delete;
		FrameBuffer& operator=(FrameBuffer&) = delete;

		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetView;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView;

		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> renderTargetShaderResourceView;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> depthStencilShaderResourceView;

		D3D11_VIEWPORT viewport = {};

		void Clear(ID3D11DeviceContext* immediateContext, float r = 0, float g = 0, float b = 0, float a = 1, UINT clear_flags = D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, float depth = 1, uint8_t stencil = 0)
		{
			float colour[4] = { r, g, b, a };
			if (renderTargetView)
			{
				immediateContext->ClearRenderTargetView(renderTargetView.Get(), colour);
			}
			if (depthStencilView)
			{
				immediateContext->ClearDepthStencilView(depthStencilView.Get(), clear_flags, depth, stencil);
			}
		}
		//clear only 'render_target_view'
		void ClearRenderTargetView(ID3D11DeviceContext* immediateContext, float r = 0, float g = 0, float b = 0, float a = 1)
		{
			float colour[4] = { r, g, b, a };
			immediateContext->ClearRenderTargetView(renderTargetView.Get(), colour);
		}
		//clear only 'depth_stencil_view'
		void ClearDepthStencilView(ID3D11DeviceContext* immediate_context, UINT clearFlags = D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, FLOAT depth = 1, UINT8 stencil = 0)
		{
			immediate_context->ClearDepthStencilView(depthStencilView.Get(), clearFlags, depth, stencil);
		}
		void Activate(ID3D11DeviceContext* immediateContext)
		{
			numberOfStoredViewports = D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE;
			immediateContext->RSGetViewports(&numberOfStoredViewports, cachedViewports);
			immediateContext->RSSetViewports(1, &viewport);

			immediateContext->OMGetRenderTargets(1, cachedRenderTargetView.ReleaseAndGetAddressOf(), cachedDepthStencilView.ReleaseAndGetAddressOf());
			immediateContext->OMSetRenderTargets(1, renderTargetView.GetAddressOf(), depthStencilView.Get());
		}
		//activate only 'render_target_view'
		void ActivateRenderTargetView(ID3D11DeviceContext* immediateContext)
		{
			numberOfStoredViewports = D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE;
			immediateContext->RSGetViewports(&numberOfStoredViewports, cachedViewports);
			immediateContext->RSSetViewports(1, &viewport);

			immediateContext->OMGetRenderTargets(1, cachedRenderTargetView.ReleaseAndGetAddressOf(), cachedDepthStencilView.ReleaseAndGetAddressOf());
			immediateContext->OMSetRenderTargets(1, renderTargetView.GetAddressOf(), 0);
		}
		//activate only 'depth_stencil_view'
		void ActivateDepthStencilView(ID3D11DeviceContext* immediateContext)
		{
			numberOfStoredViewports = D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE;
			immediateContext->RSGetViewports(&numberOfStoredViewports, cachedViewports);
			immediateContext->RSSetViewports(1, &viewport);

			immediateContext->OMGetRenderTargets(1, cachedRenderTargetView.ReleaseAndGetAddressOf(), cachedDepthStencilView.ReleaseAndGetAddressOf());
			ID3D11RenderTargetView* null_render_target_view = 0;
			immediateContext->OMSetRenderTargets(1, &null_render_target_view, depthStencilView.Get());
		}
		void DeActivate(ID3D11DeviceContext* immediateContext)
		{
			immediateContext->RSSetViewports(numberOfStoredViewports, cachedViewports);
			immediateContext->OMSetRenderTargets(1, cachedRenderTargetView.GetAddressOf(), cachedDepthStencilView.Get());
		}
	private:
		UINT numberOfStoredViewports;
		D3D11_VIEWPORT cachedViewports[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE];

		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> cachedRenderTargetView;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> cachedDepthStencilView;
	};

	//blit from an MSAA frame buffer to non-MSAA frame buffer
	class MsaaResolve
	{
	public:
		MsaaResolve(ID3D11Device* device)
		{
			const std::string shaderSource =
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
				"}\n"
				"Texture2DMS<float> depth_map  : register(t0);\n"
				"float PS(float4 position : SV_POSITION, float2 texcoord : TEXCOORD0) : SV_DEPTH\n"
				"{\n"
				"	uint width, height, number_of_samples;\n"
				"	depth_map.GetDimensions(width, height, number_of_samples);\n"
				"	return depth_map.Load(uint2(texcoord.x * width, texcoord.y * height), 0).r;\n"
				"}\n";
			CreateVSFromSource(device, "resolve_depth_stencil_vs", shaderSource, embeddedVertexShader.ReleaseAndGetAddressOf(), "VS", "vs_5_0", 0, 0, 0);
			CreatePSFromSource(device, "resolve_depth_stencil_ps", shaderSource, embeddedPixelShader.ReleaseAndGetAddressOf(), "PS");

			HRESULT hr = S_OK;

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
			depthStencilDesc.DepthEnable = TRUE;
			depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
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
		virtual ~MsaaResolve() = default;
		MsaaResolve(MsaaResolve&) = delete;
		MsaaResolve& operator =(MsaaResolve&) = delete;

	private:
		Microsoft::WRL::ComPtr<ID3D11VertexShader> embeddedVertexShader;
		Microsoft::WRL::ComPtr<ID3D11PixelShader> embeddedPixelShader;

		Microsoft::WRL::ComPtr<ID3D11RasterizerState> embeddedRasterizerState;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilState> embeddedDepthStencilState;

	public:
		void Resolve(ID3D11DeviceContext* immediateContext, const FrameBuffer* source, FrameBuffer* destination)
		{
			if (destination->renderTargetView && source->renderTargetView)
			{
				Microsoft::WRL::ComPtr<ID3D11Resource> destinationResource;
				Microsoft::WRL::ComPtr<ID3D11Resource> sourceResource;
				source->renderTargetView->GetResource(sourceResource.GetAddressOf());
				destination->renderTargetView->GetResource(destinationResource.GetAddressOf());

				Microsoft::WRL::ComPtr<ID3D11Texture2D> texture2d;
				HRESULT hr = sourceResource.Get()->QueryInterface<ID3D11Texture2D>(texture2d.GetAddressOf());
				_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

				D3D11_TEXTURE2D_DESC texture2dDesc;
				texture2d->GetDesc(&texture2dDesc);
				_ASSERT_EXPR(texture2dDesc.SampleDesc.Count > 1, L"source texture must be multisample texture");

				immediateContext->ResolveSubresource(destinationResource.Get(), D3D11CalcSubresource(0, 0, 1), sourceResource.Get(), D3D11CalcSubresource(0, 0, 1), texture2dDesc.Format);
			}
			if (destination->depthStencilShaderResourceView && source->depthStencilShaderResourceView)
			{
				destination->ClearDepthStencilView(immediateContext);
				destination->ActivateDepthStencilView(immediateContext);

				Microsoft::WRL::ComPtr<ID3D11RasterizerState> cached_rasterizer_state;
				immediateContext->RSGetState(cached_rasterizer_state.ReleaseAndGetAddressOf());
				immediateContext->RSSetState(embeddedRasterizerState.Get());

				Microsoft::WRL::ComPtr<ID3D11DepthStencilState> cached_depth_stencil_state;
				immediateContext->OMGetDepthStencilState(cached_depth_stencil_state.ReleaseAndGetAddressOf(), 0);
				immediateContext->OMSetDepthStencilState(embeddedDepthStencilState.Get(), 1);

				immediateContext->IASetVertexBuffers(0, 0, 0, 0, 0);
				immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
				immediateContext->IASetInputLayout(0);

				immediateContext->VSSetShader(embeddedVertexShader.Get(), 0, 0);
				immediateContext->PSSetShader(embeddedPixelShader.Get(), 0, 0);

				immediateContext->PSSetShaderResources(0, 1, source->depthStencilShaderResourceView.GetAddressOf());
				immediateContext->Draw(4, 0);

				immediateContext->RSSetState(cached_rasterizer_state.Get());
				immediateContext->OMSetDepthStencilState(cached_depth_stencil_state.Get(), 1);

				immediateContext->VSSetShader(0, 0, 0);
				immediateContext->PSSetShader(0, 0, 0);

				ID3D11ShaderResourceView* null_shader_resource_view = 0;
				immediateContext->PSSetShaderResources(0, 1, &null_shader_resource_view);

				destination->DeActivate(immediateContext);
			}
		}

		void Resolve(ID3D11DeviceContext* immediateContext, ID3D11RenderTargetView* source, ID3D11RenderTargetView* destination)
		{
			Microsoft::WRL::ComPtr<ID3D11Resource> destinationResource;
			Microsoft::WRL::ComPtr<ID3D11Resource> sourceResource;
			source->GetResource(sourceResource.GetAddressOf());
			destination->GetResource(destinationResource.GetAddressOf());

			Microsoft::WRL::ComPtr<ID3D11Texture2D> texture2d;
			HRESULT hr = sourceResource.Get()->QueryInterface<ID3D11Texture2D>(texture2d.GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

			D3D11_TEXTURE2D_DESC texture2dDesc;
			texture2d->GetDesc(&texture2dDesc);
			_ASSERT_EXPR(texture2dDesc.SampleDesc.Count > 1, L"source texture must be multisample texture");

			immediateContext->ResolveSubresource(destinationResource.Get(), D3D11CalcSubresource(0, 0, 1), sourceResource.Get(), D3D11CalcSubresource(0, 0, 1), texture2dDesc.Format);
		}

		void Resolve(ID3D11DeviceContext* immediateContext, ID3D11ShaderResourceView* source, ID3D11DepthStencilView* destination)
		{
			Microsoft::WRL::ComPtr<ID3D11Resource> destination_resource;
			destination->GetResource(destination_resource.GetAddressOf());
			Microsoft::WRL::ComPtr<ID3D11Texture2D> texture2d;
			HRESULT hr = destination_resource.Get()->QueryInterface<ID3D11Texture2D>(texture2d.GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
			D3D11_TEXTURE2D_DESC texture2d_desc;
			texture2d->GetDesc(&texture2d_desc);

			D3D11_VIEWPORT viewport;
			viewport.Width = static_cast<float>(texture2d_desc.Width);
			viewport.Height = static_cast<float>(texture2d_desc.Height);
			viewport.MinDepth = 0.0f;
			viewport.MaxDepth = 1.0f;
			viewport.TopLeftX = 0.0f;
			viewport.TopLeftY = 0.0f;

			immediateContext->ClearDepthStencilView(destination, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);

			UINT numberOfStoredViewports;
			D3D11_VIEWPORT cachedViewports[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE];
			numberOfStoredViewports = D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE;
			immediateContext->RSGetViewports(&numberOfStoredViewports, cachedViewports);
			immediateContext->RSSetViewports(1, &viewport);

			Microsoft::WRL::ComPtr<ID3D11RenderTargetView> cached_render_target_view;
			Microsoft::WRL::ComPtr<ID3D11DepthStencilView> cached_depth_stencil_view;
			immediateContext->OMGetRenderTargets(1, cached_render_target_view.ReleaseAndGetAddressOf(), cached_depth_stencil_view.ReleaseAndGetAddressOf());

			ID3D11RenderTargetView* null_render_target_view = 0;
			immediateContext->OMSetRenderTargets(1, &null_render_target_view, destination);

			Microsoft::WRL::ComPtr<ID3D11RasterizerState> cached_rasterizer_state;
			immediateContext->RSGetState(cached_rasterizer_state.ReleaseAndGetAddressOf());
			immediateContext->RSSetState(embeddedRasterizerState.Get());

			Microsoft::WRL::ComPtr<ID3D11DepthStencilState> cached_depth_stencil_state;
			immediateContext->OMGetDepthStencilState(cached_depth_stencil_state.ReleaseAndGetAddressOf(), 0);
			immediateContext->OMSetDepthStencilState(embeddedDepthStencilState.Get(), 1);

			immediateContext->IASetVertexBuffers(0, 0, 0, 0, 0);
			immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
			immediateContext->IASetInputLayout(0);

			immediateContext->VSSetShader(embeddedVertexShader.Get(), 0, 0);
			immediateContext->PSSetShader(embeddedPixelShader.Get(), 0, 0);

			immediateContext->PSSetShaderResources(0, 1, &source);
			immediateContext->Draw(4, 0);

			immediateContext->RSSetState(cached_rasterizer_state.Get());
			immediateContext->OMSetDepthStencilState(cached_depth_stencil_state.Get(), 1);

			immediateContext->VSSetShader(0, 0, 0);
			immediateContext->PSSetShader(0, 0, 0);

			ID3D11ShaderResourceView* null_shader_resource_view = 0;
			immediateContext->PSSetShaderResources(0, 1, &null_shader_resource_view);

			immediateContext->RSSetViewports(numberOfStoredViewports, cachedViewports);
			immediateContext->OMSetRenderTargets(1, cached_render_target_view.GetAddressOf(), cached_depth_stencil_view.Get());
		}
	};
}

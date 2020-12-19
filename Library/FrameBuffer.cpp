#include "Framebuffer.h"
#include "Misc.h"

namespace Descartes
{
	FrameBuffer::FrameBuffer(ID3D11Device* device, std::uint32_t width, std::uint32_t height, bool enableMsaa, int subsamples, DXGI_FORMAT renderTargetTexture2dFormat, DXGI_FORMAT depthStencilTexture2dFormat,
		bool needRenderTargetShaderResourceView, bool needDepthStencilShaderResourceView, bool generateMips)
	{
		HRESULT hr = S_OK;

		UINT msaaQualityLevel;
		UINT sampleCount = subsamples;
		hr = device->CheckMultisampleQualityLevels(renderTargetTexture2dFormat, sampleCount, &msaaQualityLevel);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		if (renderTargetTexture2dFormat != DXGI_FORMAT_UNKNOWN)
		{
			D3D11_TEXTURE2D_DESC texture2dDesc = {};
			texture2dDesc.Width = width;
			texture2dDesc.Height = height;
			texture2dDesc.MipLevels = generateMips ? 0 : 1;
			texture2dDesc.ArraySize = 1;
			texture2dDesc.Format = renderTargetTexture2dFormat;
			texture2dDesc.SampleDesc.Count = enableMsaa ? sampleCount : 1;
			texture2dDesc.SampleDesc.Quality = enableMsaa ? msaaQualityLevel - 1 : 0;
			texture2dDesc.Usage = D3D11_USAGE_DEFAULT;
			texture2dDesc.BindFlags = needRenderTargetShaderResourceView ? D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE : D3D11_BIND_RENDER_TARGET;
			texture2dDesc.CPUAccessFlags = 0;
			texture2dDesc.MiscFlags = generateMips ? D3D11_RESOURCE_MISC_GENERATE_MIPS : 0;

			Microsoft::WRL::ComPtr<ID3D11Texture2D> texture2d;
			hr = device->CreateTexture2D(&texture2dDesc, 0, texture2d.GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

			D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc = {};
			renderTargetViewDesc.Format = texture2dDesc.Format;
			renderTargetViewDesc.ViewDimension = enableMsaa ? D3D11_RTV_DIMENSION_TEXTURE2DMS : D3D11_RTV_DIMENSION_TEXTURE2D;
			hr = device->CreateRenderTargetView(texture2d.Get(), &renderTargetViewDesc, renderTargetView.GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

			if (needRenderTargetShaderResourceView)
			{
				D3D11_SHADER_RESOURCE_VIEW_DESC shader_resource_view_desc = {};
				shader_resource_view_desc.Format = texture2dDesc.Format;
				shader_resource_view_desc.ViewDimension = enableMsaa ? D3D11_SRV_DIMENSION_TEXTURE2DMS : D3D11_SRV_DIMENSION_TEXTURE2D;
				shader_resource_view_desc.Texture2D.MipLevels = generateMips ? -1 : 1;
				hr = device->CreateShaderResourceView(texture2d.Get(), &shader_resource_view_desc, renderTargetShaderResourceView.GetAddressOf());
				_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
			}
		}
		if (depthStencilTexture2dFormat != DXGI_FORMAT_UNKNOWN)
		{
			const DXGI_FORMAT combinationsOfDepthStencilFormats[3][3] =
			{
				{ DXGI_FORMAT_R24G8_TYPELESS, DXGI_FORMAT_D24_UNORM_S8_UINT ,DXGI_FORMAT_R24_UNORM_X8_TYPELESS },
				{ DXGI_FORMAT_R32_TYPELESS, DXGI_FORMAT_D32_FLOAT, DXGI_FORMAT_R32_FLOAT },
				{ DXGI_FORMAT_R16_TYPELESS, DXGI_FORMAT_D16_UNORM , DXGI_FORMAT_R16_UNORM },
			};
			int depthStencilTexture2dFormatIndex = 0;
			switch (depthStencilTexture2dFormatIndex)
			{
			case DXGI_FORMAT_R24G8_TYPELESS:
				depthStencilTexture2dFormatIndex = 0;
				break;
			case DXGI_FORMAT_R32_TYPELESS:
				depthStencilTexture2dFormatIndex = 1;
				break;
			case DXGI_FORMAT_R16_TYPELESS:
				depthStencilTexture2dFormatIndex = 2;
				break;
			}
			D3D11_TEXTURE2D_DESC texture2dDesc = {};
			texture2dDesc.Width = width;
			texture2dDesc.Height = height;
			texture2dDesc.MipLevels = 1;
			texture2dDesc.ArraySize = 1;
			texture2dDesc.Format = combinationsOfDepthStencilFormats[depthStencilTexture2dFormatIndex][0];
			texture2dDesc.SampleDesc.Count = enableMsaa ? sampleCount : 1;
			texture2dDesc.SampleDesc.Quality = enableMsaa ? msaaQualityLevel - 1 : 0;
			texture2dDesc.Usage = D3D11_USAGE_DEFAULT;
			texture2dDesc.BindFlags = needDepthStencilShaderResourceView ? D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE : D3D11_BIND_DEPTH_STENCIL;
			texture2dDesc.CPUAccessFlags = 0;
			texture2dDesc.MiscFlags = 0;
			Microsoft::WRL::ComPtr<ID3D11Texture2D> texture2d;
			hr = device->CreateTexture2D(&texture2dDesc, 0, texture2d.GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

			D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc = {};
			depthStencilViewDesc.Format = combinationsOfDepthStencilFormats[depthStencilTexture2dFormatIndex][1];
			depthStencilViewDesc.ViewDimension = enableMsaa ? D3D11_DSV_DIMENSION_TEXTURE2DMS : D3D11_DSV_DIMENSION_TEXTURE2D;
			depthStencilViewDesc.Flags = 0;
			hr = device->CreateDepthStencilView(texture2d.Get(), &depthStencilViewDesc, depthStencilView.GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

			if (needDepthStencilShaderResourceView)
			{
				D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc = {};
				shaderResourceViewDesc.Format = combinationsOfDepthStencilFormats[depthStencilTexture2dFormatIndex][2];
				shaderResourceViewDesc.ViewDimension = enableMsaa ? D3D11_SRV_DIMENSION_TEXTURE2DMS : D3D11_SRV_DIMENSION_TEXTURE2D;
				shaderResourceViewDesc.Texture2D.MipLevels = 1;
				hr = device->CreateShaderResourceView(texture2d.Get(), &shaderResourceViewDesc, depthStencilShaderResourceView.GetAddressOf());
				_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
			}
		}
		viewport.Width = static_cast<float>(width);
		viewport.Height = static_cast<float>(height);
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		viewport.TopLeftX = 0.0f;
		viewport.TopLeftY = 0.0f;
	}
}

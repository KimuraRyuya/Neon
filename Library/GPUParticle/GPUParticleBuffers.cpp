#include "GPUParticleBuffers.h"
#include "../Library/FrameWork.h"
#include "../Library/CommonInclude.h"

namespace GPUParticleBuffers
{
	HRESULT CreateSRV(const D3D11_BUFFER_DESC* pDesc, GPUBuffer* pBuffer, DXGI_FORMAT format)
	{
		HRESULT hr = S_OK;
		D3D11_BUFFER_DESC desc = {};
		desc.ByteWidth = pDesc->ByteWidth;
		desc.Usage = pDesc->Usage;
		desc.BindFlags = pDesc->BindFlags;
		desc.CPUAccessFlags = pDesc->CPUAccessFlags;
		desc.MiscFlags = pDesc->MiscFlags;
		desc.StructureByteStride = pDesc->StructureByteStride;
		
		if (desc.BindFlags & D3D11_BIND_SHADER_RESOURCE)
		{
			D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc = {};
			ZeroMemory(&srv_desc, sizeof(srv_desc));

			// This is a Raw Buffer
			if (desc.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS)
			{
				srv_desc.Format = DXGI_FORMAT_R32_TYPELESS;
				srv_desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
				srv_desc.BufferEx.FirstElement = 0;
				srv_desc.BufferEx.Flags = D3D11_BUFFEREX_SRV_FLAG_RAW;
				srv_desc.BufferEx.NumElements = desc.ByteWidth / 4;
			}
			else if (desc.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_STRUCTURED)
			{
				// This is a Structured Buffer

				srv_desc.Format = DXGI_FORMAT_UNKNOWN;
				srv_desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
				srv_desc.BufferEx.FirstElement = 0;
				srv_desc.BufferEx.NumElements = desc.ByteWidth / desc.StructureByteStride;
			}
			else
			{
				// This is a Typed Buffer

				srv_desc.Format = format;
				srv_desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
				srv_desc.Buffer.FirstElement = 0;
				srv_desc.Buffer.NumElements = desc.ByteWidth / desc.StructureByteStride;
			}

			hr = pFrameWork.GetDevice()->CreateShaderResourceView(pBuffer->buffer.Get(), &srv_desc, pBuffer->SRV.GetAddressOf());
			assert(SUCCEEDED(hr) && "ShaderResourceView of the GPUBuffer could not be created!");
		}
		return hr;
	}

	HRESULT CreateUAV(const D3D11_BUFFER_DESC* pDesc, GPUBuffer* pBuffer, DXGI_FORMAT format)
	{
		HRESULT hr = S_OK;
		D3D11_BUFFER_DESC desc = {};
		desc.ByteWidth = pDesc->ByteWidth;
		desc.Usage = pDesc->Usage;
		desc.BindFlags = pDesc->BindFlags;
		desc.CPUAccessFlags = pDesc->CPUAccessFlags;
		desc.MiscFlags = pDesc->MiscFlags;
		desc.StructureByteStride = pDesc->StructureByteStride;

		if (desc.BindFlags & D3D11_BIND_UNORDERED_ACCESS)
		{
			D3D11_UNORDERED_ACCESS_VIEW_DESC uav_desc = {};
			uav_desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
			uav_desc.Buffer.FirstElement = 0;

			if (desc.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS)
			{
				uav_desc.Format = DXGI_FORMAT_R32_TYPELESS;
				uav_desc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_RAW;
				uav_desc.Buffer.NumElements = desc.ByteWidth / 4;
			}
			else if (desc.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_STRUCTURED)
			{
				uav_desc.Format = DXGI_FORMAT_UNKNOWN;
				uav_desc.Buffer.NumElements = desc.ByteWidth / desc.StructureByteStride;
			}
			else
			{
				uav_desc.Format = format;
				uav_desc.Buffer.NumElements = desc.ByteWidth / desc.StructureByteStride;
			}

			hr = pFrameWork.GetDevice()->CreateUnorderedAccessView(pBuffer->buffer.Get(), &uav_desc, pBuffer->UAV.GetAddressOf());
			assert(SUCCEEDED(hr) && "UnorderedAccessView of the GPUBuffer could not be created!");
		}

		return hr;
	}

	HRESULT CreateBuffer(const D3D11_BUFFER_DESC* pDesc, D3D11_SUBRESOURCE_DATA* pData, GPUBuffer* pBuffer)
	{
		HRESULT hr = S_OK;
		D3D11_BUFFER_DESC desc = {};
		desc.ByteWidth = pDesc->ByteWidth;
		desc.Usage = pDesc->Usage;
		desc.BindFlags = pDesc->BindFlags;
		desc.CPUAccessFlags = pDesc->CPUAccessFlags;
		desc.MiscFlags = pDesc->MiscFlags;
		desc.StructureByteStride = pDesc->StructureByteStride;

		D3D11_SUBRESOURCE_DATA* data = nullptr;
		if (pData != nullptr)
		{
			data = new D3D11_SUBRESOURCE_DATA[1];
			for (uint32_t slice = 0; slice < 1; ++slice)
			{
				data[slice] = pData[slice];
			}
		}

		hr = pFrameWork.GetDevice()->CreateBuffer(&desc, data, pBuffer->buffer.GetAddressOf());
		SAFE_DELETE_ARRAY(data);
		assert(SUCCEEDED(hr) && "GPUBuffer creation failed!");

		return hr;
	}

}
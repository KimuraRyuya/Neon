#pragma once

#include <d3d11.h>
#include <wrl.h>
#include <assert.h>

#define USAGE_DYNAMIC
namespace Descartes
{
	//https://docs.microsoft.com/en-us/windows/desktop/direct3dhlsl/dx-graphics-hlsl-constants
	//https://docs.microsoft.com/en-us/windows/desktop/dxtecharts/direct3d10-frequently-asked-questions
	//https://developer.nvidia.com/content/constant-buffers-without-constant-pain-0

	template <class T>
	struct ConstantBuffer
	{
		T data{};
		ConstantBuffer(ID3D11Device* device)
		{
			_ASSERT_EXPR(sizeof(T) % 16 == 0, L"constant buffer's need to be 16 byte aligned");
			HRESULT hr = S_OK;
			D3D11_BUFFER_DESC bufferDesc = {};
			bufferDesc.ByteWidth = sizeof(T);
			bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			bufferDesc.MiscFlags = 0;
			bufferDesc.StructureByteStride = 0;
#ifdef USAGE_DYNAMIC
			//https://docs.microsoft.com/en-us/windows/desktop/direct3d11/how-to--use-dynamic-resources
			bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
			bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			hr = device->CreateBuffer(&bufferDesc, 0, bufferObject.GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
#else
			buffer_desc.Usage = D3D11_USAGE_DEFAULT;
			buffer_desc.CPUAccessFlags = 0;
			D3D11_SUBRESOURCE_DATA subresource_data;
			subresource_data.pSysMem = &data;
			subresource_data.SysMemPitch = 0;
			subresource_data.SysMemSlicePitch = 0;
			hr = device->CreateBuffer(&buffer_desc, &subresource_data, buffer_object.GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
#endif
		}
		virtual ~ConstantBuffer() = default;
		ConstantBuffer(ConstantBuffer&) = delete;
		ConstantBuffer& operator =(ConstantBuffer&) = delete;

		void Activate(ID3D11DeviceContext* immediateContext, UINT slot, bool vs_on = true, bool ps_on = true)
		{
			HRESULT hr = S_OK;
#ifdef USAGE_DYNAMIC
			D3D11_MAP map = D3D11_MAP_WRITE_DISCARD;
			D3D11_MAPPED_SUBRESOURCE mappedBuffer;

			hr = immediateContext->Map(bufferObject.Get(), 0, map, 0, &mappedBuffer);
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
			memcpy_s(mappedBuffer.pData, sizeof(T), &data, sizeof(T));
			immediateContext->Unmap(bufferObject.Get(), 0);
#else
			immediate_context->UpdateSubresource(buffer_object.Get(), 0, 0, &data, 0, 0);
#endif
			if (vs_on)
			{
				immediateContext->VSSetConstantBuffers(slot, 1u, bufferObject.GetAddressOf());
			}
			if (ps_on)
			{
				immediateContext->PSSetConstantBuffers(slot, 1u, bufferObject.GetAddressOf());
			}
		}
		void DeActivate(ID3D11DeviceContext* immediate_context)
		{
			// NOP
		}
	private:
		Microsoft::WRL::ComPtr<ID3D11Buffer> bufferObject;

	};
}

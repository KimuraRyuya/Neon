#pragma once
#include <memory>
#include <d3d11.h>
#include <wrl.h>

namespace GPUParticleBuffers
{
	class GPUBuffer
	{
	public:
		Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> SRV;
		Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> UAV;

		void Reset()
		{
			buffer.Reset();
			SRV.Reset();
			UAV.Reset();
		}
		GPUBuffer() {}
		GPUBuffer(const GPUBuffer& obj) { buffer = obj.buffer.Get(); SRV = obj.SRV.Get(); UAV = obj.UAV.Get(); }
		~GPUBuffer() {}
	};

	HRESULT CreateBuffer(const D3D11_BUFFER_DESC* pDesc, D3D11_SUBRESOURCE_DATA* pData, GPUBuffer* pBuffer);
	HRESULT CreateSRV(const D3D11_BUFFER_DESC* pDesc, GPUBuffer* pBuffer, DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN);
	HRESULT CreateUAV(const D3D11_BUFFER_DESC* pDesc, GPUBuffer* pBuffer, DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM);

}
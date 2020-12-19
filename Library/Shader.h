#pragma once

#include <d3d11.h>
#include <wrl.h>
#include <d3d11shader.h>

#include <string>

namespace Descartes
{
	//you can set null for 'input_layout' if 'input_layout' is not necessary 
	HRESULT CreateVSFromCso(ID3D11Device* device, const char* cso_name,
		ID3D11VertexShader** vertex_shader,
		ID3D11InputLayout** input_layout, D3D11_INPUT_ELEMENT_DESC* input_element_desc, size_t num_elements, bool enable_caching = true);
	HRESULT CreateVSFromSource(ID3D11Device* device, const char* key_name, const std::string& source,
		ID3D11VertexShader** vertex_shader, const char* entry_point, const char* target,
		ID3D11InputLayout** input_layout, D3D11_INPUT_ELEMENT_DESC* input_element_desc, size_t num_elements, bool enable_caching = true);
	void ReleaseAllCachedVertexShaders();

	HRESULT CreatePSFromCso(ID3D11Device* device, const char* cso_name,
		ID3D11PixelShader** pixel_shader, ID3D11ClassLinkage** class_linkage = 0, ID3D11ShaderReflection** reflector = 0, bool enable_caching = true);
	HRESULT CreatePSFromSource(ID3D11Device* device, const char* key_name, const std::string& source,
		ID3D11PixelShader** pixel_shader, const char* entry_point, const char* target = "ps_5_0", bool enable_caching = true);
	void ReleaseAllCachedPixelShaders();

	class VertexShader
	{
	public:
		VertexShader() = default;
		VertexShader(ID3D11Device* device, const char* cso, D3D11_INPUT_ELEMENT_DESC* inputElementDesc, size_t numElements, bool enableCaching = true)
		{
			Descartes::CreateVSFromCso(device, cso, shaderObject.GetAddressOf(), inputLayout.GetAddressOf(), const_cast<D3D11_INPUT_ELEMENT_DESC*>(inputElementDesc), numElements, enableCaching);
		}

		virtual ~VertexShader() = default;
		VertexShader(VertexShader&) = delete;
		VertexShader& operator=(VertexShader&) = delete;

		Microsoft::WRL::ComPtr<ID3D11VertexShader> shaderObject;
		Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;

		void Activate(ID3D11DeviceContext* immediateContext)
		{
			immediateContext->IAGetInputLayout(cachedInputLayout.ReleaseAndGetAddressOf());
			immediateContext->VSGetShader(cachedShaderObject.ReleaseAndGetAddressOf(), 0, 0);

			immediateContext->IASetInputLayout(inputLayout.Get());
			immediateContext->VSSetShader(shaderObject.Get(), 0, 0);
		}
		void DeActivate(ID3D11DeviceContext* immediateContext)
		{
			immediateContext->IASetInputLayout(cachedInputLayout.Get());
			immediateContext->VSSetShader(cachedShaderObject.Get(), 0, 0);
		}
	private:
		Microsoft::WRL::ComPtr<ID3D11VertexShader> cachedShaderObject;
		Microsoft::WRL::ComPtr<ID3D11InputLayout> cachedInputLayout;
	};
	class PixelShader
	{
	public:
		PixelShader() = default;
		PixelShader(ID3D11Device* device, const char* cso)
		{
			Descartes::CreatePSFromCso(device, cso, shaderObject.GetAddressOf());
		}
		virtual ~PixelShader() = default;
		PixelShader(PixelShader&) = delete;
		PixelShader& operator=(PixelShader&) = delete;

		Microsoft::WRL::ComPtr<ID3D11PixelShader> shaderObject;

		void Activate(ID3D11DeviceContext* immediateContext)
		{
			immediateContext->PSGetShader(cachedShaderObject.ReleaseAndGetAddressOf(), 0, 0);
			immediateContext->PSSetShader(shaderObject.Get(), 0, 0);
		}
		void DeActivate(ID3D11DeviceContext* immediateContext)
		{
			immediateContext->PSSetShader(cachedShaderObject.Get(), 0, 0);
		}
	private:
		Microsoft::WRL::ComPtr<ID3D11PixelShader> cachedShaderObject;
	};

}


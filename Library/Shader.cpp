#include <d3dcompiler.h>

#include <memory>
#include <map>
#include <sstream>
#include "Misc.h"
#include "Shader.h"

namespace Descartes
{
	struct VSCombo
	{
		VSCombo(ID3D11VertexShader* vertexShader, ID3D11InputLayout* inputLayout) : vertexShader(vertexShader), inputLayout(inputLayout) {}
		Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
		Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
	};
	static std::map<std::string, VSCombo> cachedVertexShaders;

	//you can set null for 'input_layout' if it is not necessary 
	HRESULT CreateVSFromCso(ID3D11Device* device, const char* csoName, ID3D11VertexShader** vertexShader, ID3D11InputLayout** inputLayout, D3D11_INPUT_ELEMENT_DESC* inputElementDesc, size_t numElements, bool enableCaching)
	{
		std::map<std::string, VSCombo>::iterator it = cachedVertexShaders.find(csoName);
		if (it != cachedVertexShaders.end())
		{
			*vertexShader = it->second.vertexShader.Get();
			(*vertexShader)->AddRef();
			if (inputLayout)
			{
				*inputLayout = it->second.inputLayout.Get();
				_ASSERT_EXPR_A(*inputLayout, "cached input_layout must be not nil");
				(*inputLayout)->AddRef();
			}
			return S_OK;
		}

		FILE* fp = 0;
		fopen_s(&fp, csoName, "rb");
		_ASSERT_EXPR_A(fp, (std::string("cso File not found : ") + csoName).c_str());

		fseek(fp, 0, SEEK_END);
		long cso_sz = ftell(fp);
		fseek(fp, 0, SEEK_SET);

		std::unique_ptr<uint8_t[]> cso_data = std::make_unique<uint8_t[]>(cso_sz);
		fread(cso_data.get(), cso_sz, 1, fp);
		fclose(fp);

		HRESULT hr = S_OK;
		hr = device->CreateVertexShader(cso_data.get(), cso_sz, 0, vertexShader);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		if (inputLayout)
		{
			hr = device->CreateInputLayout(inputElementDesc, static_cast<UINT>(numElements), cso_data.get(), cso_sz, inputLayout);
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		}

		if (enableCaching)
		{
			cachedVertexShaders.insert(std::make_pair(csoName, VSCombo(*vertexShader, inputLayout ? *inputLayout : 0)));
		}

		return hr;
	}
	HRESULT CreateVSFromSource(ID3D11Device* device, const char* keyName, const std::string& source,
		ID3D11VertexShader** vertexShader, const char* entryPoint, const char* target,
		ID3D11InputLayout** inputLayout, D3D11_INPUT_ELEMENT_DESC* inputElementDesc, size_t numElements, bool enableCaching)
	{
		std::map<std::string, VSCombo>::iterator it = cachedVertexShaders.find(keyName);
		if (it != cachedVertexShaders.end())
		{
			*vertexShader = it->second.vertexShader.Get();
			(*vertexShader)->AddRef();
			if (inputLayout)
			{
				*inputLayout = it->second.inputLayout.Get();
				_ASSERT_EXPR_A(*inputLayout, "cached input_layout must be not nil");
				(*inputLayout)->AddRef();
			}
			return S_OK;
		}

		HRESULT hr = S_OK;
		UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
		flags |= D3DCOMPILE_DEBUG;
		flags |= D3D10_SHADER_SKIP_OPTIMIZATION;
#endif
		Microsoft::WRL::ComPtr<ID3DBlob> compiledShaderBlob;
		Microsoft::WRL::ComPtr<ID3DBlob> errorMessageBlob;
		hr = D3DCompile(source.c_str(), source.length(), 0, 0, 0, entryPoint, "vs_5_0", flags, 0, compiledShaderBlob.GetAddressOf(), errorMessageBlob.GetAddressOf());
		_ASSERT_EXPR_A(SUCCEEDED(hr), reinterpret_cast<LPCSTR>(errorMessageBlob->GetBufferPointer()));
		hr = device->CreateVertexShader(compiledShaderBlob->GetBufferPointer(), compiledShaderBlob->GetBufferSize(), 0, vertexShader);
		_ASSERT_EXPR_A(SUCCEEDED(hr), reinterpret_cast<LPCSTR>(errorMessageBlob->GetBufferPointer()));

		if (inputLayout)
		{
			hr = device->CreateInputLayout(inputElementDesc, static_cast<UINT>(numElements), compiledShaderBlob->GetBufferPointer(), compiledShaderBlob->GetBufferSize(), inputLayout);
			_ASSERT_EXPR_A(SUCCEEDED(hr), reinterpret_cast<LPCSTR>(errorMessageBlob->GetBufferPointer()));
		}

		if (enableCaching)
		{
			cachedVertexShaders.insert(std::make_pair(keyName, VSCombo(*vertexShader, inputLayout ? *inputLayout : 0)));
		}

		return hr;
	}
	void ReleaseAllCachedVertexShaders()
	{
		cachedVertexShaders.clear();
	}

	struct PSCombo
	{
		PSCombo(ID3D11PixelShader* pixelShader, ID3D11ClassLinkage* classLinkage, ID3D11ShaderReflection* reflector) : pixelShader(pixelShader), classLinkage(classLinkage), reflector(reflector) {}
		Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
		Microsoft::WRL::ComPtr<ID3D11ClassLinkage> classLinkage;
		Microsoft::WRL::ComPtr<ID3D11ShaderReflection> reflector;
	};
	static std::map<std::string, PSCombo> cachedPixelShaders;
	HRESULT CreatePSFromCso(ID3D11Device* device, const char* csoName,
		ID3D11PixelShader** pixelShader, ID3D11ClassLinkage** classLinkage, ID3D11ShaderReflection** reflector,
		bool enableCaching)
	{
		std::map<std::string, PSCombo>::iterator it = cachedPixelShaders.find(csoName);
		if (it != cachedPixelShaders.end())
		{
			*pixelShader = it->second.pixelShader.Get();
			(*pixelShader)->AddRef();
			if (classLinkage)
			{
				*classLinkage = it->second.classLinkage.Get();
				(*classLinkage)->AddRef();
			}
			if (reflector)
			{
				*reflector = it->second.reflector.Get();
				(*reflector)->AddRef();
			}
			return S_OK;
		}
		FILE* fp = 0;
		fopen_s(&fp, csoName, "rb");
		_ASSERT_EXPR_A(fp, (std::string("cso File not found : ") + csoName).c_str());

		fseek(fp, 0, SEEK_END);
		long csoSz = ftell(fp);
		fseek(fp, 0, SEEK_SET);

		std::unique_ptr<uint8_t[]> csoData = std::make_unique<uint8_t[]>(csoSz);
		fread(csoData.get(), csoSz, 1, fp);
		fclose(fp);

		HRESULT hr = S_OK;
		if (classLinkage)
		{
			hr = device->CreateClassLinkage(classLinkage);
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		}
		if (reflector)
		{
			hr = D3DReflect(csoData.get(), csoSz, IID_ID3D11ShaderReflection, reinterpret_cast<void**>(reflector));
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		}

		hr = device->CreatePixelShader(csoData.get(), csoSz, classLinkage ? *classLinkage : 0, pixelShader);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		if (enableCaching)
		{
			cachedPixelShaders.insert(std::make_pair(csoName, PSCombo(*pixelShader, classLinkage ? *classLinkage : 0, reflector ? *reflector : 0)));
		}

		return hr;
	}
	HRESULT CreatePSFromSource(ID3D11Device* device, const char* keyName, const std::string& source, ID3D11PixelShader** pixelShader, const char* entryPoint, const char* target, bool enableCaching)
	{
		std::map<std::string, PSCombo>::iterator it = cachedPixelShaders.find(keyName);
		if (it != cachedPixelShaders.end())
		{
			*pixelShader = it->second.pixelShader.Get();
			(*pixelShader)->AddRef();
			return S_OK;
		}

		HRESULT hr = S_OK;
		UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
		flags |= D3DCOMPILE_DEBUG;
		flags |= D3D10_SHADER_SKIP_OPTIMIZATION;
#endif
		Microsoft::WRL::ComPtr<ID3DBlob> compiledShaderBlob;
		Microsoft::WRL::ComPtr<ID3DBlob> errorMessageBlob;
		hr = D3DCompile(source.c_str(), source.length(), 0, 0, 0, entryPoint, target, flags, 0, compiledShaderBlob.GetAddressOf(), errorMessageBlob.GetAddressOf());
		_ASSERT_EXPR_A(SUCCEEDED(hr), reinterpret_cast<LPCSTR>(errorMessageBlob->GetBufferPointer()));
		hr = device->CreatePixelShader(compiledShaderBlob->GetBufferPointer(), compiledShaderBlob->GetBufferSize(), 0, pixelShader);
		_ASSERT_EXPR_A(SUCCEEDED(hr), reinterpret_cast<LPCSTR>(errorMessageBlob->GetBufferPointer()));

		if (enableCaching)
		{
			cachedPixelShaders.insert(std::make_pair(keyName, PSCombo(*pixelShader, 0, 0)));
		}

		return hr;
	}
	void ReleaseAllCachedPixelShaders()
	{
		cachedPixelShaders.clear();
	}
}
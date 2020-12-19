#pragma once
#include <memory>
#include <d3d11.h>
#include <wrl.h>
#include <string>
#include <vector>
#include <directxmath.h>
#include <iostream>
#include <fstream>
#include "../Library/Logger.h"
#include "../Library/FrameWork.h"
#include "../Library/Misc.h"

class Mapper
{
private:
	ID3D11DeviceContext* immediateContext;
	ID3D11Buffer* buffer;
	D3D11_MAPPED_SUBRESOURCE	resource;

public:
	Mapper() : immediateContext(nullptr), buffer(nullptr), resource() {}

	Mapper(ID3D11Buffer* pBuffer)
		: immediateContext(pFrameWork.GetContext()), buffer(pBuffer)
	{
		HRESULT	hr = immediateContext->Map(pBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}
	Mapper(ID3D11Buffer* pBuffer, UINT subresource, D3D11_MAP mapType)
		: immediateContext(pFrameWork.GetContext()), buffer(pBuffer)
	{
		HRESULT hr = immediateContext->Map(pBuffer, subresource, mapType, 0, &resource);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}
	~Mapper()
	{
		immediateContext->Unmap(buffer, 0);
	}

	void* GetBufferPtr() { return resource.pData; }

};

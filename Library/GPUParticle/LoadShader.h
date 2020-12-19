#pragma once
#include <d3d11.h>
#include "../Library/FrameWork.h"

namespace LoadShader
{
	bool LoadVertexShader(const std::string& filename, ID3D11VertexShader** shader, D3D11_INPUT_ELEMENT_DESC* inputDesc, UINT inputDescSize, ID3D11InputLayout** inputLayout);
	bool LoadPixelShader(const std::string& filename, ID3D11PixelShader** shader);
	bool LoadComputeShader(const std::string& filename, ID3D11ComputeShader** shader);
}
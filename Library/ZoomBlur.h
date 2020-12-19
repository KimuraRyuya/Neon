#pragma once

#include <memory>

#include <iostream>
#include <fstream>
#include <shlwapi.h>
#include<vector>
#undef max
#undef min

#include "FrameBuffer.h"
#include "Rasterizer.h"
#include "Shader.h"
#include "ConstantBuffer.h"
#include "RenderState.h"

class ZoomBlur : Descartes::FullscreenQuad
{
public:
	struct ShaderConstants
	{
		float blurScalar = 3.0f;
		float samples = 25.0f;
		float minBlur = 0.0f;
		float maxBlur = 0.1f;
	};

	std::unique_ptr<Descartes::ConstantBuffer<ShaderConstants>> constantBuffer;

	ZoomBlur(ID3D11Device* device, uint32_t width, uint32_t height);
	~ZoomBlur() = default;
	ZoomBlur(ZoomBlur&) = delete;
	ZoomBlur& operator =(ZoomBlur&) = delete;

	void Blit(ID3D11DeviceContext* immediateContext, ID3D11ShaderResourceView* shaderResourceView);

private:
	enum { LINEAR_BORDER, POINT, LINEAR, ANISOTROPIC };
	std::unique_ptr<Descartes::SamplerState> samplerStates[4];

	std::unique_ptr<Descartes::BlendState> blendState;

	std::unique_ptr<Descartes::PixelShader> zoomBlurPS;

};
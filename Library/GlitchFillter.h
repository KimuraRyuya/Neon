#pragma once

#include <memory>

#include <iostream>
#include <fstream>
#include <shlwapi.h>
#undef max
#undef min

#include "FrameBuffer.h"
#include "RenderState.h"
#include "Rasterizer.h"
#include "ConstantBuffer.h"
#include "Shader.h"
#include "Texture.h"

class GlitchFillter : Descartes::FullscreenQuad
{
public:
	GlitchFillter(ID3D11Device* device) : Descartes::FullscreenQuad(device)
	{
		Texture::LoadTextureFromFile(device, L"Data/Texture/randamNoise.png", randomNoiseSRV.GetAddressOf(), &randomNoiseDesc);
		Texture::LoadTextureFromFile(device, L"Data/Texture/glitch.png", glitchSRV.GetAddressOf(), &glitchDesc);

		glitch_ps = std::make_unique<Descartes::PixelShader>(device, "Shader/GlitchFillter.cso");
		sampler_states[ANISOTROPIC] = std::make_unique<Descartes::SamplerState>(device, D3D11_FILTER_ANISOTROPIC, D3D11_TEXTURE_ADDRESS_WRAP, D3D11_COMPARISON_ALWAYS, 0.0f, 0.0f, 0.0f, 1.0f);
	}
	virtual ~GlitchFillter() = default;

	enum { ANISOTROPIC };
	std::unique_ptr<Descartes::SamplerState> sampler_states[1];

private:
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> randomNoiseSRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> glitchSRV;

	D3D11_TEXTURE2D_DESC randomNoiseDesc;
	D3D11_TEXTURE2D_DESC glitchDesc;

	std::unique_ptr<Descartes::PixelShader> glitch_ps;
public:
	void Blit(ID3D11DeviceContext* immediate_context, ID3D11ShaderResourceView* hdr_texture, float elapsed_time, bool enableTone = false)
	{
		immediate_context->PSSetShaderResources(0, 1, &hdr_texture);
		immediate_context->PSSetShaderResources(0, 1, &randomNoiseSRV);
		immediate_context->PSSetShaderResources(0, 1, &glitchSRV);
		sampler_states[ANISOTROPIC]->Activate(immediate_context, 0, false);

		glitch_ps->Activate(immediate_context);
		Descartes::FullscreenQuad::Blit(immediate_context);
		glitch_ps->DeActivate(immediate_context);
	
		sampler_states[ANISOTROPIC]->DeActivate(immediate_context);

		ID3D11ShaderResourceView* null_shader_resource_views[3] = {};
		immediate_context->PSSetShaderResources(0, 3, null_shader_resource_views);
	}
};

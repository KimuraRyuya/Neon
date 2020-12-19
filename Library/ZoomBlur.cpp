#include "ZoomBlur.h"

ZoomBlur::ZoomBlur(ID3D11Device* device, uint32_t width, uint32_t height) : Descartes::FullscreenQuad(device)
{
	constantBuffer = std::make_unique<Descartes::ConstantBuffer<ShaderConstants>>(device);

	zoomBlurPS= std::make_unique<Descartes::PixelShader>(device, "Shader/ZoomBlurPS.cso");

	samplerStates[LINEAR_BORDER] = std::make_unique<Descartes::SamplerState>(device, D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_BORDER, D3D11_COMPARISON_ALWAYS, 0.0f, 0.0f, 0.0f, 1.0f);
	samplerStates[POINT] = std::make_unique<Descartes::SamplerState>(device, D3D11_FILTER_MIN_MAG_MIP_POINT, D3D11_TEXTURE_ADDRESS_WRAP, D3D11_COMPARISON_ALWAYS, 0.0f, 0.0f, 0.0f, 1.0f);
	samplerStates[LINEAR] = std::make_unique<Descartes::SamplerState>(device, D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_WRAP, D3D11_COMPARISON_ALWAYS, 0.0f, 0.0f, 0.0f, 1.0f);
	samplerStates[ANISOTROPIC] = std::make_unique<Descartes::SamplerState>(device, D3D11_FILTER_ANISOTROPIC, D3D11_TEXTURE_ADDRESS_WRAP, D3D11_COMPARISON_ALWAYS, 0.0f, 0.0f, 0.0f, 1.0f);

	blendState = std::make_unique<Descartes::BlendState>(device, BLEND_MODE::ALPHA);
}

void ZoomBlur::Blit(ID3D11DeviceContext* immediate_context, ID3D11ShaderResourceView* shaderResourceView)
{
	samplerStates[POINT]->Activate(immediate_context, 0, false);
	samplerStates[LINEAR]->Activate(immediate_context, 1, false);
	samplerStates[ANISOTROPIC]->Activate(immediate_context, 2, false);
	blendState->Activate(immediate_context);
	constantBuffer->Activate(immediate_context, 0, false, true);

	immediate_context->PSSetShaderResources(0, 1, &shaderResourceView);
	zoomBlurPS->Activate(immediate_context);
	Descartes::FullscreenQuad::Blit(immediate_context);
	zoomBlurPS->DeActivate(immediate_context);
	ID3D11ShaderResourceView* null_shader_resource_views[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT] = { 0 };
	immediate_context->PSSetShaderResources(0, D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT, null_shader_resource_views);

	constantBuffer->DeActivate(immediate_context);
	blendState->DeActivate(immediate_context);
	samplerStates[POINT]->DeActivate(immediate_context);
	samplerStates[LINEAR]->DeActivate(immediate_context);
	samplerStates[ANISOTROPIC]->DeActivate(immediate_context);
}


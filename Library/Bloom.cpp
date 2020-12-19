#include "Bloom.h"

Bloom::Bloom(ID3D11Device* device, uint32_t width, uint32_t height) : Descartes::FullscreenQuad(device)
{
	constantBuffer = std::make_unique<Descartes::ConstantBuffer<ShaderConstants>>(device);

	glowExtraction = std::make_unique<Descartes::FrameBuffer>(device, width, height, false/*enable_msaa*/, 1/*subsamples*/,
		DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_UNKNOWN,
		true/*need_render_target_shader_resource_view*/, false/*need_depth_stencil_shader_resource_view*/,
		false/*generate_mips*/);

	for (size_t index_of_downsampled = 0; index_of_downsampled < numberOfDownsampled; ++index_of_downsampled)
	{
		gaussianBlur[index_of_downsampled][0] = std::make_unique<Descartes::FrameBuffer>(device, width >> index_of_downsampled, height >> index_of_downsampled, false/*enable_msaa*/, 1/*subsamples*/,
			DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_UNKNOWN,
			true/*need_render_target_shader_resource_view*/, false/*need_depth_stencil_shader_resource_view*/,
			false/*generate_mips*/);
		gaussianBlur[index_of_downsampled][1] = std::make_unique<Descartes::FrameBuffer>(device, width >> index_of_downsampled, height >> index_of_downsampled, false/*enable_msaa*/, 1/*subsamples*/,
			DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_UNKNOWN,
			true/*need_render_target_shader_resource_view*/, false/*need_depth_stencil_shader_resource_view*/,
			false/*generate_mips*/);
	}
	glowExtractionPS = std::make_unique<Descartes::PixelShader>(device, "Shader/GlowExtracionPS.cso");
	gaussianBlurHorizontalPS = std::make_unique<Descartes::PixelShader>(device, "Shader/GaussHorizontal.cso");
	gaussianBlurVerticalPS = std::make_unique<Descartes::PixelShader>(device, "Shader/GaussVertical.cso");
	gaussianBlurConvolutionPS = std::make_unique<Descartes::PixelShader>(device, "Shader/GaussConvolution.cso");
	gaussianBlurDownsamplingPS = std::make_unique<Descartes::PixelShader>(device, "Shader/GaussDownSampling.cso");


	samplerStates[LINEAR_BORDER] = std::make_unique<Descartes::SamplerState>(device, D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_BORDER, D3D11_COMPARISON_ALWAYS, 0.0f, 0.0f, 0.0f, 1.0f);
	samplerStates[POINT] = std::make_unique<Descartes::SamplerState>(device, D3D11_FILTER_MIN_MAG_MIP_POINT, D3D11_TEXTURE_ADDRESS_WRAP, D3D11_COMPARISON_ALWAYS, 0.0f, 0.0f, 0.0f, 1.0f);
	samplerStates[LINEAR] = std::make_unique<Descartes::SamplerState>(device, D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_WRAP, D3D11_COMPARISON_ALWAYS, 0.0f, 0.0f, 0.0f, 1.0f);
	samplerStates[ANISOTROPIC] = std::make_unique<Descartes::SamplerState>(device, D3D11_FILTER_ANISOTROPIC, D3D11_TEXTURE_ADDRESS_WRAP, D3D11_COMPARISON_ALWAYS, 0.0f, 0.0f, 0.0f, 1.0f);

	blendState = std::make_unique<Descartes::BlendState>(device, BLEND_MODE::ADD);
}

void Bloom::generate(ID3D11DeviceContext* immediate_context, ID3D11ShaderResourceView* hdr_texture)
{
	samplerStates[POINT]->Activate(immediate_context, 0, false);
	samplerStates[LINEAR]->Activate(immediate_context, 1, false);
	samplerStates[ANISOTROPIC]->Activate(immediate_context, 2, false);

	constantBuffer->Activate(immediate_context, 0, false, true);

	ID3D11ShaderResourceView* nullShaderResourceView = 0;
	//Extracting bright color
	glowExtraction->Clear(immediate_context);
	glowExtraction->Activate(immediate_context);
	glowExtractionPS->Activate(immediate_context);
	immediate_context->PSSetShaderResources(0, 1, &hdr_texture);
	Descartes::FullscreenQuad::Blit(immediate_context);
	immediate_context->PSSetShaderResources(0, 1, &nullShaderResourceView);
	glowExtractionPS->DeActivate(immediate_context);
	glowExtraction->DeActivate(immediate_context);

	samplerStates[POINT]->DeActivate(immediate_context);
	samplerStates[LINEAR]->DeActivate(immediate_context);
	samplerStates[ANISOTROPIC]->DeActivate(immediate_context);

	//Gaussian blur
	//Efficient Gaussian blur with linear sampling
	//http://rastergrid.com/blog/2010/09/efficient-gaussian-blur-with-linear-sampling/
	samplerStates[LINEAR_BORDER]->Activate(immediate_context, 0, false);
	{

		// downsampling
		immediate_context->PSSetShaderResources(0, 1, &hdr_texture);
		gaussianBlur[0][0]->Clear(immediate_context);
		gaussianBlur[0][0]->Activate(immediate_context);
		gaussianBlurDownsamplingPS->Activate(immediate_context);
		Descartes::FullscreenQuad::Blit(immediate_context);
		gaussianBlurDownsamplingPS->DeActivate(immediate_context);
		gaussianBlur[0][0]->DeActivate(immediate_context);

		// ping-pong gaussian blur
		immediate_context->PSSetShaderResources(0, 1, gaussianBlur[0][0]->renderTargetShaderResourceView.GetAddressOf());
		gaussianBlur[0][1]->Clear(immediate_context);
		gaussianBlur[0][1]->Activate(immediate_context);
		gaussianBlurHorizontalPS->Activate(immediate_context);
		Descartes::FullscreenQuad::Blit(immediate_context);
		gaussianBlurHorizontalPS->DeActivate(immediate_context);
		gaussianBlur[0][1]->DeActivate(immediate_context);
		immediate_context->PSSetShaderResources(0, 1, gaussianBlur[0][1]->renderTargetShaderResourceView.GetAddressOf());
		gaussianBlur[0][0]->Clear(immediate_context);
		gaussianBlur[0][0]->Activate(immediate_context);
		gaussianBlurVerticalPS->Activate(immediate_context);
		Descartes::FullscreenQuad::Blit(immediate_context);
		gaussianBlurVerticalPS->DeActivate(immediate_context);
		gaussianBlur[0][0]->DeActivate(immediate_context);
	}
	for (size_t indexOfDownsampled = 1; indexOfDownsampled < numberOfDownsampled; ++indexOfDownsampled)
	{
		// downsampling
		immediate_context->PSSetShaderResources(0, 1, gaussianBlur[indexOfDownsampled - 1][0]->renderTargetShaderResourceView.GetAddressOf());
		gaussianBlur[indexOfDownsampled][0]->Clear(immediate_context);
		gaussianBlur[indexOfDownsampled][0]->Activate(immediate_context);
		gaussianBlurDownsamplingPS->Activate(immediate_context);
		Descartes::FullscreenQuad::Blit(immediate_context);
		gaussianBlurDownsamplingPS->DeActivate(immediate_context);
		gaussianBlur[indexOfDownsampled][0]->DeActivate(immediate_context);
		// ping-pong gaussian blur
		immediate_context->PSSetShaderResources(0, 1, gaussianBlur[indexOfDownsampled][0]->renderTargetShaderResourceView.GetAddressOf());
		gaussianBlur[indexOfDownsampled][1]->Clear(immediate_context);
		gaussianBlur[indexOfDownsampled][1]->Activate(immediate_context);
		gaussianBlurHorizontalPS->Activate(immediate_context);
		Descartes::FullscreenQuad::Blit(immediate_context);
		gaussianBlurHorizontalPS->DeActivate(immediate_context);
		gaussianBlur[indexOfDownsampled][1]->DeActivate(immediate_context);
		immediate_context->PSSetShaderResources(0, 1, gaussianBlur[indexOfDownsampled][1]->renderTargetShaderResourceView.GetAddressOf());
		gaussianBlur[indexOfDownsampled][0]->Clear(immediate_context);
		gaussianBlur[indexOfDownsampled][0]->Activate(immediate_context);
		gaussianBlurVerticalPS->Activate(immediate_context);
		Descartes::FullscreenQuad::Blit(immediate_context);
		gaussianBlurVerticalPS->DeActivate(immediate_context);
		gaussianBlur[indexOfDownsampled][0]->DeActivate(immediate_context);
	}
	samplerStates[LINEAR_BORDER]->DeActivate(immediate_context);
}

void Bloom::Blit(ID3D11DeviceContext* immediate_context)
{
	samplerStates[POINT]->Activate(immediate_context, 0, false);
	samplerStates[LINEAR]->Activate(immediate_context, 1, false);
	samplerStates[ANISOTROPIC]->Activate(immediate_context, 2, false);
	blendState->Activate(immediate_context);
	constantBuffer->Activate(immediate_context, 0, false, true);

	std::vector< ID3D11ShaderResourceView*> shaderResouceView;
	for (size_t indexOfDownsampled = 0; indexOfDownsampled < numberOfDownsampled; ++indexOfDownsampled)
	{
		shaderResouceView.push_back(gaussianBlur[indexOfDownsampled][0]->renderTargetShaderResourceView.Get());
	}
	immediate_context->PSSetShaderResources(0, numberOfDownsampled, &shaderResouceView.at(0));
	gaussianBlurConvolutionPS->Activate(immediate_context);
	Descartes::FullscreenQuad::Blit(immediate_context);
	gaussianBlurConvolutionPS->DeActivate(immediate_context);
	ID3D11ShaderResourceView* null_shader_resource_views[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT] = { 0 };
	immediate_context->PSSetShaderResources(0, D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT, null_shader_resource_views);

	constantBuffer->DeActivate(immediate_context);
	blendState->DeActivate(immediate_context);
	samplerStates[POINT]->DeActivate(immediate_context);
	samplerStates[LINEAR]->DeActivate(immediate_context);
	samplerStates[ANISOTROPIC]->DeActivate(immediate_context);
}


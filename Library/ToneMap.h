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

class ToneMap : Descartes::FullscreenQuad
{
public:
	ToneMap(ID3D11Device* device) : Descartes::FullscreenQuad(device)
	{
		averageLuminance = std::make_unique<Descartes::FrameBuffer>(device, 512, 512, false, 1, DXGI_FORMAT_R16_FLOAT, DXGI_FORMAT_UNKNOWN, true, false, true/*generate_mips*/);
		averageLuminancePS = std::make_unique<Descartes::PixelShader>(device, "Shader/AverageLuminancePS.cso");
		tonemapPS = std::make_unique<Descartes::PixelShader>(device, "Shader/ToneMapPS.cso");
		constantBuffer = std::make_unique<Descartes::ConstantBuffer<ShaderConstants>>(device);
		samplerStates[LINEAR_BORDER] = std::make_unique<Descartes::SamplerState>(device, D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_BORDER, D3D11_COMPARISON_ALWAYS, 0.0f, 0.0f, 0.0f, 1.0f);
		samplerStates[POINT] = std::make_unique<Descartes::SamplerState>(device, D3D11_FILTER_MIN_MAG_MIP_POINT, D3D11_TEXTURE_ADDRESS_WRAP, D3D11_COMPARISON_ALWAYS, 0.0f, 0.0f, 0.0f, 1.0f);
		samplerStates[LINEAR] = std::make_unique<Descartes::SamplerState>(device, D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_WRAP, D3D11_COMPARISON_ALWAYS, 0.0f, 0.0f, 0.0f, 1.0f);
		samplerStates[ANISOTROPIC] = std::make_unique<Descartes::SamplerState>(device, D3D11_FILTER_ANISOTROPIC, D3D11_TEXTURE_ADDRESS_WRAP, D3D11_COMPARISON_ALWAYS, 0.0f, 0.0f, 0.0f, 1.0f);
	}
	virtual ~ToneMap() = default;

	std::unique_ptr<Descartes::FrameBuffer> averageLuminance;
	enum { LINEAR_BORDER, POINT, LINEAR, ANISOTROPIC };
	std::unique_ptr<Descartes::SamplerState> samplerStates[4];
	struct ShaderConstants
	{
		float whitePoint = 0.6f; //The luminance level to use as the upper end of a tone mapping curve.
		float averageGray = 0.25f; //The luminance level to use as the midpoint of a tone mapping curve.

		//Brightness - Contrast Effect
		//The brightness - contrast effect allows you to modify the brightness and contrast of the rendered image.
		//Brightness: The brighness of the image.Ranges from - 1 to 1 (-1 is solid black, 0 no change, 1 solid white).
		//Contrast : The contrast of the image.Ranges from - 1 to 1 (-1 is solid gray, 0 no change, 1 maximum contrast).
		float brightness = 0.0f;
		float contrast = 0.0f;

		//Hue - Saturation Effect
		//The hue - saturation effect allows you to modify the hue and saturation of the rendered image.
		//Hue: The hue of the image.Ranges from - 1 to 1 (-1 is 180 degrees in the negative direction, 0 no change, 1 is 180 degrees in the postitive direction).
		//Saturation : The saturation of the image.Ranges from - 1 to 1 (-1 is solid gray, 0 no change, 1 maximum saturation).
		float hue = 0.0f;
		float saturation = 0.0f;

		//Vignette Effect
		//In photography and optics, vignetting is a reduction of an image's brightness or saturation at the periphery compared to the image center.
		//You can use it to draw attention to the center of the frame. (from Wikipedia)
		//Offset: Controls the offset of the effect.
		//Darkness : Controls the darkness of the effect.
		float offset = 0.0f;
		float darkness = 1.0f;

		// Screen Filter
		DirectX::XMFLOAT3 colourFilter = { 1, 1, 1 };

		//Sepia Effect
		//The Sepia effect makes the image look like an old photograph.
		//Amount: Controls the intensity of the effect. Ranges from 0 to 1.
		float amount = 0.0f;


	};
	std::unique_ptr<Descartes::ConstantBuffer<ShaderConstants>> constantBuffer;

private:
	//ïΩãœãPìxíäèoPS
	std::unique_ptr<Descartes::PixelShader> averageLuminancePS;
	std::unique_ptr<Descartes::PixelShader> tonemapPS;

	float cumulativeTime = 0;
public:
	void Blit(ID3D11DeviceContext* immediateContext, ID3D11ShaderResourceView* hdrTexture, float elapsedTime, bool enableTone = false)
	{
		immediateContext->PSSetShaderResources(0, 1, &hdrTexture);
		samplerStates[POINT]->Activate(immediateContext, 0, false);
		samplerStates[LINEAR]->Activate(immediateContext, 1, false);
		samplerStates[ANISOTROPIC]->Activate(immediateContext, 2, false);
		if (enableTone)
		{
			//create average luminance
			const float interval_time = 1.0f / 30.0f;
			cumulativeTime += elapsedTime;
			if (cumulativeTime > interval_time)
			{
				averageLuminance->Clear(immediateContext);
				averageLuminance->Activate(immediateContext);
				averageLuminancePS->Activate(immediateContext);
				FullscreenQuad::Blit(immediateContext);
				averageLuminancePS->DeActivate(immediateContext);
				averageLuminance->DeActivate(immediateContext);
				immediateContext->GenerateMips(averageLuminance->renderTargetShaderResourceView.Get());
				cumulativeTime -= interval_time;
			}

			//blit to screen
			constantBuffer->Activate(immediateContext, 0, false, true);

			immediateContext->PSSetShaderResources(1, 1, averageLuminance->renderTargetShaderResourceView.GetAddressOf());

			tonemapPS->Activate(immediateContext);
			FullscreenQuad::Blit(immediateContext);
			tonemapPS->DeActivate(immediateContext);
		}
		else
		{
			FullscreenQuad::Blit(immediateContext, true, true, true, true);
		}
		samplerStates[POINT]->DeActivate(immediateContext);
		samplerStates[LINEAR]->DeActivate(immediateContext);
		samplerStates[ANISOTROPIC]->DeActivate(immediateContext);

		ID3D11ShaderResourceView* null_shader_resource_views[2] = {};
		immediateContext->PSSetShaderResources(0, 2, null_shader_resource_views);
	}
};

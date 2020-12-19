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

class Bloom : Descartes::FullscreenQuad
{
public:
	struct ShaderConstants
	{
		float glowExtractionThreshold = 1.000f;
		float blurConvolutionIntensity = 0.100f;
		float lensFlareThreshold = 1.000f;
		float lensFlareGhostDispersal = 0.300f; //dispersion factor
		int numberOfGhosts = 6;
		float lensFlareIntensity = 0.35f;
		int options[2];

		template<class T>
		void serialize(T& archive)
		{
			//TODO:Add 'number_of_ghosts', "lens_flare_intensity"
			archive(CEREAL_NVP(glowExtractionThreshold), CEREAL_NVP(blurConvolutionIntensity), CEREAL_NVP(lensFlareThreshold), CEREAL_NVP(lensFlareGhostDispersal));
		}
	};

	std::unique_ptr<Descartes::FrameBuffer> glowExtraction;

	std::unique_ptr<Descartes::ConstantBuffer<ShaderConstants>> constantBuffer;

	Bloom(ID3D11Device* device, uint32_t width, uint32_t height);
	~Bloom() = default;
	Bloom(Bloom&) = delete;
	Bloom& operator =(Bloom&) = delete;

	void Bloom::generate(ID3D11DeviceContext* immediateContext, ID3D11ShaderResourceView* hdr_texture);
	void Blit(ID3D11DeviceContext* immediateContext);

	// if you change value of 'number_of_downsampled', you must change 'number_of_downsampled' in bloom.hlsli to this same value.
	const static size_t numberOfDownsampled = 3;
	std::unique_ptr<Descartes::FrameBuffer> gaussianBlur[numberOfDownsampled][2];

private:
	enum { LINEAR_BORDER, POINT, LINEAR, ANISOTROPIC };
	std::unique_ptr<Descartes::SamplerState> samplerStates[4];

	std::unique_ptr<Descartes::BlendState> blendState;

	std::unique_ptr<Descartes::PixelShader> glowExtractionPS;
	std::unique_ptr<Descartes::PixelShader> gaussianBlurHorizontalPS;
	std::unique_ptr<Descartes::PixelShader> gaussianBlurVerticalPS;
	std::unique_ptr<Descartes::PixelShader> gaussianBlurConvolutionPS;
	std::unique_ptr<Descartes::PixelShader> gaussianBlurDownsamplingPS;

};
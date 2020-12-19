
/*
The MIT License (MIT)

Copyright (c) 2018 Wicked Engine

Permission is hereby granted, free of charge, to any person obtaining a copy of this 
software and associated documentation files (the "Software"), to deal in the Software 
without restriction, including without limitation the rights to use, copy, modify, 
merge, publish, distribute, sublicense, and/or sell copies of the Software, and to 
permit persons to whom the Software is furnished to do so, subject to the following 
conditions: 

The above copyright notice and this permission notice shall be included in all copies 
or substantial portions of the Software.  

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT 
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF 
CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE 
OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
https://github.com/turanszkij/WickedEngine/blob/master/LICENSE.md
*/

#pragma once
#include "../Library/Model.h"
#include <d3d11.h>
#include <wrl.h>
#include <string>
#include <vector>
#include <directxmath.h>
#include <iostream>
#include <memory>
#include <vector>
#include "GPUParticleBuffers.h"
#include "LoadShader.h"
#include "ShaderInTerop_EmittedPaticle.h"


struct Render
{
	struct IndirectDispatchArgs
	{
		uint32_t ThreadGroupCountX = 0;
		uint32_t ThreadGroupCountY = 0;
		uint32_t ThreadGroupCountZ = 0;
	};
	struct IndirectDrawArgsInstanced
	{
		uint32_t VertexCountPerInstance = 0;
		uint32_t InstanceCount = 0;
		uint32_t StartVertexLocation = 0;
		uint32_t StartInstanceLocation = 0;
	};
};

struct ParticleInfo
{
	float size = 0.0f;
	float random_factor = 1.0f;
	float normal_factor = 1.0f;
	float count = 0.0f;
	float life = 10.0f;
	float random_life = 1.0f;
	float scaleX = 1.0f;
	float scaleY = 1.0f;
	float speed = 1.0f;
	float rotation = 50.0f;
	float motionBlurAmount = 0.0f;
	float color = 1.0f;
};

class EmitParticle
{
private:
	std::string kickOffShaderName = "EmitParticleKickOffCS.cso";
	std::string emitShaderName = "EmitParticleEmitCS.cso";
	std::string finishShaderName = "EmitParticleFinish.cso";
	std::string simulateShaderName = "EmitParticleSimulateCS.cso";

	std::unique_ptr<GPUParticleBuffers::GPUBuffer> particleBuffer;
	std::unique_ptr<GPUParticleBuffers::GPUBuffer> aliveList[2];
	std::unique_ptr<GPUParticleBuffers::GPUBuffer> deadList;

	std::unique_ptr<GPUParticleBuffers::GPUBuffer> counterBuffer;
	std::unique_ptr<GPUParticleBuffers::GPUBuffer> indirectBuffers;
	std::unique_ptr<GPUParticleBuffers::GPUBuffer> constantBuffer;

	Microsoft::WRL::ComPtr<ID3D11VertexShader>    vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>       pixelShader;
	Microsoft::WRL::ComPtr<ID3D11ComputeShader> kickoffShader;
	Microsoft::WRL::ComPtr<ID3D11ComputeShader> emitShader;
	Microsoft::WRL::ComPtr<ID3D11ComputeShader> finishShader;
	Microsoft::WRL::ComPtr<ID3D11ComputeShader> simulateShader;

	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
	Microsoft::WRL::ComPtr<ID3D11BlendState> blendState;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerState;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencilState;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResouceView;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> sampleState;
	D3D11_TEXTURE2D_DESC texture2dDesc;

	GPUParticleBuffers::GPUBuffer* meshEmitBuffer[2];

	float emit = 0.0f;
	int burst = 0;

	bool buffersUpToDate = false;
	uint32_t MAX_PARTICLES = 3000000;

	std::vector<ParticleParam> particlePram;

	void CreateSelfBuffers();
	HRESULT CreateBuffer(D3D11_BUFFER_DESC* pDesc, D3D11_SUBRESOURCE_DATA* pData, GPUParticleBuffers::GPUBuffer* pBuffer);
	HRESULT CreateSRV(GPUParticleBuffers::GPUBuffer* pBuffer);
	HRESULT CreateUAV(GPUParticleBuffers::GPUBuffer* pBuffer);
	void BindUAVs(const GPUParticleBuffers::GPUBuffer* const* resources, uint32_t slot, uint32_t count);
	void BindShaderResources(const GPUParticleBuffers::GPUBuffer* const* resources, uint32_t slot, uint32_t count);
	uint32_t CompressColor( const DirectX::XMFLOAT4& color);
	void BindComputeShader(ID3D11ComputeShader* cs);
	void UnbindUAVs(uint32_t slot, uint32_t num);
	void UnbindShaderResources(uint32_t slot, uint32_t num);
public:
	float size = 0.1f;
	float random_factor = 1.0f;
	float normal_factor = 1.0f;
	float count = 0.0f;
	float life = 10.0f;
	float random_life = 1.0f;
	float scaleX = 1.0f;
	float scaleY = 1.0f;
	float speed = 1.0f;
	float rotation = 50.0f;
	float motionBlurAmount = 0.0f;
	float color = 1;


	void setParticleInfo(ParticleInfo& inf)
	{
		size = inf.size;
		random_factor = inf.random_factor;
		normal_factor = inf.normal_factor;
		count = inf.count;
		life = inf.life;
		random_life = inf.random_life;
		scaleX = inf.scaleX;
		scaleY = inf.scaleY;
		speed = inf.speed;
		rotation = inf.rotation;
		motionBlurAmount = inf.motionBlurAmount;
		color = inf.color;
	}

	float FIXED_TIMESTEP = 1.0f; // -1 : variable timestep; >=0 : fixed timestep

	DirectX::XMFLOAT3 center;

public:
	EmitParticle() : texture2dDesc(), meshEmitBuffer(), center() {}
	~EmitParticle() {}
	void UpdateCPU(DirectX::XMFLOAT3 pos, float dt, uint32_t indices = 0, GPUParticleBuffers::GPUBuffer* indexBuffer = nullptr, GPUParticleBuffers::GPUBuffer* vertexBuffer = nullptr);
	void UpdateCPU(DirectX::XMFLOAT4X4 transform, float dt, uint32_t indices = 0, GPUParticleBuffers::GPUBuffer* indexBuffer = nullptr, GPUParticleBuffers::GPUBuffer* vertexBuffer = nullptr);
	void Burst(int num);
	void Burst(int num, DirectX::XMFLOAT3 pos, float dt, float nowTime, float burstTime);
	void Burst(int num, DirectX::XMFLOAT3 pos, float dt, std::shared_ptr<ModelResource> modelResource, float nowTime, float burstTime);
	void Burst(int num, DirectX::XMFLOAT4X4 transform, float dt, std::shared_ptr<ModelResource> modelResource, float nowTime, float burstTime);
	void Restart();

	// Must have a transform and material component, but mesh is optional
	void Initialize(const wchar_t* fileName, bool enableMeshEmit = false);
	void UpdateGPU();
	void Draw();
	void LoadShaders(bool enableMeshEmit = false);
	void SetMaxParticleCount(uint32_t value);
	void setKickOffShader(std::string name) { kickOffShaderName = name; }
	void setEmitShader(std::string name) { emitShaderName = name; }
	void setFinishShader(std::string name) { finishShaderName = name; }
	void setSimulateShader(std::string name) { simulateShaderName = name; }
};
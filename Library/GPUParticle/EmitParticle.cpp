#define NOMINMAX

#include "EmitParticle.h"
#include "../Library/Camera.h"
#include "../Library/FrameWork.h"
#include "../Library/CommonInclude.h"
#include "../Library/Texture.h"
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <Windows.h>
const void* const __nullBlob[128] = {}; // this is initialized to nullptrs and used to unbind resources!

void EmitParticle::CreateSelfBuffers()
{
	if (buffersUpToDate)
	{
		return;
	}
	buffersUpToDate = true;

	HRESULT hr;

	// reset
	particleBuffer->Reset();
	aliveList[0]->Reset();
	aliveList[1]->Reset();
	deadList->Reset();
	counterBuffer->Reset();
	indirectBuffers->Reset();
	constantBuffer->Reset();


	// Particle buffer
	{
		D3D11_BUFFER_DESC bd = {};
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		bd.StructureByteStride = sizeof(Particle);
		bd.ByteWidth = bd.StructureByteStride * MAX_PARTICLES;

		// Buffer作成
		hr = GPUParticleBuffers::CreateBuffer(&bd, nullptr, particleBuffer.get());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		
		// SRV作成
		hr = CreateSRV(particleBuffer.get());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		// UAV作成
		hr = CreateUAV(particleBuffer.get());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}

	// Alive index lists
	{
		D3D11_BUFFER_DESC bd = {};
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		bd.StructureByteStride = sizeof(uint32_t);
		bd.ByteWidth = bd.StructureByteStride * MAX_PARTICLES;
		
		// aliveList[0]作成
		{
			// Buffer作成
			hr = CreateBuffer(&bd, nullptr, aliveList[0].get());
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

			// SRV作成
			hr = CreateSRV(aliveList[0].get());
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

			// UAV作成
			hr = CreateUAV(aliveList[0].get());
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		}

		// aliveList[1]作成
		{
			// Buffer作成
			hr = CreateBuffer(&bd, nullptr, aliveList[1].get());
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

			// SRV作成
			hr = CreateSRV(aliveList[1].get());
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

			// UAV作成
			hr = CreateUAV(aliveList[1].get());
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		}
	}

	// Dead index list
	{
		D3D11_BUFFER_DESC bd = {};
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		bd.StructureByteStride = sizeof(uint32_t);
		bd.ByteWidth = bd.StructureByteStride * MAX_PARTICLES;

		uint32_t* indices = new uint32_t[MAX_PARTICLES];
		for (uint32_t i = 0; i < MAX_PARTICLES; i++)
		{
			indices[i] = i;
		}

		D3D11_SUBRESOURCE_DATA data = {};
		data.pSysMem = indices;
		
		// Buffer作成
		hr = CreateBuffer(&bd, &data, deadList.get());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		
		// SRV作成
		hr = CreateSRV(deadList.get());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		// UAV作成
		hr = CreateUAV(deadList.get());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		SAFE_DELETE_ARRAY(indices);
	}

	// Particle System statistics
	{
		ParticleCounters counters = {};
		counters.aliveCount = 0;
		counters.deadCount = MAX_PARTICLES;
		counters.realEmitCount = 0;
		counters.aliveCount_afterSimulation = 0;

		D3D11_BUFFER_DESC bd = {};
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
		bd.ByteWidth = sizeof(counters);
		bd.StructureByteStride = sizeof(counters);
		bd.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;
		bd.CPUAccessFlags = 0;
		
		D3D11_SUBRESOURCE_DATA data = {};
		data.pSysMem = &counters;

		// Buffer作成
		hr = CreateBuffer(&bd, &data, counterBuffer.get());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		// SRV作成
		hr = CreateSRV(counterBuffer.get());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		// UAV作成
		hr = CreateUAV(counterBuffer.get());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}


	// Indirect Execution buffer
	{
		D3D11_BUFFER_DESC bd = {};
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
		bd.CPUAccessFlags = 0;
		bd.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
		bd.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS | D3D11_RESOURCE_MISC_DRAWINDIRECT_ARGS;

		bd.ByteWidth =
			sizeof(Render::IndirectDispatchArgs) +
			sizeof(Render::IndirectDispatchArgs) +
			sizeof(Render::IndirectDrawArgsInstanced);

		// Buffer作成
		hr = CreateBuffer(&bd, nullptr, indirectBuffers.get());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		// SRV作成
		hr = CreateSRV(indirectBuffers.get());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		// UAV作成
		hr = CreateUAV(indirectBuffers.get());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}

	// Constant buffer
	{
		D3D11_BUFFER_DESC bd = {};
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
		bd.CPUAccessFlags = 0;
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(EmittedParticleCB);
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags = 0;

		// Buffer作成
		hr = CreateBuffer(&bd, nullptr, constantBuffer.get());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		// SRV作成
		hr = CreateSRV(constantBuffer.get());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		// UAV作成
		hr = CreateUAV(constantBuffer.get());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}
}

void EmitParticle::UpdateCPU(DirectX::XMFLOAT3 pos, float dt, uint32_t indices, GPUParticleBuffers::GPUBuffer* indexBuffer, GPUParticleBuffers::GPUBuffer* vertexBuffer)
{

	CreateSelfBuffers();

	center = pos;

	emit += (float)count * dt;

	emit += burst;
	burst = 0;



	if (&particleBuffer == nullptr)
	{
		return;
	}

	// 後で一括でMapするからVectorで溜めとく
	ParticleParam cb;
	DirectX::XMMATRIX T;
	T = DirectX::XMMatrixTranslation(center.x, center.y, center.z * -1);
	DirectX::XMMATRIX W = DirectX::XMLoadFloat4x4(&IDENTITYMATRIX);
	W *= T;
	DirectX::XMFLOAT4X4 mat;
	DirectX::XMStoreFloat4x4(&mat, W);
	cb.xEmitterWorld = mat;
	cb.xEmitterMeshIndexCount = indices;
	cb.xEmitterMeshVertexPositionStride = sizeof(ModelResource::Vertex_POS);
	cb.xEmitterRandomness = rand() % 1001 * 0.001f;
	cb.xParticleLifeSpan = life;
	cb.xParticleLifeSpanRandomness = random_life;
	cb.xParticleNormalFactor = normal_factor;
	cb.xParticleRandomFactor = random_factor;
	cb.xParticleScaling = scaleX;
	cb.xParticleSpeed = speed;
	cb.xParticleSize = size;
	cb.xParticleMotionBlurAmount = motionBlurAmount;
	cb.xParticleRotation = rotation * XM_PI * 60;
	cb.xParticleColor = color;
	cb.xEmitterOpacity = 1.0f; //  不透明度
	cb.xEmitterMaxParticleCount = MAX_PARTICLES;
	cb.xEmitterFixedTimestep = FIXED_TIMESTEP;

	particlePram.push_back(cb);

	meshEmitBuffer[0] = indexBuffer == nullptr ? nullptr : indexBuffer;
	meshEmitBuffer[1] = vertexBuffer == nullptr ? nullptr : vertexBuffer;

}

void EmitParticle::UpdateCPU(DirectX::XMFLOAT4X4 transform, float dt, uint32_t indices, GPUParticleBuffers::GPUBuffer* indexBuffer, GPUParticleBuffers::GPUBuffer* vertexBuffer)
{

	CreateSelfBuffers();


	emit += (float)count * dt;

	emit += burst;
	burst = 0;



	if (&particleBuffer == nullptr)
	{
		return;
	}

	ParticleParam cb;
	transform._43 *= -1;
	cb.xEmitterWorld = transform;
	cb.xEmitterMeshIndexCount = indices;
	cb.xEmitterMeshVertexPositionStride = sizeof(ModelResource::Vertex_POS);
	cb.xEmitterRandomness = rand() % 1001 * 0.001f;
	cb.xParticleLifeSpan = life;
	cb.xParticleLifeSpanRandomness = random_life;
	cb.xParticleNormalFactor = normal_factor;
	cb.xParticleRandomFactor = random_factor;
	cb.xParticleScaling = scaleX;
	cb.xParticleSpeed = speed;
	cb.xParticleSize = size;
	cb.xParticleMotionBlurAmount = motionBlurAmount;
	cb.xParticleRotation = rotation * XM_PI * 60;
	cb.xParticleColor = color;
	cb.xEmitterOpacity = 1.0f; //  不透明度
	cb.xEmitterMaxParticleCount = MAX_PARTICLES;
	cb.xEmitterFixedTimestep = FIXED_TIMESTEP;

	particlePram.push_back(cb);

	meshEmitBuffer[0] = indexBuffer == nullptr ? nullptr : indexBuffer;
	meshEmitBuffer[1] = vertexBuffer == nullptr ? nullptr : vertexBuffer;

}

void EmitParticle::Burst(int num)
{
	burst += num;
}

void EmitParticle::Burst(int num, DirectX::XMFLOAT3 pos, float dt, float nowTime, float burstTime)
{
	if (burstTime <= nowTime) burst += num;
	UpdateCPU(pos, dt);
	//UpdateGPU();
}

void EmitParticle::Burst(int num, DirectX::XMFLOAT3 pos, float dt,  std::shared_ptr<ModelResource> modelResource, float nowTime, float burstTime)
{
	if(burstTime <= nowTime) burst += num;
	{
		UpdateCPU(pos, dt, modelResource->indicesSize, &modelResource->meshEmitIndexBuffer, &modelResource->vertexBufferPOS);
		//UpdateGPU(&modelResource->m_data->meshes.at(i), &modelResource->meshEmit_indexBuffer.at(i), &modelResource->vertexBuffer_POS.at(i));
	}
}

void EmitParticle::Burst(int num, DirectX::XMFLOAT4X4 transform, float dt,  std::shared_ptr<ModelResource> modelResource, float nowTime, float burstTime)
{
	if(burstTime <= nowTime) burst += num;
	{
		UpdateCPU(transform, dt, modelResource->indicesSize, &modelResource->meshEmitIndexBuffer, &modelResource->vertexBufferPOS);
		//UpdateGPU(&modelResource->m_data->meshes.at(i), &modelResource->meshEmit_indexBuffer.at(i), &modelResource->vertexBuffer_POS.at(i));
	}
}

void EmitParticle::Restart()
{
	buffersUpToDate = false;
}

void EmitParticle::SetMaxParticleCount(uint32_t value)
{
	buffersUpToDate = false;
	MAX_PARTICLES = value;
}

void EmitParticle::UpdateGPU()
{

	if (&particleBuffer == nullptr)
	{
		return;
	}

	// Swap CURRENT alivelist with NEW alivelist
	aliveList[0].swap(aliveList[1]);

	EmittedParticleCB cb;
	{
		// まとめた定数を
		uint size = static_cast<uint>(particlePram.size());
		int i = 0;
		for (auto p : particlePram)
		{
			cb.particleParam[i] = p;
			i++;
		}
		cb.xEmitCount = (uint32_t)emit;
		cb.particleParamSize = size;
		emit = std::max(0.0f, emit - floorf(emit));
	}

	pFrameWork.GetContext()->UpdateSubresource(constantBuffer->buffer.Get(), 0, nullptr, &cb, 0, 0);
	pFrameWork.GetContext()->CSSetConstantBuffers(CB_GETBINDSLOT(EmittedParticleCB), 1, constantBuffer->buffer.GetAddressOf());

	particlePram.clear();

	Camera::GetInstance().MapConstantBuffer();
	Camera::GetInstance().BindConstantBuffer(SceneShaderType::CS);


	{
		GPUParticleBuffers::GPUBuffer* uavs[] =
		{
			particleBuffer.get(),
			aliveList[0].get(), // CURRENT alivelist
			aliveList[1].get(), // NEW alivelist
			deadList.get(),
			counterBuffer.get(),
			indirectBuffers.get(),
		};
		BindUAVs(uavs, 0, arraysize(uavs));

	}

	{
		GPUParticleBuffers::GPUBuffer* resources[] = {
			meshEmitBuffer[0] == nullptr ? nullptr : meshEmitBuffer[0],
			meshEmitBuffer[1] == nullptr ? nullptr : meshEmitBuffer[1]
		};
		BindShaderResources(resources, TEXSLOT_ONDEMAND0, arraysize(resources));
	}

	// kick off updating, set up state
	BindComputeShader(kickoffShader.Get());
	pFrameWork.GetContext()->Dispatch(1, 1, 1);


	// emit the required amount if there are free slots in dead list
	BindComputeShader(emitShader.Get());
	pFrameWork.GetContext()->DispatchIndirect(indirectBuffers->buffer.Get(), ARGUMENTBUFFER_OFFSET_DISPATCHEMIT);


	BindComputeShader(simulateShader.Get());
	pFrameWork.GetContext()->DispatchIndirect(indirectBuffers->buffer.Get(), ARGUMENTBUFFER_OFFSET_DISPATCHSIMULATION);


	{
		ID3D11UnorderedAccessView* pNull = NULL;
		pFrameWork.GetContext()->CSSetUnorderedAccessViews(0, 1, &pNull, (UINT*)&pNull);
		pFrameWork.GetContext()->CSSetUnorderedAccessViews(1, 1, &pNull, (UINT*)&pNull);
		pFrameWork.GetContext()->CSSetUnorderedAccessViews(2, 1, &pNull, (UINT*)&pNull);
		pFrameWork.GetContext()->CSSetUnorderedAccessViews(3, 1, &pNull, (UINT*)&pNull);
		pFrameWork.GetContext()->CSSetUnorderedAccessViews(4, 1, &pNull, (UINT*)&pNull);
		pFrameWork.GetContext()->CSSetUnorderedAccessViews(5, 1, &pNull, (UINT*)&pNull);
	}

	BindComputeShader(finishShader.Get());

	pFrameWork.GetContext()->CSSetShaderResources(0, 1, counterBuffer->SRV.GetAddressOf());

	pFrameWork.GetContext()->CSSetUnorderedAccessViews(0, 1, indirectBuffers->UAV.GetAddressOf(), (UINT*)indirectBuffers->UAV.GetAddressOf());

	pFrameWork.GetContext()->Dispatch(1, 1, 1);
	{

		ID3D11UnorderedAccessView* pNull = NULL;
		pFrameWork.GetContext()->CSSetUnorderedAccessViews(0, 1, &pNull, (UINT*)&pNull);
		pFrameWork.GetContext()->CSSetUnorderedAccessViews(1, 1, &pNull, (UINT*)&pNull);
		pFrameWork.GetContext()->CSSetUnorderedAccessViews(2, 1, &pNull, (UINT*)&pNull);
		pFrameWork.GetContext()->CSSetUnorderedAccessViews(3, 1, &pNull, (UINT*)&pNull);
		pFrameWork.GetContext()->CSSetUnorderedAccessViews(4, 1, &pNull, (UINT*)&pNull);
		pFrameWork.GetContext()->CSSetUnorderedAccessViews(5, 1, &pNull, (UINT*)&pNull);
	}
	{
		ID3D11ShaderResourceView* pNull = NULL;
		pFrameWork.GetContext()->CSSetShaderResources(0, 1, &pNull);
	}

	meshEmitBuffer[0] = nullptr;
	meshEmitBuffer[1] = nullptr;
}


void EmitParticle::Draw()
{
	Camera::GetInstance().MapConstantBuffer();
	Camera::GetInstance().BindConstantBuffer(SceneShaderType::VS);
	Camera::GetInstance().BindConstantBuffer(SceneShaderType::PS);

	pFrameWork.GetContext()->VSSetShader(vertexShader.Get(), nullptr, 0);
	pFrameWork.GetContext()->PSSetShader(pixelShader.Get(), nullptr, 0);

	const float blend_factor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

	pFrameWork.GetContext()->VSSetConstantBuffers(CB_GETBINDSLOT(EmittedParticleCB), 1, constantBuffer->buffer.GetAddressOf());
	pFrameWork.GetContext()->PSSetConstantBuffers(CB_GETBINDSLOT(EmittedParticleCB), 1, constantBuffer->buffer.GetAddressOf());

	//pFrameWork.GetContext()->OMSetBlendState(blendState.Get(), blend_factor, 0xFFFFFFFF);
	pFrameWork.GetContext()->RSSetState(rasterizerState.Get());
	pFrameWork.GetContext()->OMSetDepthStencilState(depthStencilState.Get(), 0);

	pFrameWork.GetContext()->PSSetShaderResources(0, 1, shaderResouceView.GetAddressOf());
	pFrameWork.GetContext()->PSSetSamplers(0, 1, sampleState.GetAddressOf());

	//pFrameWork.GetContext()->IASetVertexBuffers(0, 0, 0, 0, 0);
	//pFrameWork.GetContext()->IASetInputLayout(inputLayout.Get());
	pFrameWork.GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	pFrameWork.GetContext()->VSSetShaderResources(0, 1, particleBuffer->SRV.GetAddressOf());
	pFrameWork.GetContext()->VSSetShaderResources(1, 1, aliveList[1]->SRV.GetAddressOf());

	pFrameWork.GetContext()->DrawInstancedIndirect(indirectBuffers->buffer.Get(), ARGUMENTBUFFER_OFFSET_DRAWPARTICLES);

	{
		ID3D11ShaderResourceView* pNull = NULL;
		pFrameWork.GetContext()->VSSetShaderResources(0, 1, &pNull);
		pFrameWork.GetContext()->VSSetShaderResources(1, 1, &pNull);
	}

}

void EmitParticle::LoadShaders(bool enableMeshEmit)
{
	{
		// 入力レイアウト
		D3D11_INPUT_ELEMENT_DESC input_element_desc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		LoadShader::LoadVertexShader("EmittedParticleVS.cso", vertexShader.GetAddressOf(), input_element_desc, ARRAYSIZE(input_element_desc), inputLayout.GetAddressOf());
	}

	LoadShader::LoadPixelShader("EmitParticlePS.cso", pixelShader.GetAddressOf());

	LoadShader::LoadComputeShader(kickOffShaderName, kickoffShader.GetAddressOf());
	LoadShader::LoadComputeShader(emitShaderName, emitShader.GetAddressOf());
	LoadShader::LoadComputeShader(finishShaderName, finishShader.GetAddressOf());
	LoadShader::LoadComputeShader(simulateShaderName, simulateShader.GetAddressOf());

}

void EmitParticle::Initialize(const wchar_t* fileName, bool enableMeshEmit)
{
	particleBuffer = std::make_unique<GPUParticleBuffers::GPUBuffer>();
	aliveList[0] = std::make_unique<GPUParticleBuffers::GPUBuffer>();
	aliveList[1] = std::make_unique<GPUParticleBuffers::GPUBuffer>();
	deadList = std::make_unique<GPUParticleBuffers::GPUBuffer>();
	counterBuffer = std::make_unique<GPUParticleBuffers::GPUBuffer>();
	indirectBuffers = std::make_unique<GPUParticleBuffers::GPUBuffer>();
	constantBuffer = std::make_unique<GPUParticleBuffers::GPUBuffer>();

	buffersUpToDate = false;

	// テクスチャ作成
	{
		Texture::LoadTextureFromFile(pFrameWork.GetDevice(), fileName, shaderResouceView.GetAddressOf(), &texture2dDesc);
	}

	// サンプラを作成
	{
		D3D11_SAMPLER_DESC desc = {};
		desc.MipLODBias = 0.0f;
		desc.MaxAnisotropy = 1;
		desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		desc.MinLOD = -FLT_MAX;
		desc.MaxLOD = FLT_MAX;
		desc.BorderColor[0] = 1.0f;
		desc.BorderColor[1] = 1.0f;
		desc.BorderColor[2] = 1.0f;
		desc.BorderColor[3] = 1.0f;
		desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		HRESULT hr = pFrameWork.GetDevice()->CreateSamplerState(&desc, sampleState.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}

	{
		D3D11_RASTERIZER_DESC desc;
		::memset(&desc, 0, sizeof(desc));
		desc.FillMode = D3D11_FILL_SOLID;
		desc.CullMode = D3D11_CULL_NONE;
		desc.FrontCounterClockwise = TRUE;
		desc.DepthBias = 0;
		desc.DepthBiasClamp = 0.0f;
		desc.SlopeScaledDepthBias = 0.0f;
		desc.DepthClipEnable = TRUE;
		desc.ScissorEnable = FALSE;
		desc.MultisampleEnable = FALSE;
		desc.AntialiasedLineEnable = FALSE;

		HRESULT hr = pFrameWork.GetDevice()->CreateRasterizerState(&desc, rasterizerState.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}

	{
		D3D11_DEPTH_STENCIL_DESC dsd;
		dsd.DepthEnable = true;
		dsd.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		dsd.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
		dsd.StencilEnable = false;
		HRESULT hr = pFrameWork.GetDevice()->CreateDepthStencilState(&dsd, &depthStencilState);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}

	{
		D3D11_BLEND_DESC desc;
		::memset(&desc, 0, sizeof(desc));
		desc.AlphaToCoverageEnable = false;
		desc.IndependentBlendEnable = false;
		desc.RenderTarget[0].BlendEnable = true;
		desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		HRESULT hr = pFrameWork.GetDevice()->CreateBlendState(&desc, blendState.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}

	LoadShaders(enableMeshEmit);
	CreateSelfBuffers();
}

HRESULT EmitParticle::CreateSRV(GPUParticleBuffers::GPUBuffer* pBuffer)
{
	HRESULT hr = S_OK;
	D3D11_BUFFER_DESC desc = {};
	pBuffer->buffer->GetDesc(&desc);

	if (desc.BindFlags & D3D11_BIND_SHADER_RESOURCE)
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc = {};
		ZeroMemory(&srv_desc, sizeof(srv_desc));

		// This is a Raw Buffer
		if (desc.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS)
		{
			srv_desc.Format = DXGI_FORMAT_R32_TYPELESS;
			srv_desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
			srv_desc.BufferEx.FirstElement = 0;
			srv_desc.BufferEx.Flags = D3D11_BUFFEREX_SRV_FLAG_RAW;
			srv_desc.BufferEx.NumElements = desc.ByteWidth / 4;
		}
		else if (desc.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_STRUCTURED)
		{
			// This is a Structured Buffer

			srv_desc.Format = DXGI_FORMAT_UNKNOWN;
			srv_desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
			srv_desc.BufferEx.FirstElement = 0;
			srv_desc.BufferEx.NumElements = desc.ByteWidth / desc.StructureByteStride;
		}
		else
		{
			// This is a Typed Buffer

			srv_desc.Format = DXGI_FORMAT_UNKNOWN;
			srv_desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
			srv_desc.Buffer.FirstElement = 0;
			srv_desc.Buffer.NumElements = desc.ByteWidth / desc.StructureByteStride;
		}

		hr = pFrameWork.GetDevice()->CreateShaderResourceView(pBuffer->buffer.Get(), &srv_desc, pBuffer->SRV.GetAddressOf());
		assert(SUCCEEDED(hr) && "ShaderResourceView of the GPUBuffer could not be created!");
	}
	return hr;
}

HRESULT EmitParticle::CreateUAV(GPUParticleBuffers::GPUBuffer* pBuffer)
{
	HRESULT hr = S_OK;
	D3D11_BUFFER_DESC desc = {};
	pBuffer->buffer->GetDesc(&desc);

	if (desc.BindFlags & D3D11_BIND_UNORDERED_ACCESS)
	{
		D3D11_UNORDERED_ACCESS_VIEW_DESC uav_desc = {};
		uav_desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		uav_desc.Buffer.FirstElement = 0;

		if (desc.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS)
		{
			uav_desc.Format = DXGI_FORMAT_R32_TYPELESS;
			uav_desc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_RAW;
			uav_desc.Buffer.NumElements = desc.ByteWidth / 4;
		}
		else if (desc.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_STRUCTURED)
		{
			uav_desc.Format = DXGI_FORMAT_UNKNOWN;
			uav_desc.Buffer.NumElements = desc.ByteWidth / desc.StructureByteStride;
		}
		else
		{
			uav_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			uav_desc.Buffer.NumElements = desc.ByteWidth / desc.StructureByteStride;
		}

		hr = pFrameWork.GetDevice()->CreateUnorderedAccessView(pBuffer->buffer.Get(), &uav_desc, pBuffer->UAV.GetAddressOf());
		assert(SUCCEEDED(hr) && "UnorderedAccessView of the GPUBuffer could not be created!");
	}

	return hr;
}

HRESULT EmitParticle::CreateBuffer(D3D11_BUFFER_DESC* pDesc, D3D11_SUBRESOURCE_DATA* pData, GPUParticleBuffers::GPUBuffer* pBuffer)
{
	HRESULT hr = S_OK;
	D3D11_BUFFER_DESC desc = {};
	desc.ByteWidth = pDesc->ByteWidth;
	desc.Usage = pDesc->Usage;
	desc.BindFlags = pDesc->BindFlags;
	desc.CPUAccessFlags = pDesc->CPUAccessFlags;
	desc.MiscFlags = pDesc->MiscFlags;
	desc.StructureByteStride = pDesc->StructureByteStride;

	D3D11_SUBRESOURCE_DATA* data = nullptr;
	if (pData != nullptr)
	{
		data = new D3D11_SUBRESOURCE_DATA[1];
		for (uint32_t slice = 0; slice < 1; ++slice)
		{
			data[slice] = pData[slice];
		}
	}

	hr = pFrameWork.GetDevice()->CreateBuffer(&desc, data, pBuffer->buffer.GetAddressOf());
	SAFE_DELETE_ARRAY(data);
	assert(SUCCEEDED(hr) && "GPUBuffer creation failed!");

	return hr;
}

void EmitParticle::BindUAVs(const GPUParticleBuffers::GPUBuffer* const* resources, uint32_t slot, uint32_t count)
{
	assert(slot + count <= 7);
	ID3D11UnorderedAccessView* uavs[7];
	for (uint32_t i = 0; i < count; ++i)
	{
		uavs[i] = resources[i] != nullptr ? (ID3D11UnorderedAccessView*)resources[i]->UAV.Get() : nullptr;
	}

	pFrameWork.GetContext()->CSSetUnorderedAccessViews(static_cast<uint32_t>(slot), static_cast<uint32_t>(count), uavs, nullptr);
	
}

void EmitParticle::BindShaderResources(const GPUParticleBuffers::GPUBuffer* const* resources, uint32_t slot, uint32_t count)
{
	assert(count <= 16);
	ID3D11ShaderResourceView* srvs[16];
	for (uint32_t i = 0; i < count; ++i)
	{
		srvs[i] = resources[i] != nullptr ? (ID3D11ShaderResourceView*)resources[i]->SRV.Get() : nullptr;
	}

	pFrameWork.GetContext()->CSSetShaderResources(slot, count, srvs);

}

uint32_t EmitParticle::CompressColor(const XMFLOAT4& color)
{
	uint32_t retval = 0;

	retval |= (uint32_t)((uint8_t)(saturate(color.x) * 255.0f) << 0);
	retval |= (uint32_t)((uint8_t)(saturate(color.y) * 255.0f) << 8);
	retval |= (uint32_t)((uint8_t)(saturate(color.z) * 255.0f) << 16);
	retval |= (uint32_t)((uint8_t)(saturate(color.w) * 255.0f) << 24);

	return retval;
}

void EmitParticle::BindComputeShader(ID3D11ComputeShader* cs)
{
	ID3D11ComputeShader* _cs = cs == nullptr ? nullptr : cs;
	pFrameWork.GetContext()->CSSetShader(_cs, nullptr, 0);
}

void EmitParticle::UnbindShaderResources(uint32_t slot, uint32_t num)
{
	assert(num <= arraysize(__nullBlob) && "Extend nullBlob to support more resource unbinding!");
	pFrameWork.GetContext()->PSSetShaderResources(slot, num, (ID3D11ShaderResourceView**)__nullBlob);
	pFrameWork.GetContext()->VSSetShaderResources(slot, num, (ID3D11ShaderResourceView**)__nullBlob);
	pFrameWork.GetContext()->GSSetShaderResources(slot, num, (ID3D11ShaderResourceView**)__nullBlob);
	pFrameWork.GetContext()->HSSetShaderResources(slot, num, (ID3D11ShaderResourceView**)__nullBlob);
	pFrameWork.GetContext()->DSSetShaderResources(slot, num, (ID3D11ShaderResourceView**)__nullBlob);
	pFrameWork.GetContext()->CSSetShaderResources(slot, num, (ID3D11ShaderResourceView**)__nullBlob);
}

void EmitParticle::UnbindUAVs(uint32_t slot, uint32_t num)
{
	assert(num <= arraysize(__nullBlob) && "Extend nullBlob to support more resource unbinding!");
	pFrameWork.GetContext()->CSSetUnorderedAccessViews(slot, num, (ID3D11UnorderedAccessView**)__nullBlob, 0);
}
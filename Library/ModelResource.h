// UNIT.02
#pragma once

#include <memory>
#include <d3d11.h>
#include <wrl.h>
#include "ModelData.h"
#include "GPUParticle\GPUParticleBuffers.h"

class ModelResource
{
public:
	ModelResource(ID3D11Device* device, std::unique_ptr<ModelData> data, bool enableMeshEmit = false);
	~ModelResource() {}

	struct Material
	{
		DirectX::XMFLOAT4 color = { 0.8f, 0.8f, 0.8f, 1.0f };
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shadeResourceView;
	};

	struct Subset
	{
		u_int		startIndex = 0;
		u_int		indexCount = 0;
		Material*	material;
	};

	struct Mesh
	{
		Microsoft::WRL::ComPtr<ID3D11Buffer>	vertexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer>	indexBuffer;
		std::vector<Subset>						subsets;

		int										nodeIndex;
		std::vector<int>						nodeIndices;
		std::vector<DirectX::XMFLOAT4X4*>		inverseTransforms;
	};

	struct  Vertex_POS
	{
		DirectX::XMFLOAT3 pos = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
		DirectX::XMFLOAT3 nor = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	};


	const std::vector<Mesh>& GetMeshes() const { return meshes; }
	const std::vector<ModelData::Node>& GetNodes() const { return modelData->nodes; }
	const std::vector<ModelData::Animation>& GetAnimations() const { return modelData->animations; }
	bool GetIndexFormat();

	GPUParticleBuffers::GPUBuffer vertexBufferPOS;
	GPUParticleBuffers::GPUBuffer meshEmitIndexBuffer;
	uint32_t indicesSize = 0;
	std::unique_ptr<ModelData>	modelData;
private:
	std::vector<Material>	materials;
	std::vector<Mesh>		meshes;


};

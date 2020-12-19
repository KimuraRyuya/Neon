
#include "WICTextureLoader.h"
#include "Logger.h"
#include "ModelResource.h"
#include "Misc.h"
#include "FrameWork.h"
#include "CommonInclude.h"
#include <iostream>
#include <algorithm>

ModelResource::ModelResource(ID3D11Device* device, std::unique_ptr<ModelData> data, bool enableMeshEmit)
{
	modelData = std::move(data);
	// マテリアル
	materials.resize(modelData->materials.size());
	for (size_t material_index = 0; material_index < materials.size(); ++material_index)
	{
		auto&& src = modelData->materials.at(material_index);
		auto&& dst = materials.at(material_index);

		dst.color = src.diffuseColor;

		// テクスチャ読み込み
		if (!src.textureFilename.empty())
		{
			size_t length;
			wchar_t filename[256];
			::mbstowcs_s(&length, filename, 256, src.textureFilename.c_str(), _TRUNCATE);

			Microsoft::WRL::ComPtr<ID3D11Resource> resource;
			HRESULT hr = DirectX::CreateWICTextureFromFile(device, filename, resource.GetAddressOf(), dst.shadeResourceView.GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		}
	}

	// メッシュ
	meshes.resize(modelData->meshes.size());
	for (size_t mesh_index = 0; mesh_index < meshes.size(); ++mesh_index)
	{
		auto&& src = modelData->meshes.at(mesh_index);
		auto&& dst = meshes.at(mesh_index);

		// 頂点バッファ
		{
			D3D11_BUFFER_DESC buffer_desc = {};
			D3D11_SUBRESOURCE_DATA subresource_data = {};

			buffer_desc.ByteWidth = static_cast<UINT>(sizeof(ModelData::Vertex) * src.vertices.size());
			//buffer_desc.Usage = D3D11_USAGE_DEFAULT;
			buffer_desc.Usage = D3D11_USAGE_IMMUTABLE;
			buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			buffer_desc.CPUAccessFlags = 0;
			buffer_desc.MiscFlags = 0;
			buffer_desc.StructureByteStride = 0;
			subresource_data.pSysMem = src.vertices.data();
			subresource_data.SysMemPitch = 0; 
			subresource_data.SysMemSlicePitch = 0;

			HRESULT hr = device->CreateBuffer(&buffer_desc, &subresource_data, dst.vertexBuffer.GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		}

		// インデックスバッファ
		{
			D3D11_BUFFER_DESC buffer_desc = {};
			D3D11_SUBRESOURCE_DATA subresource_data = {};

			buffer_desc.ByteWidth = static_cast<UINT>(sizeof(u_int) * src.indices.size());
			//buffer_desc.Usage = D3D11_USAGE_DEFAULT;
			buffer_desc.Usage = D3D11_USAGE_IMMUTABLE;
			buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
			buffer_desc.CPUAccessFlags = 0;
			buffer_desc.MiscFlags = 0;
			buffer_desc.StructureByteStride = 0;
			subresource_data.pSysMem = src.indices.data();
			subresource_data.SysMemPitch = 0; //Not use for index buffers.
			subresource_data.SysMemSlicePitch = 0; //Not use for index buffers.
			HRESULT hr = device->CreateBuffer(&buffer_desc, &subresource_data, dst.indexBuffer.GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		}

		dst.nodeIndex = src.nodeIndex;

		// サブセット
		dst.subsets.resize(src.subsets.size());
		for (size_t subset_index = 0; subset_index < src.subsets.size(); ++subset_index)
		{
			auto&& src_subset = src.subsets.at(subset_index);
			auto&& dst_subset = dst.subsets.at(subset_index);

			dst_subset.startIndex = src_subset.startIndex;
			dst_subset.indexCount = src_subset.indexCount;
			dst_subset.material = &materials.at(src_subset.materialIndex);
		}

		// ボーン変換行列用
		dst.nodeIndices.resize(src.nodeIndices.size());
		::memcpy(dst.nodeIndices.data(), src.nodeIndices.data(), sizeof(int) * dst.nodeIndices.size());

		dst.inverseTransforms.resize(src.inverseTransforms.size());
		for (size_t index = 0; index < dst.inverseTransforms.size(); ++index)
		{
			dst.inverseTransforms.at(index) = &src.inverseTransforms.at(index).inverseTransforms;
		}
	}

	if (!enableMeshEmit) return;
	HRESULT hr;

	std::vector<Vertex_POS> tmpV;
	for (auto& m : modelData->meshes)
	{
		for (auto& v : m.vertices)
		{
			Vertex_POS _v;
			_v.pos = v.position;
			_v.nor = v.normal;
			tmpV.emplace_back(_v);
		}
		indicesSize += m.indices.size();
	}

	// Create index buffer GPU data:
	{
		uint32_t counter = 0;
		uint8_t stride;
		void* gpuIndexData;
		if (!GetIndexFormat())
		{
			gpuIndexData = new uint32_t[indicesSize];
			stride = sizeof(uint32_t);

			for(int i = 0; i < indicesSize; i++)
			{
				static_cast<uint32_t*>(gpuIndexData)[counter++] = static_cast<uint32_t>(i);
			}

		}
		else
		{
			gpuIndexData = new uint16_t[indicesSize];
			stride = sizeof(uint16_t);

			for (int i = 0; i < indicesSize; i++)
			{
				static_cast<uint16_t*>(gpuIndexData)[counter++] = static_cast<uint16_t>(i);
			}

		}


		D3D11_BUFFER_DESC bd;
		bd.Usage = D3D11_USAGE_IMMUTABLE;
		bd.CPUAccessFlags = 0;
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER | D3D11_BIND_SHADER_RESOURCE;
		bd.MiscFlags = 0;
		bd.StructureByteStride = stride;
		DXGI_FORMAT format = GetIndexFormat() == true ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;

		D3D11_SUBRESOURCE_DATA InitData;
		InitData.pSysMem = gpuIndexData;
		bd.ByteWidth = (uint32_t)(stride * indicesSize);

		hr = GPUParticleBuffers::CreateBuffer(&bd, &InitData, &meshEmitIndexBuffer);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		hr = GPUParticleBuffers::CreateSRV(&bd, &meshEmitIndexBuffer, format);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		hr = GPUParticleBuffers::CreateUAV(&bd, &meshEmitIndexBuffer, format);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		SAFE_DELETE_ARRAY(gpuIndexData);
	}
	{
		D3D11_BUFFER_DESC bd;
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.CPUAccessFlags = 0;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_SHADER_RESOURCE;
		bd.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;
		bd.ByteWidth = (uint32_t)(sizeof(Vertex_POS) * tmpV.size());

		D3D11_SUBRESOURCE_DATA InitData;
		InitData.pSysMem = tmpV.data();

		hr = GPUParticleBuffers::CreateBuffer(&bd, &InitData, &vertexBufferPOS);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		hr = GPUParticleBuffers::CreateSRV(&bd, &vertexBufferPOS);

		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		hr = GPUParticleBuffers::CreateUAV(&bd, &vertexBufferPOS);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	}

}

bool ModelResource::GetIndexFormat()
{
	return modelData->meshes.size() > 65535 ? false : true;
}
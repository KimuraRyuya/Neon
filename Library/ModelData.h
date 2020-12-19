#pragma once

#include <string>
#include <vector>
#include <directxmath.h>

#include <iostream>
#include <fstream>
#include <cereal/cereal.hpp>
#include <cereal/archives/binary.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/map.hpp>
#include <cereal/types/map.hpp>
#include <cereal/types/string.hpp>

struct ModelData
{
	struct Node
	{
		std::string			name;
		int					parentIndex;
		DirectX::XMFLOAT3	scale;
		DirectX::XMFLOAT4	rotate;
		DirectX::XMFLOAT3	translate;

		Node() : parentIndex(0), scale(0, 0, 0), rotate(0, 0, 0, 0), translate(0, 0, 0) {}

		template<class T>
		void serialize(T &archive)
		{
			archive
			(
				name,
				parentIndex,
				scale.x, scale.y, scale.z,
				rotate.x, rotate.y, rotate.z, rotate.w,
				translate.x, translate.y, translate.z
			);
		}
	};

	struct Vertex
	{
		DirectX::XMFLOAT3	position;
		DirectX::XMFLOAT3	normal;
		DirectX::XMFLOAT2	texcoord;
		DirectX::XMFLOAT4	boneWeight;
		DirectX::XMUINT4	boneIndex;

		template<class T>
		void serialize(T &archive)
		{
			archive
			(
				position.x, position.y, position.z,
				normal.x, normal.y, normal.z,
				texcoord.x, texcoord.y,
				boneWeight.x, boneWeight.y, boneWeight.z, boneWeight.w,
				boneIndex.x, boneIndex.y, boneIndex.z, boneIndex.w
			);
		}
	};

	struct Subset
	{
		int					materialIndex;
		int					startIndex;
		int					indexCount;

		template<class T>
		void serialize(T &archive)
		{
			archive
			(
				materialIndex,
				startIndex,
				indexCount
			);
		}

	};

	struct InverseTransform
	{
		DirectX::XMFLOAT4X4 inverseTransforms;

		template<class T>
		void serialize(T &archive)
		{
			archive
			(
				inverseTransforms._11, inverseTransforms._12, inverseTransforms._13, inverseTransforms._14,
				inverseTransforms._21, inverseTransforms._22, inverseTransforms._23, inverseTransforms._24,
				inverseTransforms._31, inverseTransforms._32, inverseTransforms._33, inverseTransforms._34,
				inverseTransforms._41, inverseTransforms._42, inverseTransforms._43, inverseTransforms._44
			);
		}
	};

	struct Mesh
	{
		std::vector<Vertex>	vertices;
		std::vector<int>	indices;
		std::vector<Subset>	subsets;

		int					nodeIndex;

		std::vector<int>					nodeIndices;
		std::vector<InverseTransform>	inverseTransforms;

		Mesh() : nodeIndex(NULL) {}

		template<class T>
		void serialize(T &archive)
		{
			archive
			(
				vertices,
				indices,
				subsets,
				nodeIndex,
				nodeIndices,
				inverseTransforms
			);
		}

	};

	struct Material
	{
		DirectX::XMFLOAT4	diffuseColor;
		DirectX::XMFLOAT4	ambientColor;
		DirectX::XMFLOAT4	specularColor;
		DirectX::XMFLOAT4	emissiveColor;
		std::string			textureFilename;

		Material() : diffuseColor(0, 0, 0, 0), ambientColor(0, 0, 0, 0), specularColor(0, 0, 0, 0), emissiveColor(0, 0, 0, 0) {}

		template<class T>
		void serialize(T &archive)
		{
			archive
			(
				diffuseColor.x, diffuseColor.y, diffuseColor.z, diffuseColor.w,
				ambientColor.x, ambientColor.y, ambientColor.z, ambientColor.w,
				specularColor.x, specularColor.y, specularColor.z, specularColor.w,
				emissiveColor.x, emissiveColor.y, emissiveColor.z, emissiveColor.w,
				textureFilename
			);
		}

	};

	struct NodeKeyData
	{
		DirectX::XMFLOAT3	scale;
		DirectX::XMFLOAT4	rotate;
		DirectX::XMFLOAT3	translate;

		template<class T>
		void serialize(T &archive)
		{
			archive
			(
				scale.x, scale.y, scale.z,
				rotate.x, rotate.y, rotate.z, rotate.w,
				translate.x, translate.y, translate.z
			);
		}

	};

	struct Keyframe
	{
		float						seconds;
		std::vector<NodeKeyData>	nodeKeys;

		Keyframe() : seconds(0) {}

		template<class T>
		void serialize(T &archive)
		{
			archive
			(
				seconds,
				nodeKeys
			);
		}
	};
	struct Animation
	{
		float						secondsLength;
		std::vector<Keyframe>		keyframes;

		Animation() : secondsLength(0) {}

		template<class T>
		void serialize(T &archive)
		{
			archive
			(
				secondsLength,
				keyframes
			);
		}
	};

	std::vector<Node>		nodes;
	std::vector<Mesh>		meshes;
	std::vector<Material>	materials;
	std::vector<Animation>	animations;

	template<class Archive>
	void serialize(Archive & archive)
	{
		archive(nodes, meshes, materials, animations);
	}

	void CreateBinaryFile(const char* filename)
	{
		// Serialize to 'fbx_filename + extension' file.
		std::ofstream ofs;
		ofs.open((std::string(filename) + ".bin").c_str(), std::ios::binary);
		cereal::BinaryOutputArchive o_archive(ofs);
		o_archive(*this);
	}

	void LoadBinaryFile(const char* filename)
	{
		// Deserialize from 'fbx_filename + extension' file.
		std::ifstream ifs;
		ifs.open((std::string(filename) + ".bin").c_str(), std::ios::binary);
		cereal::BinaryInputArchive i_archive(ifs);
		i_archive(*this);
	}


};
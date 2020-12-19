#pragma once

#include <memory>
#include <d3d11.h>
#include "ModelResource.h"

class Model
{
public:
	Model(std::shared_ptr<ModelResource>& resource);
	~Model() {}

	struct Node
	{
		const char*			name;
		Node*				parent;
		DirectX::XMFLOAT3	scale;
		DirectX::XMFLOAT4	rotate;
		DirectX::XMFLOAT3	translate;
		DirectX::XMFLOAT4X4	localTransform;
		DirectX::XMFLOAT4X4	worldTransform;
	};

	// アニメーション
	bool IsPlayAnimation() const { return currentAnimation >= 0; }
	void PlayAnimation(int animation_index, bool loop = false);
	void UpdateAnimation(float elapsed_time);

	// 行列計算
	void CalculateLocalTransform();
	void CalculateWorldTransform(const DirectX::XMMATRIX& world_transform);

	const std::vector<Node>& GetNodes() const { return nodes; }
	const ModelResource* GetModelResource() const { return modelResource.get(); }

	DirectX::XMFLOAT4X4 GetWorldMatrix() { return world; }
	void SetWorldMatrix(DirectX::XMFLOAT4X4 _w) { world = _w; }

private:
	std::shared_ptr<ModelResource>	modelResource;
	std::vector<Node>				nodes;
	int								currentAnimation = -1;
	float							currentSeconds = 0.0f;
	bool							loopAnimation = false;
	bool							endAnimation = false;

	DirectX::XMFLOAT4X4 world;

	//Renderer
	static const int MaxBones = 64;

	struct CbMesh
	{
		DirectX::XMFLOAT4X4	boneTransforms[MaxBones];
	};

	struct CbSubset
	{
		DirectX::XMFLOAT4	materialColor;
	};


	Microsoft::WRL::ComPtr<ID3D11Buffer>			cbScene;
	Microsoft::WRL::ComPtr<ID3D11Buffer>			cbMesh;
	Microsoft::WRL::ComPtr<ID3D11Buffer>			cbSubset;

	Microsoft::WRL::ComPtr<ID3D11VertexShader>	vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>		pixelShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>		inputLayout;

	Microsoft::WRL::ComPtr<ID3D11BlendState>		blendState;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState>	rasterizerState;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState>	depthStencilState;

	Microsoft::WRL::ComPtr<ID3D11SamplerState>		samplerState;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	dummySRV;


};

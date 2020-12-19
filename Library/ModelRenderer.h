#pragma once

#include <memory>
#include <d3d11.h>
#include "Model.h"

class ModelRenderer
{
public:
	ModelRenderer(ID3D11Device* device);
	~ModelRenderer() {}

	void Begin(ID3D11DeviceContext* immediateContext);
	void Draw(ID3D11DeviceContext* immediateContext, Model& model, DirectX::XMFLOAT4 luminance);
	void End(ID3D11DeviceContext* immediateContext);

private:
	static const int MaxBones = 32;

	struct CbScene
	{
		DirectX::XMFLOAT4X4	world;
		DirectX::XMFLOAT4 luminance;
	};

	struct CbMesh
	{
		DirectX::XMFLOAT4X4	boneTransforms[MaxBones];
	};

	struct CbSubset
	{
		DirectX::XMFLOAT4	materialColor;
	};

	struct CbForPerFrame
	{
		DirectX::XMFLOAT4	lightColor;			// ライトの色
		DirectX::XMFLOAT4	lightDir;				// ライトの方向
		DirectX::XMFLOAT4	ambientColor;	//  環境光
		DirectX::XMFLOAT4	eyePos;				// カメラの位置
	};

	Microsoft::WRL::ComPtr<ID3D11Buffer>			cbScene;
	Microsoft::WRL::ComPtr<ID3D11Buffer>			cbMesh;
	Microsoft::WRL::ComPtr<ID3D11Buffer>			cbSubset;
	Microsoft::WRL::ComPtr<ID3D11Buffer>			cbHighlight;

	Microsoft::WRL::ComPtr<ID3D11VertexShader>		vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>		pixelShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>		inputLayout;

	Microsoft::WRL::ComPtr<ID3D11BlendState>		blendState;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState>	rasterizerState;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState>	depthStencilState;

	Microsoft::WRL::ComPtr<ID3D11SamplerState>		samplerState;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	dummySrv;
};

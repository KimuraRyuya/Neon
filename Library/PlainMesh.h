#pragma once
#include <DirectXMath.h>
#include "FrameBuffer.h"
#include "Rasterizer.h"
#include "Shader.h"
#include "ConstantBuffer.h"
#include "RenderState.h"
#include "Texture.h"

//--------------------------------------------------------------------
//	PlainMesh�N���X
//--------------------------------------------------------------------

class PlainMesh
{
private:
	struct VERTEX
	{
		XMFLOAT3 Pos;	//�ʒu
		XMFLOAT3 Normal;//�@��
		XMFLOAT2 Tex;	//UV���W
	};

	Microsoft::WRL::ComPtr<ID3D11Buffer> VertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> defTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> normalTexture;
	D3D11_TEXTURE2D_DESC texture2dDesc;

	std::unique_ptr<descartes::sampler_state> sampler_states[2];
	std::unique_ptr<descartes::blend_state> blend_state;

	Microsoft::WRL::ComPtr<ID3D11VertexShader>		vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>		pixelShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>		inputLayout;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState>	solidRasterizerState;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencilState;

	int iNumVertices;	// ���_���@
	Microsoft::WRL::ComPtr<ID3D11Buffer> IndexBuffer;
	int iNumIndices;	// �C���f�b�N�X���@

	Microsoft::WRL::ComPtr<ID3D11Buffer> ConstantBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> ConstantWaveBuffer;

	struct CBWaveParam
	{
		//�g�̑����E����
		DirectX::XMFLOAT2 wave1;
		DirectX::XMFLOAT2 wave2;
		//�g�̑傫��
		float wave_size;
		//�g�ɂ��h�炬
		float fluctuation;
		float dummy0;
		float dummy1;
	};

	CBWaveParam wavePram;

	struct ConstantBufferForPerMesh
	{
		DirectX::XMFLOAT4X4 world;
		DirectX::XMFLOAT4X4 matWVP;
	};
	// ���
	DirectX::XMFLOAT3 pos = { 0.0f,0.0f,0.0f };
	DirectX::XMFLOAT3 scale = { 1.0f, 1.0f, 1.0f };
	DirectX::XMFLOAT3 angle = { 0.0f, 0.0f, 0.0f };
	DirectX::XMFLOAT4X4 WorldMatrix;

public:
	PlainMesh(int row, int col);

	~PlainMesh();
	void Update();
	void Render(ID3D11ShaderResourceView* srv,D3D_PRIMITIVE_TOPOLOGY topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	void SetPos(const DirectX::XMFLOAT3& pos) { this->pos = pos; }
	void SetScale(const DirectX::XMFLOAT3& scale) { this->scale = scale; }
	void SetAngle(const DirectX::XMFLOAT3& angle) { this->angle = angle; }



};
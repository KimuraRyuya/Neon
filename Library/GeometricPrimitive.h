#pragma once

#include <d3d11.h>
#include <DirectXMath.h>

#include <windows.h>
#include <tchar.h>
#include <sstream>
#include <memory>

#include <cassert>

#include <d3d11.h>
#include <wrl.h>

#include "Misc.h"
#include "HighResolutionTimer.h"
#include "GPUParticle/LoadShader.h"

class GeometricPrimitive
{
public:
	struct Vertex
	{
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT3 normal;
	};

	struct Cbuffer
	{
		DirectX::XMFLOAT4X4 wvp; //ワールド・ビュー・プロジェクション合成行列
		DirectX::XMFLOAT4X4 world; //ワールド変換行列
		DirectX::XMFLOAT4 material_color; //材質色
		DirectX::XMFLOAT4 light_direction; //ライト進行方向
	};

	enum GeometricPrimitiveType
	{
		CUBE,
		SPHERE,
		CYLINDER,
	};

public:
	Microsoft::WRL::ComPtr <ID3D11VertexShader> vertexShader;
	Microsoft::WRL::ComPtr <ID3D11PixelShader> pixelShader;
	Microsoft::WRL::ComPtr <ID3D11InputLayout> inputLayout;
	Microsoft::WRL::ComPtr <ID3D11Buffer> vertexBuffer; //（頂点バッファ）
	Microsoft::WRL::ComPtr <ID3D11Buffer> indexBuffer; //（インデックスバッファ）
	Microsoft::WRL::ComPtr <ID3D11Buffer> constantBuffer; //（定数バッファ）
	Microsoft::WRL::ComPtr <ID3D11RasterizerState> wireframeRasterizerState; //（線描画）
	Microsoft::WRL::ComPtr <ID3D11RasterizerState> solidRasterizerState; //（塗りつぶし描画）
	Microsoft::WRL::ComPtr <ID3D11DepthStencilState> depthStencilState;

	int numIndex;

public:
	GeometricPrimitive(ID3D11Device* device, int type, bool isCreateBottom = false, DirectX::XMFLOAT3* scale = 0);
	~GeometricPrimitive() {}

	void Render(
		ID3D11DeviceContext *, //デバイスコンテキスト
		const DirectX::XMFLOAT4X4 &, //ワールド・ビュー・プロジェクション合成行列
		const DirectX::XMFLOAT4X4 &, //ワールド変換行列
		const DirectX::XMFLOAT4 &, //ライト進行方向
		const DirectX::XMFLOAT4 &, //材質色
		bool //線・塗りつぶし描画フラグ
	);

private:
	void GeometricCube(ID3D11Device* device, DirectX::XMFLOAT3 scale, bool isCreateBottom);
	void GeometricCylinder(ID3D11Device* device);
	void GeometricSphere(ID3D11Device* device, u_int slices = 16, u_int stacks = 16);

	void CreateBuffer(ID3D11Device *device, Vertex* v, unsigned int* i, int numV, int numI);
};
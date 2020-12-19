#include "FrameWork.h"
#include "Camera.h"
#include "PlainMesh.h"
#include <imgui.h>

PlainMesh::PlainMesh(int row, int col) 
{
	ID3D11Device* device =pFrameWork.GetDevice();

	//波の速さ・向き
	wavePram.wave1 = { 0.005, 0.001 };
	wavePram.wave2 = { -0.005, 0.016 };
	//波の大きさ
	wavePram.wave_size = 7.0f;
	//波による揺らぎ
	wavePram.fluctuation = 0.01f;
	wavePram.dummy0 = 0.0f;
	wavePram.dummy1 = 0.0f;

	sampler_states[0] = std::make_unique<descartes::sampler_state>(device, D3D11_FILTER_ANISOTROPIC, D3D11_TEXTURE_ADDRESS_BORDER, D3D11_COMPARISON_ALWAYS, 0.0f, 0.0f, 0.0f, 1.0f);
	sampler_states[1] = std::make_unique<descartes::sampler_state>(device, D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_WRAP, D3D11_COMPARISON_ALWAYS, 0.0f, 0.0f, 0.0f, 1.0f);
	blend_state = std::make_unique<descartes::blend_state>(device, BLEND_MODE::ALPHA);

	D3D11_INPUT_ELEMENT_DESC input_element_desc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	descartes::create_vs_from_cso(pFrameWork.GetDevice(), "Shader/ReflectSea_vs.cso", vertexShader.GetAddressOf(), inputLayout.GetAddressOf(), input_element_desc, ARRAYSIZE(input_element_desc));

	descartes::create_ps_from_cso(pFrameWork.GetDevice(), "Shader/ReflectSea_ps.cso", pixelShader.GetAddressOf());

	Texture::LoadTextureFromFile(device, L"Data/Texture/NormalSea.png", normalTexture.GetAddressOf(), &texture2dDesc);
	Texture::LoadTextureFromFile(device, L"Data/Texture/Sea.png", defTexture.GetAddressOf(), &texture2dDesc);

	//頂点を定義
	VERTEX* vertices = new VERTEX[(row + 1) * (col + 1)];
	float w = 1.0f / col;
	float h = 1.0f / row;
	for (int y = 0; y < row + 1; y++) {
		for (int x = 0; x < col + 1; x++) {
			int index = (col + 1) * y + x;
			vertices[index].Pos.x = w * x - 0.5f;
			vertices[index].Pos.y = h * y - 0.5f;
			vertices[index].Pos.z = 0.0f;

			vertices[index].Tex.x = 1.0f - w * x;
			vertices[index].Tex.y = 1.0f - h * y;
			vertices[index].Normal = XMFLOAT3(0, 0, 1);
		}
	}

	//上の頂点でバーテックスバッファ作成
	iNumVertices = (row + 1) * (col + 1);
	{
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(VERTEX) * iNumVertices;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA InitData;
		InitData.pSysMem = vertices;
		device->CreateBuffer(&bd, &InitData,
			VertexBuffer.GetAddressOf());
	}

	delete[] vertices;


	UINT16* indices = new UINT16[row * col * 3 * 2];
	int n = 0;
	for (int y = 0; y < row; y++) {
		for (int x = 0; x < col; x++) {
			indices[n + 0] = (col + 1) * y + x;
			indices[n + 1] = (col + 1) * y + x + 1;
			indices[n + 2] = (col + 1) * (y + 1) + x;

			indices[n + 3] = (col + 1) * (y + 1) + x;
			indices[n + 4] = (col + 1) * y + x + 1;
			indices[n + 5] = (col + 1) * (y + 1) + (x + 1);
			n += 6;
		}
	}


	//上の頂点でインデックスバッファ作成
	iNumIndices = row * col * 3 * 2;
	{
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(UINT16) * iNumIndices;
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA InitData;
		InitData.pSysMem = indices;
		device->CreateBuffer(&bd, &InitData,
			IndexBuffer.GetAddressOf());
	}
	delete[] indices;

	// 定数バッファ生成(行列)
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(D3D11_BUFFER_DESC));
	bd.ByteWidth = sizeof(ConstantBufferForPerMesh);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.CPUAccessFlags = 0;

	HRESULT hr = device->CreateBuffer(&bd, NULL, ConstantBuffer.GetAddressOf());
	assert(SUCCEEDED(hr));
	bd.ByteWidth = sizeof(CBWaveParam);
	hr = device->CreateBuffer(&bd, NULL, ConstantWaveBuffer.GetAddressOf());
	assert(SUCCEEDED(hr));

	WorldMatrix = XMFLOAT4X4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);

	//ラスタライザーステートオブジェクト作成************************************
	D3D11_RASTERIZER_DESC rasterizer_desc = {};
	//ソリッド
	ZeroMemory(&rasterizer_desc, sizeof(rasterizer_desc));
	rasterizer_desc.FillMode = D3D11_FILL_SOLID;
	rasterizer_desc.CullMode = D3D11_CULL_NONE;
	rasterizer_desc.FrontCounterClockwise = FALSE;
	rasterizer_desc.DepthClipEnable = TRUE;
	hr = device->CreateRasterizerState(&rasterizer_desc, solidRasterizerState.GetAddressOf());
	assert(!hr && "CreateRasterizerState	Error");
	//**************************************************************************

	//深度ステンシル ステート オブジェクト**************************************
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	depthStencilDesc.DepthEnable = TRUE;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthStencilDesc.StencilEnable = FALSE;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	hr = device->CreateDepthStencilState(&depthStencilDesc, depthStencilState.GetAddressOf());
	assert(!hr && "CreateDepthStencilState		Error");
	//**************************************************************************


}

PlainMesh::~PlainMesh()
{

}

void PlainMesh::Update()
{
	//ワールド変換行列作成
	//XMMATRIX mW = XMMatrixIdentity();
	//拡大
	XMMATRIX s = XMMatrixScaling(scale.x, scale.y, scale.z);
	//回転
	XMMATRIX a = XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
	//移動
	XMMATRIX p = XMMatrixTranslation(pos.x, pos.y, pos.z);
	// 行列の合成
	XMMATRIX mW = s * a * p;
	XMStoreFloat4x4(&WorldMatrix, mW);

#ifdef _DEBUG
	ImGui::Begin("Wave");
	ImGui::DragFloat2(u8"波の速さと向き1", &wavePram.wave1.x);
	ImGui::DragFloat2(u8"波の速さと向き2", &wavePram.wave2.x);
	ImGui::DragFloat(u8"波の大きさ", &wavePram.wave_size);
	ImGui::DragFloat(u8"波の揺らぎ", &wavePram.fluctuation);
	ImGui::End();
#endif // _DEBUG


}

void PlainMesh::Render(ID3D11ShaderResourceView* srv, D3D_PRIMITIVE_TOPOLOGY topology)
{
	ID3D11DeviceContext* device_context = pFrameWork.GetContext();


	device_context->VSSetShader(vertexShader.Get(), NULL, 0);
	device_context->PSSetShader(pixelShader.Get(), NULL, 0);

	//	ラスタライザーの設定
	device_context ->RSSetState(solidRasterizerState.Get());

	//深度テストの設定
	device_context->OMSetDepthStencilState(depthStencilState.Get(), 0);

	ConstantBufferForPerMesh cb;
	cb.world = WorldMatrix;
	XMStoreFloat4x4(&cb.matWVP, XMLoadFloat4x4(&WorldMatrix) * (Camera::GetInstance().GetViewMatrix()) * (Camera::GetInstance().GetProjectionMatrix()));
	device_context->UpdateSubresource(ConstantBuffer.Get(), 0, NULL, &cb, 0, 0);

	device_context->VSSetConstantBuffers(0, 1, ConstantBuffer.GetAddressOf());
	device_context->PSSetConstantBuffers(0, 1, ConstantBuffer.GetAddressOf());

	CBWaveParam cbW;
	//波の速さ・向き
	cbW.wave1 = wavePram.wave1;
	cbW.wave2 = wavePram.wave2;
	//波の大きさ
	cbW.wave_size = wavePram.wave_size;
	//波による揺らぎ
	cbW.fluctuation = wavePram.fluctuation;
	cbW.dummy0 = 0.0f;
	cbW.dummy1 = 0.0f;
	device_context->UpdateSubresource(ConstantWaveBuffer.Get(), 0, NULL, &cbW, 0, 0);

	device_context->VSSetConstantBuffers(1, 1, ConstantWaveBuffer.GetAddressOf());
	device_context->PSSetConstantBuffers(1, 1, ConstantWaveBuffer.GetAddressOf());

	//変更　プリミティブ・トポロジーをセット
	device_context->IASetPrimitiveTopology(topology);

	//バーテックスバッファーをセット
	UINT stride = sizeof(VERTEX);
	UINT offset = 0;
	device_context->IASetVertexBuffers(0, 1, VertexBuffer.GetAddressOf(), &stride, &offset);
	device_context->IASetIndexBuffer(IndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);
	
	//テクスチャ設定
	device_context->PSSetShaderResources(0, 1, defTexture.GetAddressOf());
	device_context->PSSetShaderResources(2, 1, normalTexture.GetAddressOf());
	sampler_states[0]->activate(device_context, 0, false);
	sampler_states[1]->activate(device_context, 2, false);
	blend_state->activate(device_context);

	//レンダリング(インデックス付き)
	device_context->DrawIndexed(iNumIndices, 0, 0);

	sampler_states[0]->deactivate(device_context);
	sampler_states[1]->deactivate(device_context);
	blend_state->deactivate(device_context);

	device_context->VSSetShader(NULL, NULL, 0);
	device_context->HSSetShader(NULL, NULL, 0);
	device_context->DSSetShader(NULL, NULL, 0);
	device_context->GSSetShader(NULL, NULL, 0);
	device_context->PSSetShader(NULL, NULL, 0);
}
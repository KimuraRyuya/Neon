#include "Sprite.h"
#include <vector>
#include "Misc.h"

#include "FrameWork.h"
#include "Texture.h"
#include "Shader.h"

//***********************************************************************************************************************************
//
//			form
//
//***********************************************************************************************************************************

SpriteBatch::SpriteBatch(ModeNo mode)
{
	HRESULT hr = S_OK;
	if (ModeNo::rect_solid == mode)
	{
		vertex_form vertices[] = {
		{ DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT4(1, 1, 1, 1) },
		{ DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT4(1, 1, 1, 1) },
		{ DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT4(1, 1, 1, 1) },
		{ DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT4(1, 1, 1, 1) },
		};
		D3D11_BUFFER_DESC buffer_desc = {};
		buffer_desc.ByteWidth = sizeof(vertices);
		buffer_desc.Usage = D3D11_USAGE_DYNAMIC; // UNIT.03
		buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; // UNIT.03
		buffer_desc.MiscFlags = 0;
		buffer_desc.StructureByteStride = 0;
		D3D11_SUBRESOURCE_DATA subresource_data = {};
		subresource_data.pSysMem = vertices;
		subresource_data.SysMemPitch = 0; //Not use for vertex buffers.
		subresource_data.SysMemSlicePitch = 0; //Not use for vertex buffers.
		hr = pFrameWork.GetDevice()->CreateBuffer(&buffer_desc, &subresource_data, vertexBuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}
	else if (ModeNo::rect_wire == mode)
	{
		vertex_form vertices[] = {
		{ DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT4(1, 1, 1, 1) },
		{ DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT4(1, 1, 1, 1) },
		{ DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT4(1, 1, 1, 1) },
		{ DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT4(1, 1, 1, 1) },
		{ DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT4(1, 1, 1, 1) },
		};
		D3D11_BUFFER_DESC buffer_desc = {};
		buffer_desc.ByteWidth = sizeof(vertices);
		buffer_desc.Usage = D3D11_USAGE_DYNAMIC; // UNIT.03
		buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; // UNIT.03
		buffer_desc.MiscFlags = 0;
		buffer_desc.StructureByteStride = 0;
		D3D11_SUBRESOURCE_DATA subresource_data = {};
		subresource_data.pSysMem = vertices;
		subresource_data.SysMemPitch = 0; //Not use for vertex buffers.
		subresource_data.SysMemSlicePitch = 0; //Not use for vertex buffers.
		hr = pFrameWork.GetDevice()->CreateBuffer(&buffer_desc, &subresource_data, vertexBuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}
	else if (ModeNo::circle_solid == mode)
	{
		vertex_form vertices[1920];
		D3D11_BUFFER_DESC buffer_desc = {};
		buffer_desc.ByteWidth = sizeof(vertices);
		buffer_desc.Usage = D3D11_USAGE_DYNAMIC; // UNIT.03
		buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; // UNIT.03
		buffer_desc.MiscFlags = 0;
		buffer_desc.StructureByteStride = 0;
		D3D11_SUBRESOURCE_DATA subresource_data = {};
		subresource_data.pSysMem = vertices;
		subresource_data.SysMemPitch = 0; //Not use for vertex buffers.
		subresource_data.SysMemSlicePitch = 0; //Not use for vertex buffers.
		hr = pFrameWork.GetDevice()->CreateBuffer(&buffer_desc, &subresource_data, vertexBuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}
	else if(ModeNo::circle_wire == mode)
	{
		vertex_form vertices[1920];
		D3D11_BUFFER_DESC buffer_desc = {};
		buffer_desc.ByteWidth = sizeof(vertices);
		buffer_desc.Usage = D3D11_USAGE_DYNAMIC; // UNIT.03
		buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; // UNIT.03
		buffer_desc.MiscFlags = 0;
		buffer_desc.StructureByteStride = 0;
		D3D11_SUBRESOURCE_DATA subresource_data = {};
		subresource_data.pSysMem = vertices;
		subresource_data.SysMemPitch = 0; //Not use for vertex buffers.
		subresource_data.SysMemSlicePitch = 0; //Not use for vertex buffers.
		hr = pFrameWork.GetDevice()->CreateBuffer(&buffer_desc, &subresource_data, vertexBuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}
	else if (ModeNo::line == mode)
	{
		vertex_form vertices[] = {
		{ DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT4(1, 1, 1, 1) },
		{ DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT4(1, 1, 1, 1) },
		};
		D3D11_BUFFER_DESC buffer_desc = {};
		buffer_desc.ByteWidth = sizeof(vertices);
		buffer_desc.Usage = D3D11_USAGE_DYNAMIC; // UNIT.03
		buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; // UNIT.03
		buffer_desc.MiscFlags = 0;
		buffer_desc.StructureByteStride = 0;
		D3D11_SUBRESOURCE_DATA subresource_data = {};
		subresource_data.pSysMem = vertices;
		subresource_data.SysMemPitch = 0; //Not use for vertex buffers.
		subresource_data.SysMemSlicePitch = 0; //Not use for vertex buffers.
		hr = pFrameWork.GetDevice()->CreateBuffer(&buffer_desc, &subresource_data, vertexBuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	}

	D3D11_INPUT_ELEMENT_DESC input_element_desc[] =
	{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	Descartes::CreateVSFromCso(pFrameWork.GetDevice(), "Shader/sprite_form_vs.cso", vertexShader.GetAddressOf(), inputLayout.GetAddressOf(), input_element_desc, ARRAYSIZE(input_element_desc));
	Descartes::CreatePSFromCso(pFrameWork.GetDevice(), "Shader/sprite_form_ps.cso", pixelShader.GetAddressOf());

	D3D11_RASTERIZER_DESC rasterizer_desc = {}; //https://msdn.microsoft.com/en-us/library/windows/desktop/ff476198(v=vs.85).aspx
	//rasterizer_desc.FillMode = D3D11_FILL_SOLID; //D3D11_FILL_WIREFRAME, D3D11_FILL_SOLID
	rasterizer_desc.FillMode = D3D11_FILL_SOLID; //D3D11_FILL_WIREFRAME, D3D11_FILL_SOLID
	rasterizer_desc.CullMode = D3D11_CULL_NONE; //D3D11_CULL_NONE, D3D11_CULL_FRONT, D3D11_CULL_BACK   
	rasterizer_desc.FrontCounterClockwise = FALSE;
	rasterizer_desc.DepthBias = 0; //https://msdn.microsoft.com/en-us/library/windows/desktop/cc308048(v=vs.85).aspx
	rasterizer_desc.DepthBiasClamp = 0;
	rasterizer_desc.SlopeScaledDepthBias = 0;
	rasterizer_desc.DepthClipEnable = FALSE;
	rasterizer_desc.ScissorEnable = FALSE;
	rasterizer_desc.MultisampleEnable = FALSE;
	rasterizer_desc.AntialiasedLineEnable = FALSE;
	hr = pFrameWork.GetDevice()->CreateRasterizerState(&rasterizer_desc, rasterizerState.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

}

void SpriteBatch::BoxRenderWire(float dx, float dy, float dw, float dh, float angle/*degree*/, float r, float g, float b, float a) const
{
	D3D11_VIEWPORT viewport;
	UINT num_viewports = 1;
	pFrameWork.GetContext()->RSGetViewports(&num_viewports, &viewport);
	float screen_width = viewport.Width;
	float screen_height = viewport.Height;

	// Set each sprite's vertices coordinate to screen space
	// left-top
	float x0 = dx;
	float y0 = dy;
	// right-top
	float x1 = dx + dw;
	float y1 = dy;
	// left-bottom
	float x2 = dx;
	float y2 = dy + dh;
	// right-bottom
	float x3 = dx + dw;
	float y3 = dy + dh;

	// Translate sprite's centre to origin (rotate centre)
	float mx = dx + dw * 0.5f;
	float my = dy + dh * 0.5f;
	x0 -= mx;
	y0 -= my;
	x1 -= mx;
	y1 -= my;
	x2 -= mx;
	y2 -= my;
	x3 -= mx;
	y3 -= my;

	// Rotate each sprite's vertices by angle
	float rx, ry;
	float cos = cosf(angle*0.01745f);
	float sin = sinf(angle*0.01745f);
	rx = x0;
	ry = y0;
	x0 = cos * rx + -sin * ry;
	y0 = sin * rx + cos * ry;
	rx = x1;
	ry = y1;
	x1 = cos * rx + -sin * ry;
	y1 = sin * rx + cos * ry;
	rx = x2;
	ry = y2;
	x2 = cos * rx + -sin * ry;
	y2 = sin * rx + cos * ry;
	rx = x3;
	ry = y3;
	x3 = cos * rx + -sin * ry;
	y3 = sin * rx + cos * ry;

	// Translate sprite's centre to original position
	x0 += mx;
	y0 += my;
	x1 += mx;
	y1 += my;
	x2 += mx;
	y2 += my;
	x3 += mx;
	y3 += my;

	// Convert to NDC space
	x0 = 2.0f*x0 / screen_width - 1.0f;
	y0 = 1.0f - 2.0f*y0 / screen_height;
	x1 = 2.0f*x1 / screen_width - 1.0f;
	y1 = 1.0f - 2.0f*y1 / screen_height;
	x2 = 2.0f*x2 / screen_width - 1.0f;
	y2 = 1.0f - 2.0f*y2 / screen_height;
	x3 = 2.0f*x3 / screen_width - 1.0f;
	y3 = 1.0f - 2.0f*y3 / screen_height;

	HRESULT hr = S_OK;
	D3D11_MAP map = D3D11_MAP_WRITE_DISCARD;
	D3D11_MAPPED_SUBRESOURCE mapped_buffer;
	hr = pFrameWork.GetContext()->Map(vertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_buffer);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	vertex_form *vertices = static_cast<vertex_form *>(mapped_buffer.pData);
	vertices[0].position.x = x0;
	vertices[0].position.y = y0;
	vertices[1].position.x = x1;
	vertices[1].position.y = y1;
	vertices[3].position.x = x2;
	vertices[3].position.y = y2;
	vertices[2].position.x = x3;
	vertices[2].position.y = y3;
	vertices[4].position.x = x0;
	vertices[4].position.y = y0;
	vertices[0].position.z = vertices[1].position.z = vertices[2].position.z = vertices[3].position.z = vertices[4].position.z =0.0f;

	DirectX::XMFLOAT4 color(r, g, b, a);
	vertices[0].color = vertices[1].color = vertices[2].color = vertices[3].color = vertices[4].color =color;

	pFrameWork.GetContext()->Unmap(vertexBuffer.Get(), 0);

	// UNIT.02
	UINT stride = sizeof(vertex_form);
	UINT offset = 0;
	pFrameWork.GetContext()->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);
	pFrameWork.GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);
	pFrameWork.GetContext()->IASetInputLayout(inputLayout.Get());

	pFrameWork.GetContext()->RSSetState(rasterizerState.Get());

	pFrameWork.GetContext()->VSSetShader(vertexShader.Get(), nullptr, 0);
	pFrameWork.GetContext()->PSSetShader(pixelShader.Get(), nullptr, 0);

	pFrameWork.GetContext()->Draw(5, 0);

}
void SpriteBatch::BoxRenderSolid(float dx, float dy, float dw, float dh, float angle/*degree*/, float r, float g, float b, float a) const
{
	D3D11_VIEWPORT viewport;
	UINT num_viewports = 1;
	pFrameWork.GetContext()->RSGetViewports(&num_viewports, &viewport);
	float screen_width = viewport.Width;
	float screen_height = viewport.Height;

	// Set each sprite's vertices coordinate to screen space
	// left-top
	float x0 = dx;
	float y0 = dy;
	// right-top
	float x1 = dx + dw;
	float y1 = dy;
	// left-bottom
	float x2 = dx;
	float y2 = dy + dh;
	// right-bottom
	float x3 = dx + dw;
	float y3 = dy + dh;

	// Translate sprite's centre to origin (rotate centre)
	float mx = dx + dw * 0.5f;
	float my = dy + dh * 0.5f;
	x0 -= mx;
	y0 -= my;
	x1 -= mx;
	y1 -= my;
	x2 -= mx;
	y2 -= my;
	x3 -= mx;
	y3 -= my;

	// Rotate each sprite's vertices by angle
	float rx, ry;
	float cos = cosf(angle*0.01745f);
	float sin = sinf(angle*0.01745f);
	rx = x0;
	ry = y0;
	x0 = cos * rx + -sin * ry;
	y0 = sin * rx + cos * ry;
	rx = x1;
	ry = y1;
	x1 = cos * rx + -sin * ry;
	y1 = sin * rx + cos * ry;
	rx = x2;
	ry = y2;
	x2 = cos * rx + -sin * ry;
	y2 = sin * rx + cos * ry;
	rx = x3;
	ry = y3;
	x3 = cos * rx + -sin * ry;
	y3 = sin * rx + cos * ry;

	// Translate sprite's centre to original position
	x0 += mx;
	y0 += my;
	x1 += mx;
	y1 += my;
	x2 += mx;
	y2 += my;
	x3 += mx;
	y3 += my;

	// Convert to NDC space
	x0 = 2.0f*x0 / screen_width - 1.0f;
	y0 = 1.0f - 2.0f*y0 / screen_height;
	x1 = 2.0f*x1 / screen_width - 1.0f;
	y1 = 1.0f - 2.0f*y1 / screen_height;
	x2 = 2.0f*x2 / screen_width - 1.0f;
	y2 = 1.0f - 2.0f*y2 / screen_height;
	x3 = 2.0f*x3 / screen_width - 1.0f;
	y3 = 1.0f - 2.0f*y3 / screen_height;

	HRESULT hr = S_OK;
	D3D11_MAP map = D3D11_MAP_WRITE_DISCARD;
	D3D11_MAPPED_SUBRESOURCE mapped_buffer;
	hr = pFrameWork.GetContext()->Map(vertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_buffer);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	vertex_form *vertices = static_cast<vertex_form *>(mapped_buffer.pData);
	vertices[0].position.x = x0;
	vertices[0].position.y = y0;
	vertices[1].position.x = x1;
	vertices[1].position.y = y1;
	vertices[2].position.x = x2;
	vertices[2].position.y = y2;
	vertices[3].position.x = x3;
	vertices[3].position.y = y3;
	vertices[0].position.z = vertices[1].position.z = vertices[2].position.z = vertices[3].position.z = 0.0f;

	DirectX::XMFLOAT4 color(r, g, b, a);
	vertices[0].color = vertices[1].color = vertices[2].color = vertices[3].color = color;

	pFrameWork.GetContext()->Unmap(vertexBuffer.Get(), 0);

	// UNIT.02
	UINT stride = sizeof(vertex_form);
	UINT offset = 0;
	pFrameWork.GetContext()->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);
	pFrameWork.GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	pFrameWork.GetContext()->IASetInputLayout(inputLayout.Get());

	pFrameWork.GetContext()->RSSetState(rasterizerState.Get());

	pFrameWork.GetContext()->VSSetShader(vertexShader.Get(), nullptr, 0);
	pFrameWork.GetContext()->PSSetShader(pixelShader.Get(), nullptr, 0);

	pFrameWork.GetContext()->Draw(4, 0);

}
void SpriteBatch::LineRender(float dx, float dy, float dw, float dh, float r, float g, float b, float a) const
{
	D3D11_VIEWPORT viewport;
	UINT num_viewports = 1;
	pFrameWork.GetContext()->RSGetViewports(&num_viewports, &viewport);
	float screen_width = viewport.Width;
	float screen_height = viewport.Height;

	// Set each sprite's vertices coordinate to screen space
	// left-top
	float x0 = dx;
	float y0 = dy;
	// right-top
	float x1 = dx + dw;
	float y1 = dy + dh;

	// Translate sprite's centre to origin (rotate centre)
	float mx = dx + dw * 0.5f;
	float my = dy + dh * 0.5f;
	x0 -= mx;
	y0 -= my;
	x1 -= mx;
	y1 -= my;

	//// Rotate each sprite's vertices by angle
	//float rx, ry;
	//float cos = cosf(angle*0.01745f);
	//float sin = sinf(angle*0.01745f);
	//rx = x0;
	//ry = y0;
	//x0 = cos * rx + -sin * ry;
	//y0 = sin * rx + cos * ry;
	//rx = x1;
	//ry = y1;
	//x1 = cos * rx + -sin * ry;
	//y1 = sin * rx + cos * ry;

	// Translate sprite's centre to original position
	x0 += mx;
	y0 += my;
	x1 += mx;
	y1 += my;

	// Convert to NDC space
	x0 = 2.0f*x0 / screen_width - 1.0f;
	y0 = 1.0f - 2.0f*y0 / screen_height;
	x1 = 2.0f*x1 / screen_width - 1.0f;
	y1 = 1.0f - 2.0f*y1 / screen_height;

	HRESULT hr = S_OK;
	D3D11_MAP map = D3D11_MAP_WRITE_DISCARD;
	D3D11_MAPPED_SUBRESOURCE mapped_buffer;
	hr = pFrameWork.GetContext()->Map(vertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_buffer);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	vertex_form *vertices = static_cast<vertex_form *>(mapped_buffer.pData);
	vertices[0].position.x = x0;
	vertices[0].position.y = y0;
	vertices[1].position.x = x1;
	vertices[1].position.y = y1;
	vertices[0].position.z = vertices[1].position.z = 0.0f;

	DirectX::XMFLOAT4 color(r, g, b, a);
	vertices[0].color = vertices[1].color = color;

	pFrameWork.GetContext()->Unmap(vertexBuffer.Get(), 0);

	// UNIT.02
	UINT stride = sizeof(vertex_form);
	UINT offset = 0;
	pFrameWork.GetContext()->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);
	pFrameWork.GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	pFrameWork.GetContext()->IASetInputLayout(inputLayout.Get());

	pFrameWork.GetContext()->RSSetState(rasterizerState.Get());

	pFrameWork.GetContext()->VSSetShader(vertexShader.Get(), nullptr, 0);
	pFrameWork.GetContext()->PSSetShader(pixelShader.Get(), nullptr, 0);

	pFrameWork.GetContext()->Draw(2, 0);

}
void SpriteBatch::CircleRenderWire(float dx, float dy, float rad, float r, float g, float b, float a) const
{
	assert(rad <= 1920);
	D3D11_VIEWPORT viewport;
	UINT num_viewports = 1;
	pFrameWork.GetContext()->RSGetViewports(&num_viewports, &viewport);
	float screen_width = viewport.Width;
	float screen_height = viewport.Height;
	int num = (int)rad;
	float x[1920];
	float y[1920];
	float rr = rad * 0.00005f;
	float add_angle = 360.0f * 0.0174f / num;
	for (int i = 0; i < num + 10; i++)
	{
		float ang = (add_angle + rr) * i;
		x[i] = cosf(ang) * rad + dx;
		y[i] = sinf(ang) * rad + dy;
		x[i] = 2.0f*x[i] / screen_width - 1.0f;
		y[i] = 1.0f - 2.0f*y[i] / screen_height;
	}


	HRESULT hr = S_OK;
	D3D11_MAP map = D3D11_MAP_WRITE_DISCARD;
	D3D11_MAPPED_SUBRESOURCE mapped_buffer;
	hr = pFrameWork.GetContext()->Map(vertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_buffer);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	vertex_form *vx = static_cast<vertex_form *>(mapped_buffer.pData);
	for (int i = 0; i < num + 10; i++)
	{
		vx[i].position = DirectX::XMFLOAT3(x[i], y[i], 0.0f);
		vx[i].color = DirectX::XMFLOAT4(r, g, b, a);
	}

	pFrameWork.GetContext()->Unmap(vertexBuffer.Get(), 0);

	UINT stride = sizeof(vertex_form);
	UINT offset = 0;
	pFrameWork.GetContext()->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);
	pFrameWork.GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);
	pFrameWork.GetContext()->IASetInputLayout(inputLayout.Get());

	pFrameWork.GetContext()->RSSetState(rasterizerState.Get());

	pFrameWork.GetContext()->VSSetShader(vertexShader.Get(), nullptr, 0);
	pFrameWork.GetContext()->PSSetShader(pixelShader.Get(), nullptr, 0);
	pFrameWork.GetContext()->Draw(num + 10, 0);
}
void SpriteBatch::CircleRenderSolid(float dx, float dy, float rad, float r, float g, float b, float a) const
{
	assert(rad <= 1920);
	D3D11_VIEWPORT viewport;
	UINT num_viewports = 1;
	pFrameWork.GetContext()->RSGetViewports(&num_viewports, &viewport);
	float screen_width = viewport.Width;
	float screen_height = viewport.Height;
	int num = (int)rad;
	float x[1920];
	float y[1920];
	float rr = rad * 0.00005f;
	float add_angle = 360.0f * 0.0174f / num;
	for (int i = 0; i < num + 10; i++)
	{
		float ang = (add_angle + rr) * i;
		x[i] = cosf(ang) * rad + dx;
		y[i] = sinf(ang) * rad + dy;
		x[i] = 2.0f*x[i] / screen_width - 1.0f;
		y[i] = 1.0f - 2.0f*y[i] / screen_height;
	}

	float ox = dx;
	float oy = dy;

	HRESULT hr = S_OK;
	D3D11_MAP map = D3D11_MAP_WRITE_DISCARD;
	D3D11_MAPPED_SUBRESOURCE mapped_buffer;
	hr = pFrameWork.GetContext()->Map(vertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_buffer);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	vertex_form *vx = static_cast<vertex_form *>(mapped_buffer.pData);
	for (int i = 0; i < 1; i++)
	{
		vx[i].position = DirectX::XMFLOAT3(ox, oy, 0.0f);
		vx[i].color = DirectX::XMFLOAT4(r, g, b, a);
		vx[i+1].position = DirectX::XMFLOAT3(x[i+1], y[i+1], 0.0f);
		vx[i+1].color = DirectX::XMFLOAT4(r, g, b, a);
		vx[i+2].position = DirectX::XMFLOAT3(x[i+2], y[i+2], 0.0f);
		vx[i+2].color = DirectX::XMFLOAT4(r, g, b, a);
	}

	pFrameWork.GetContext()->Unmap(vertexBuffer.Get(), 0);

	UINT stride = sizeof(vertex_form);
	UINT offset = 0;
	pFrameWork.GetContext()->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);
	pFrameWork.GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	pFrameWork.GetContext()->IASetInputLayout(inputLayout.Get());

	pFrameWork.GetContext()->RSSetState(rasterizerState.Get());

	pFrameWork.GetContext()->VSSetShader(vertexShader.Get(), nullptr, 0);
	pFrameWork.GetContext()->PSSetShader(pixelShader.Get(), nullptr, 0);
	pFrameWork.GetContext()->Draw(num + 10, 0);
}
//***********************************************************************************************************************************
//
//		テクスチャー
//
//***********************************************************************************************************************************
SpriteBatch::SpriteBatch(const wchar_t *file_name, size_t max_instance)/*: MAX_INSTANCES(max_instance)*/
{
	HRESULT hr = S_OK;

	vertex vertices[] =
	{
	{ DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT2(0, 0) },
	{ DirectX::XMFLOAT3(1, 0, 0), DirectX::XMFLOAT2(1, 0) },
	{ DirectX::XMFLOAT3(0, 1, 0), DirectX::XMFLOAT2(0, 1) },
	{ DirectX::XMFLOAT3(1, 1, 0), DirectX::XMFLOAT2(1, 1) },
	};

	D3D11_BUFFER_DESC buffer_desc = {};
	buffer_desc.ByteWidth = sizeof(vertices);
	buffer_desc.Usage = D3D11_USAGE_IMMUTABLE;
	buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	buffer_desc.CPUAccessFlags = 0;
	buffer_desc.MiscFlags = 0;
	buffer_desc.StructureByteStride = 0;


	D3D11_SUBRESOURCE_DATA subresource_data = {};
	subresource_data.pSysMem = vertices;
	subresource_data.SysMemPitch = 0;
	subresource_data.SysMemSlicePitch = 0; 
	hr = pFrameWork.GetDevice()->CreateBuffer(&buffer_desc, &subresource_data, vertexBuffer.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	D3D11_INPUT_ELEMENT_DESC input_element_desc[] = //シェーダーとの関連
	{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "NDC_TRANSFORM", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
	{ "NDC_TRANSFORM", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
	{ "NDC_TRANSFORM", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
	{ "NDC_TRANSFORM", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
	{ "TEXCOORD_TRANSFORM", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
	{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
	};

	Descartes::CreateVSFromCso(pFrameWork.GetDevice(), "Shader/SpriteBatchVS.cso", vertexShader.GetAddressOf(), inputLayout.GetAddressOf(), input_element_desc, ARRAYSIZE(input_element_desc));
	Descartes::CreatePSFromCso(pFrameWork.GetDevice(), "Shader/SpriteBatchPS.cso", pixelShader.GetAddressOf());
	Descartes::CreatePSFromCso(pFrameWork.GetDevice(), "Shader/SpriteBatchDissolvePS.cso", pixelDissolveShader.GetAddressOf());

	instance *instances = new instance[MAX_INSTANCES];
	{
		D3D11_BUFFER_DESC buffer_desc = {};
		D3D11_SUBRESOURCE_DATA subresource_data = {};

		buffer_desc.ByteWidth = static_cast<UINT>(sizeof(instance)*MAX_INSTANCES);
		buffer_desc.Usage = D3D11_USAGE_DYNAMIC;
		buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		buffer_desc.MiscFlags = 0;
		buffer_desc.StructureByteStride = 0;
		subresource_data.pSysMem = instances;
		subresource_data.SysMemPitch = 0; 
		subresource_data.SysMemSlicePitch = 0;
		hr = pFrameWork.GetDevice()->CreateBuffer(&buffer_desc, &subresource_data, instanceBuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}
	delete[] instances;

	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.ByteWidth = sizeof(DissolveCBuffer);
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;
	hr = pFrameWork.GetDevice()->CreateBuffer(&bufferDesc, nullptr, dissolveConstantBuffer.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	D3D11_RASTERIZER_DESC rasterizer_desc = {};
	rasterizer_desc.FillMode = D3D11_FILL_SOLID; 
	rasterizer_desc.CullMode = D3D11_CULL_NONE; 
	rasterizer_desc.FrontCounterClockwise = FALSE;
	rasterizer_desc.DepthBias = 0;
	rasterizer_desc.DepthBiasClamp = 0;
	rasterizer_desc.SlopeScaledDepthBias = 0;
	rasterizer_desc.DepthClipEnable = FALSE;
	rasterizer_desc.ScissorEnable = FALSE;
	rasterizer_desc.MultisampleEnable = FALSE;
	rasterizer_desc.AntialiasedLineEnable = FALSE;
	hr = pFrameWork.GetDevice()->CreateRasterizerState(&rasterizer_desc, rasterizerState.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	Texture::LoadTextureFromFile(pFrameWork.GetDevice(), file_name, shaderResourceView.GetAddressOf(), &texture2dDesc);

	D3D11_SAMPLER_DESC sampler_desc;
	sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;// D3D11_FILTER_ANISOTROPIC; //UNIT.06
	sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	sampler_desc.MipLODBias = 0;
	sampler_desc.MaxAnisotropy = 16;
	sampler_desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	//memcpy(sampler_desc.BorderColor, & DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f), sizeof(DirectX::XMFLOAT4));
	sampler_desc.MinLOD = 0;
	sampler_desc.MaxLOD = D3D11_FLOAT32_MAX;
	hr = pFrameWork.GetDevice()->CreateSamplerState(&sampler_desc, samplerState.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	D3D11_DEPTH_STENCIL_DESC depth_stencil_desc;
	depth_stencil_desc.DepthEnable = FALSE;
	depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	depth_stencil_desc.DepthFunc = D3D11_COMPARISON_ALWAYS;
	depth_stencil_desc.StencilEnable = FALSE;
	depth_stencil_desc.StencilReadMask = 0xFF;
	depth_stencil_desc.StencilWriteMask = 0xFF;
	depth_stencil_desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depth_stencil_desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depth_stencil_desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depth_stencil_desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depth_stencil_desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depth_stencil_desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depth_stencil_desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depth_stencil_desc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	hr = pFrameWork.GetDevice()->CreateDepthStencilState(&depth_stencil_desc, depthStencilState.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));


}

void SpriteBatch::BeginDissolve()
{
	HRESULT hr = S_OK;

	UINT strides[2] = { sizeof(vertex), sizeof(instance) };
	UINT offsets[2] = { 0, 0 };
	ID3D11Buffer* vbs[2] = { vertexBuffer.Get(), instanceBuffer.Get() };
	pFrameWork.GetContext()->IASetVertexBuffers(0, 2, vbs, strides, offsets);
	pFrameWork.GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	pFrameWork.GetContext()->IASetInputLayout(inputLayout.Get());
	pFrameWork.GetContext()->OMSetDepthStencilState(depthStencilState.Get(), 1);
	pFrameWork.GetContext()->RSSetState(rasterizerState.Get());
	pFrameWork.GetContext()->VSSetShader(vertexShader.Get(), nullptr, 0);
	pFrameWork.GetContext()->PSSetShader(pixelDissolveShader.Get(), nullptr, 0);

	pFrameWork.GetContext()->PSSetShaderResources(0, 1, shaderResourceView.GetAddressOf());
	pFrameWork.GetContext()->PSSetShaderResources(1, 1, dissolveSRV.GetAddressOf());
	pFrameWork.GetContext()->PSSetShaderResources(2, 1, emissiveSRV.GetAddressOf());

	pFrameWork.GetContext()->PSSetSamplers(0, 1, samplerState.GetAddressOf());

	UINT num_viewports = 1;
	pFrameWork.GetContext()->RSGetViewports(&num_viewports, &viewport);

	D3D11_MAP map = D3D11_MAP_WRITE_DISCARD;
	D3D11_MAPPED_SUBRESOURCE mapped_buffer;
	hr = pFrameWork.GetContext()->Map(instanceBuffer.Get(), 0, map, 0, &mapped_buffer);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	instances = static_cast<instance*>(mapped_buffer.pData);

	countInstance = 0;
}

void SpriteBatch::Begin()
{
	HRESULT hr = S_OK;

	UINT strides[2] = { sizeof(vertex), sizeof(instance) };
	UINT offsets[2] = { 0, 0 };
	ID3D11Buffer *vbs[2] = { vertexBuffer.Get(), instanceBuffer.Get() };
	pFrameWork.GetContext()->IASetVertexBuffers(0, 2, vbs, strides, offsets);
	pFrameWork.GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	pFrameWork.GetContext()->IASetInputLayout(inputLayout.Get());
	pFrameWork.GetContext()->OMSetDepthStencilState(depthStencilState.Get(), 1);
	pFrameWork.GetContext()->RSSetState(rasterizerState.Get());
	pFrameWork.GetContext()->VSSetShader(vertexShader.Get(), nullptr, 0);
	pFrameWork.GetContext()->PSSetShader(pixelShader.Get(), nullptr, 0);
	pFrameWork.GetContext()->PSSetShaderResources(0, 1, shaderResourceView.GetAddressOf());
	pFrameWork.GetContext()->PSSetSamplers(0, 1, samplerState.GetAddressOf());

	UINT num_viewports = 1;
	pFrameWork.GetContext()->RSGetViewports(&num_viewports, &viewport);

	D3D11_MAP map = D3D11_MAP_WRITE_DISCARD;
	D3D11_MAPPED_SUBRESOURCE mapped_buffer;
	hr = pFrameWork.GetContext()->Map(instanceBuffer.Get(), 0, map, 0, &mapped_buffer);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	instances = static_cast<instance *>(mapped_buffer.pData);

	countInstance = 0;
}

void SpriteBatch::Render(float dx, float dy, float dw, float dh, float angle/*degree*/, float r, float g, float b, float a, SpriteInf inf)
{
	_ASSERT_EXPR(countInstance < MAX_INSTANCES, L"Number of instances must be less than MAX_INSTANCES.");

	float cx = dw * 0.5f, cy = dh * 0.5f; /*Center of Rotation*/
#if 0
#else//座標変換
	FLOAT c = cosf(angle*0.01745f);
	FLOAT s = sinf(angle*0.01745f);
	FLOAT w = 2.0f / viewport.Width;
	FLOAT h = -2.0f / viewport.Height;
	instances[countInstance].ndcTransform._11 = w * dw*c;
	instances[countInstance].ndcTransform._21 = h * dw*s;
	instances[countInstance].ndcTransform._31 = 0.0f;
	instances[countInstance].ndcTransform._41 = 0.0f;
	instances[countInstance].ndcTransform._12 = w * dh*-s;
	instances[countInstance].ndcTransform._22 = h * dh*c;
	instances[countInstance].ndcTransform._32 = 0.0f;
	instances[countInstance].ndcTransform._42 = 0.0f;
	instances[countInstance].ndcTransform._13 = 0.0f;
	instances[countInstance].ndcTransform._23 = 0.0f;
	instances[countInstance].ndcTransform._33 = 1.0f;
	instances[countInstance].ndcTransform._43 = 0.0f;
	instances[countInstance].ndcTransform._14 = w * (-cx * c + -cy * -s + cx + dx) - 1.0f;
	instances[countInstance].ndcTransform._24 = h * (-cx * s + -cy * c + cy + dy) + 1.0f;
	instances[countInstance].ndcTransform._34 = 0.0f;
	instances[countInstance].ndcTransform._44 = 1.0f;
#endif
	//texcood
	float tw = static_cast<float>(texture2dDesc.Width);
	float th = static_cast<float>(texture2dDesc.Height);
	instances[countInstance].texcoordTransform = DirectX::XMFLOAT4(inf.sx / tw, inf.sy / th, inf.sw / tw, inf.sh / th);
	instances[countInstance].color = DirectX::XMFLOAT4(r, g, b, a);

	countInstance++;

}

void SpriteBatch::RenderDissolve(float dx, float dy, float dw, float dh, float angle/*degree*/, float r, float g, float b, float a, SpriteInf inf, float dissolve, float emissiveRange)
{
	_ASSERT_EXPR(countInstance < MAX_INSTANCES, L"Number of instances must be less than MAX_INSTANCES.");

	float cx = dw * 0.5f, cy = dh * 0.5f; /*Center of Rotation*/
#if 0
#else//座標変換
	FLOAT c = cosf(angle*0.01745f);
	FLOAT s = sinf(angle*0.01745f);
	FLOAT w = 2.0f / viewport.Width;
	FLOAT h = -2.0f / viewport.Height;
	instances[countInstance].ndcTransform._11 = w * dw*c;
	instances[countInstance].ndcTransform._21 = h * dw*s;
	instances[countInstance].ndcTransform._31 = 0.0f;
	instances[countInstance].ndcTransform._41 = 0.0f;
	instances[countInstance].ndcTransform._12 = w * dh*-s;
	instances[countInstance].ndcTransform._22 = h * dh*c;
	instances[countInstance].ndcTransform._32 = 0.0f;
	instances[countInstance].ndcTransform._42 = 0.0f;
	instances[countInstance].ndcTransform._13 = 0.0f;
	instances[countInstance].ndcTransform._23 = 0.0f;
	instances[countInstance].ndcTransform._33 = 1.0f;
	instances[countInstance].ndcTransform._43 = 0.0f;
	instances[countInstance].ndcTransform._14 = w * (-cx * c + -cy * -s + cx + dx) - 1.0f;
	instances[countInstance].ndcTransform._24 = h * (-cx * s + -cy * c + cy + dy) + 1.0f;
	instances[countInstance].ndcTransform._34 = 0.0f;
	instances[countInstance].ndcTransform._44 = 1.0f;
#endif
	//texcood
	float tw = static_cast<float>(texture2dDesc.Width);
	float th = static_cast<float>(texture2dDesc.Height);
	instances[countInstance].texcoordTransform = DirectX::XMFLOAT4(inf.sx / tw, inf.sy / th, inf.sw / tw, inf.sh / th);
	instances[countInstance].color = DirectX::XMFLOAT4(r, g, b, a);

	DissolveCBuffer dcb;
	dcb.dissolveValue = dissolve;
	dcb.emissiveRange = emissiveRange;
	dcb.emissiveColor = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	dcb.dummy01 = 0.0f;
	dcb.dummy02 = 0.0f;

	pFrameWork.GetContext()->UpdateSubresource(dissolveConstantBuffer.Get(), 0, nullptr, &dcb, 0, 0);
	pFrameWork.GetContext()->VSSetConstantBuffers(0, 1, dissolveConstantBuffer.GetAddressOf());
	pFrameWork.GetContext()->PSSetConstantBuffers(0, 1, dissolveConstantBuffer.GetAddressOf());

	countInstance++;

}

void SpriteBatch::End()
{
	pFrameWork.GetContext()->Unmap(instanceBuffer.Get(), 0);

	pFrameWork.GetContext()->DrawInstanced(4, countInstance, 0, 0);
	countInstance = 0;
}

void SpriteBatch::DrawRota(float x, float y, float angle, float scale, SpriteInf inf , DirectX::XMFLOAT4 color)
{
	Render(x - (inf.sw / 2), y - (inf.sh / 2), inf.sw * scale, inf.sh * scale, angle, color.x, color.y, color.z, color.w, inf);
}
void SpriteBatch::DrawDissolve(float x, float y, float angle, float scale, SpriteInf inf , float dissolve, float range, DirectX::XMFLOAT4 color)
{
	RenderDissolve(x - (inf.sw / 2.0f * scale), y - (inf.sh / 2.0f * scale), inf.sw * scale, inf.sh * scale, angle, color.x, color.y, color.z, color.w, inf, dissolve, range);
}
void SpriteBatch::DrawRota2(float x, float y, float angle, float scale, SpriteInf inf , DirectX::XMFLOAT4 color)
{
	Render(x - (inf.sw / 2.0f * scale), y - (inf.sh / 2.0f * scale), inf.sw * scale, inf.sh * scale, angle, color.x, color.y, color.z, color.w, inf);
}
void SpriteBatch::DrawRota3(float x, float y, float angle, float scalex, float scaley, SpriteInf inf , DirectX::XMFLOAT4 color)
{
	Render(x - (inf.sw / 2.0f * scalex), y - (inf.sh / 2.0f * scaley), inf.sw * scalex, inf.sh * scaley, angle, color.x, color.y, color.z, color.w, inf);
}


#pragma once

#include <windows.h>
#include <d3d11.h>
#include <wrl.h>
#include <map>
#include <memory>
#include <cassert>


class Texture
{
public:
	static void LoadTextureFromFile
	(
		ID3D11Device *device,
		const wchar_t *fileName,
		ID3D11ShaderResourceView **shaderResourceView,
		D3D11_TEXTURE2D_DESC *texture2dDesc
	);

	static void LoadTextureFromFile
	(
		ID3D11Device* device,
		const char* fileName,
		ID3D11ShaderResourceView** shaderResourceView
	);

	static void CreateTexture
	(
		ID3D11Device* device,
		u_int width,
		u_int height,
		DXGI_FORMAT format,
		ID3D11ShaderResourceView** shaderResourceView,
		ID3D11Texture2D** texture2d
	);

	// TODO : ダミーテクスチャ作成研究中！
	static void CreateDummyTexture
	(
		ID3D11Device* device,
		const wchar_t* fileName,
		ID3D11ShaderResourceView** shaderResourceView
	);
};
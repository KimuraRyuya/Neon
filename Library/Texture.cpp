#include "Texture.h"
#include "WICTextureLoader.h"
#include "../DirectXTex-master/DirectXTex/DirectXTex.h"
#include <string>

void Texture::LoadTextureFromFile
  ( ID3D11Device *device,
	const wchar_t *fileName,
	ID3D11ShaderResourceView **shaderResourceView,
	D3D11_TEXTURE2D_DESC *texture2dDesc )
{

	HRESULT hr = S_OK;


	Microsoft::WRL::ComPtr<ID3D11Resource> resource;
	
	static std::map<std::wstring, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> cache;

	auto it = cache.find( fileName );
	if (it != cache.end())
	{
		*shaderResourceView = it->second.Get();
		( *shaderResourceView )->AddRef();
		( *shaderResourceView )->GetResource( resource.GetAddressOf() );
	}
	else
	{
		hr = DirectX::CreateWICTextureFromFile( device, fileName, resource.GetAddressOf(), shaderResourceView );
		assert( !hr && "DirectX::CreateWICTextureFromFile	Error" );

		cache.insert( std::make_pair( fileName, *shaderResourceView ) );
	}


	Microsoft::WRL::ComPtr<ID3D11Texture2D> texture2d;

	hr = resource.Get()->QueryInterface<ID3D11Texture2D>( texture2d.GetAddressOf() );
	assert( !hr && "QueryInterface	Error" );


	texture2d->GetDesc( texture2dDesc );

}

void Texture::LoadTextureFromFile
 (  ID3D11Device* device,
	const char* fileName,
	ID3D11ShaderResourceView** shaderResourceView )
{

	HRESULT hr = S_OK;


	Microsoft::WRL::ComPtr<ID3D11Resource> resource;

	static std::map<std::string, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> cache;

	auto it = cache.find( fileName );
	if ( it != cache.end() )
	{
		*shaderResourceView = it->second.Get();
		( *shaderResourceView )->AddRef();
		( *shaderResourceView )->GetResource( resource.GetAddressOf() );
	}
	else
	{
		/*hr = DirectX::CreateWICTextureFromFile(device, fileName, resource.GetAddressOf(), shaderResourceView);
		assert( !hr && "DirectX::CreateWICTextureFromFile	Error" );*/

		char ext[32];
		_splitpath_s( fileName, NULL, NULL, NULL, NULL, NULL, NULL, ext, sizeof( ext ) );

		wchar_t _fileName[256];

		MultiByteToWideChar( CP_ACP, 0, fileName, static_cast<int>( strlen( fileName ) + 1 ), _fileName, 256 );

		DirectX::TexMetadata metaData;
		DirectX::ScratchImage image;
		if ( strcmp( ext, ".tga" ) == 0 )
		{
			hr = DirectX::LoadFromTGAFile( _fileName, &metaData, image );
			assert( !hr && "DirectX::LoadFromTGAFile	Error" );
		}
		else if ( strcmp( ext, ".png" ) == 0 )
		{
			hr = DirectX::LoadFromWICFile( _fileName, 0, &metaData, image );
			assert( !hr && "DirectX::LoadFromWICFile	Error" );
		}

		hr = DirectX::CreateShaderResourceViewEx(
			device,
			image.GetImages(),
			image.GetImageCount(),
			metaData,
			D3D11_USAGE_DEFAULT,
			D3D11_BIND_SHADER_RESOURCE,
			0,
			D3D11_RESOURCE_MISC_TEXTURECUBE,
			true,
			shaderResourceView );
		assert( !hr && "DirectX::CreateShaderResourceViewEx" );

		cache.insert( std::make_pair( fileName, *shaderResourceView ) );
	}

}

void Texture::CreateTexture
 (  ID3D11Device* device,
	 u_int width,
	 u_int height,
	 DXGI_FORMAT format,
	ID3D11ShaderResourceView** shaderResourceView,
	 ID3D11Texture2D** texture2d )
{
	HRESULT hr = S_OK;

	//	テクスチャ作成
	D3D11_TEXTURE2D_DESC texture2d_desc;
	texture2d_desc.Width = width;
	texture2d_desc.Height = height;
	texture2d_desc.MipLevels = 1;
	texture2d_desc.ArraySize = 1;
	texture2d_desc.Format = format;
	texture2d_desc.SampleDesc.Count = 1;
	texture2d_desc.Usage = D3D11_USAGE_DEFAULT;
	texture2d_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	texture2d_desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

	hr = device->CreateTexture2D(&texture2d_desc, NULL, texture2d);
	assert( !hr );
}

void Texture::CreateDummyTexture
 (  ID3D11Device* device,
	const wchar_t* fileName,
	ID3D11ShaderResourceView** shaderResourceView )
{

	HRESULT hr = S_OK;


	Microsoft::WRL::ComPtr<ID3D11Resource> resource;

	static std::map<std::wstring, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> cache;

	auto it = cache.find( fileName );
	if ( it != cache.end() )
	{
		*shaderResourceView = it->second.Get();
		( *shaderResourceView )->AddRef();
		( *shaderResourceView )->GetResource( resource.GetAddressOf() );
	}
	else
	{
		D3D11_TEXTURE2D_DESC TextureDesc = {};
		TextureDesc.Width = 1;
		TextureDesc.Height = 1;
		TextureDesc.MipLevels = 1;
		TextureDesc.ArraySize = 1;
		TextureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		TextureDesc.SampleDesc.Count = 1;
		TextureDesc.SampleDesc.Quality = 0;
		TextureDesc.Usage = D3D11_USAGE_DEFAULT;
		TextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		TextureDesc.CPUAccessFlags = 0;
		TextureDesc.MiscFlags = 0;

		ID3D11Texture2D* Texture = 0;

		unsigned char materialColor[] =
		{
			UCHAR_MAX,
			UCHAR_MAX,
			UCHAR_MAX
		};

		D3D11_SUBRESOURCE_DATA subresourceData = {};
		subresourceData.pSysMem = materialColor;
		subresourceData.SysMemPitch = 1;
		subresourceData.SysMemSlicePitch = 0;

		hr = device->CreateTexture2D( &TextureDesc, &subresourceData, &Texture );
		assert( !hr );

		// ShaderResourceViewの情報を作成する
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = TextureDesc.Format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = TextureDesc.MipLevels;

		hr = device->CreateShaderResourceView( Texture, &srvDesc, shaderResourceView );
		assert( !hr );

		cache.insert(std::make_pair( fileName, *shaderResourceView ) );

		Texture->Release();
	}


	/*Microsoft::WRL::ComPtr<ID3D11Texture2D> texture2d;

	hr = resource.Get()->QueryInterface<ID3D11Texture2D>(texture2d.GetAddressOf());
	assert(!hr && "QueryInterface	Error");*/

}
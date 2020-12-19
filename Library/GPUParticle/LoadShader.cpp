#undef max
#undef min

#include "LoadShader.h"

using namespace std;

string SHADERPATH = "Shader/";

namespace LoadShader
{
	bool LoadVertexShader(const std::string& filename, ID3D11VertexShader** shader, D3D11_INPUT_ELEMENT_DESC* inputDesc, UINT inputDescSize, ID3D11InputLayout** inputLayout)
	{
		string filepath = SHADERPATH + filename;

		// ファイルを開く
		FILE* fp = nullptr;
		fopen_s(&fp, filepath.c_str(), "rb");
		_ASSERT_EXPR_A(fp, "CSO File not found");

		// ファイルのサイズを求める
		fseek(fp, 0, SEEK_END);
		long cso_sz = ftell(fp);
		fseek(fp, 0, SEEK_SET);

		// メモリ上に頂点シェーダーデータを格納する領域を用意する
		std::unique_ptr<u_char[]> cso_data = std::make_unique<u_char[]>(cso_sz);
		fread(cso_data.get(), cso_sz, 1, fp);
		fclose(fp);

		// 頂点シェーダー生成
		HRESULT hr = pFrameWork.GetDevice()->CreateVertexShader(cso_data.get(), cso_sz, nullptr, shader);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		
		hr = pFrameWork.GetDevice()->CreateInputLayout(inputDesc, inputDescSize, cso_data.get(), cso_sz, inputLayout);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		return true;
	}

	bool LoadComputeShader(const std::string& filename, ID3D11ComputeShader** shader)
	{
		string filepath = SHADERPATH + filename;

		// ファイルを開く
		FILE* fp = nullptr;
		fopen_s(&fp, filepath.c_str(), "rb");
		_ASSERT_EXPR_A(fp, "CSO File not found");

		// ファイルのサイズを求める
		fseek(fp, 0, SEEK_END);
		long cso_sz = ftell(fp);
		fseek(fp, 0, SEEK_SET);

		// メモリ上に頂点シェーダーデータを格納する領域を用意する
		std::unique_ptr<u_char[]> cso_data = std::make_unique<u_char[]>(cso_sz);
		fread(cso_data.get(), cso_sz, 1, fp);
		fclose(fp);

		// 頂点シェーダー生成
		HRESULT hr = pFrameWork.GetDevice()->CreateComputeShader(cso_data.get(), cso_sz, nullptr, shader);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		return true;
	}

	bool LoadPixelShader(const std::string& filename, ID3D11PixelShader** shader)
	{
		string filepath = SHADERPATH + filename;

		// ファイルを開く
		FILE* fp = nullptr;
		fopen_s(&fp, filepath.c_str(), "rb");
		_ASSERT_EXPR_A(fp, "CSO File not found");

		// ファイルのサイズを求める
		fseek(fp, 0, SEEK_END);
		long cso_sz = ftell(fp);
		fseek(fp, 0, SEEK_SET);

		// メモリ上に頂点シェーダーデータを格納する領域を用意する
		std::unique_ptr<u_char[]> cso_data = std::make_unique<u_char[]>(cso_sz);
		fread(cso_data.get(), cso_sz, 1, fp);
		fclose(fp);

		// ピクセルシェーダー生成
		HRESULT hr = pFrameWork.GetDevice()->CreatePixelShader(cso_data.get(), cso_sz, nullptr, shader);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		return true;
	}

}
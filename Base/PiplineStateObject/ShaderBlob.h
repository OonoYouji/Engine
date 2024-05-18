#pragma once 

#include <wrl/client.h>
#include <dxcapi.h>

#include <string>

using namespace Microsoft::WRL;

class ShaderCompile;


class ShaderBlob final {
public:

	ShaderBlob();
	~ShaderBlob();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(
		ShaderCompile* shaderCompile,
		const std::wstring& vsFilePath,
		const wchar_t* vsProfile,
		const std::wstring& psFilePath,
		const wchar_t* psProfile
	);

	IDxcBlob* GetVS() const { return vertexShader_.Get(); }
	IDxcBlob* GetPS() const { return pixelShader_.Get(); }

private:

	ComPtr<IDxcBlob> vertexShader_;
	ComPtr<IDxcBlob> pixelShader_;

};
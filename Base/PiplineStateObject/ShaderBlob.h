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

	/// <summary>
	/// CSの初期化
	/// </summary>
	void Initialize(
		ShaderCompile* shaderCompile,
		const std::wstring& csFilePath,
		const std::wstring& csEntryPoint,
		const std::wstring& csProfile
	);

	IDxcBlob* GetVS() const { return vertexShader_.Get(); }
	IDxcBlob* GetPS() const { return pixelShader_.Get(); }
	IDxcBlob* GetCS() const { return computeShader_.Get(); }

private:

	ComPtr<IDxcBlob> vertexShader_;
	ComPtr<IDxcBlob> pixelShader_;
	ComPtr<IDxcBlob> computeShader_;

};
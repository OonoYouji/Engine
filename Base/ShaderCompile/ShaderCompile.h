#pragma once

#include <wrl/client.h>
#include <dxcapi.h>

#include <string>

using namespace Microsoft::WRL;

/// <summary>
/// ShaderCompiler
/// </summary>
class ShaderCompile final {
public:

	ShaderCompile();
	~ShaderCompile();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// Shaderをコンパイルする
	/// </summary>
	/// <param name="filePath: ">Shaderへのファイルパス</param>
	/// <param name="profile: ">Shaderのバージョン</param>
	/// <returns>コンパイルしたShaderBlob</returns>
	ComPtr<IDxcBlob> CompileShader(const std::wstring& filePath, const wchar_t* profile);

private:

	ComPtr<IDxcUtils> dxcUtils_;
	ComPtr<IDxcCompiler3> dxcCompiler_;
	ComPtr<IDxcIncludeHandler> includeHandler_;

};
#include <ShaderBlob.h>

#include <ShaderCompile.h>

ShaderBlob::ShaderBlob() {}
ShaderBlob::~ShaderBlob() {}


void ShaderBlob::Initialize(ShaderCompile* shaderCompile, const std::wstring& vsFilePath, const wchar_t* vsProfile, const std::wstring& psFilePath, const wchar_t* psProfile) {

	vertexShader_ = shaderCompile->CompileShader(vsFilePath, vsProfile);
	pixelShader_ = shaderCompile->CompileShader(psFilePath, psProfile);

}



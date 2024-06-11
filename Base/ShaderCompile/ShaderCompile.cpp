#include <ShaderCompile.h>

#include <iostream>
#include <cassert>
#include <format>

#include <Engine.h>


ShaderCompile::ShaderCompile() {}
ShaderCompile::~ShaderCompile() {}


void ShaderCompile::Initialize() {
	HRESULT result = S_FALSE;

	result = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils_));
	assert(SUCCEEDED(result));
	result = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxcCompiler_));
	assert(SUCCEEDED(result));

	///- includeに対応するために設定を行う
	result = dxcUtils_->CreateDefaultIncludeHandler(&includeHandler_);
	assert(SUCCEEDED(result));

}

ComPtr<IDxcBlob> ShaderCompile::CompileShader(const std::wstring& filePath, const wchar_t* profile) {
	HRESULT result = S_FALSE;

	///- これからShaderをCompileするログを出力
	Engine::ConsolePrint(std::format(L"Begin CompileShader, path:{}, profile;{}\n", filePath, profile));

	///- hlslを読み込む
	ComPtr<IDxcBlobEncoding> shaderSource = nullptr;
	result = dxcUtils_->LoadFile(filePath.c_str(), nullptr, &shaderSource);
	assert(SUCCEEDED(result));

	///- ファイルの内容を設定する
	DxcBuffer shaderSourceBuffer;
	shaderSourceBuffer.Ptr = shaderSource->GetBufferPointer();
	shaderSourceBuffer.Size = shaderSource->GetBufferSize();
	shaderSourceBuffer.Encoding = DXC_CP_UTF8; //- 文字コード

	///- Compileの設定
	LPCWSTR arguments[] = {
		filePath.c_str(),			//- Compile対象のhlslファイル名
		L"-E", L"main",				//- エントリーポイントの指定; 基本的にmain以外にはしない
		L"-T", profile,				//- ShaderProfileの設定
		L"-Zi", L"-Qembed_debug",	//- デバッグ用の情報を埋め込む
		L"-Od",						//- 最適化を外す
		L"-Zpr",					//- メモリレイアウトは行優先
	};

	///- 実際にCompileする
	ComPtr<IDxcResult> shaderResult = nullptr;
	result = dxcCompiler_->Compile(
		&shaderSourceBuffer,		//- 読み込んだファイル
		arguments,					//- コンパイルオプション
		_countof(arguments),		//- コンパイルオプションの数
		includeHandler_.Get(),		//- includeが含まれた諸々
		IID_PPV_ARGS(&shaderResult)	//- コンパイル結果
	);
	assert(SUCCEEDED(result));

	///- 警告・エラーが出たらログに出力して止める
	ComPtr<IDxcBlobUtf8> shaderError = nullptr;
	shaderResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&shaderError), nullptr);
	if(shaderError != nullptr && shaderError->GetStringLength() != 0) {
		Engine::ConsolePrint(shaderError->GetStringPointer());
		assert(false);
	}

	///- Compile結果を受け取りreturnする
	ComPtr<IDxcBlob> shaderBlob = nullptr;
	result = shaderResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shaderBlob), nullptr);
	assert(SUCCEEDED(result));
	//- 成功したログ出力
	Engine::ConsolePrint(std::format(L"Compile Succeended, path:{}, profile:{}\n", filePath, profile));

	///- 不要になったリソースの解放
	shaderSource.Reset();
	shaderResult.Reset();

	return shaderBlob;
}



ComPtr<IDxcBlob> ShaderCompile::CompileShader(const std::wstring& filePath, const std::wstring& entryPoint, const std::wstring& profile) {
	// DXCコンパイラとライブラリの初期化
	Microsoft::WRL::ComPtr<IDxcCompiler> compiler;
	Microsoft::WRL::ComPtr<IDxcLibrary> library;
	Microsoft::WRL::ComPtr<IDxcBlobEncoding> sourceBlob;

	HRESULT hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&compiler));
	if(FAILED(hr)) {
		Engine::ConsolePrint("Failed to create DXC compiler instance");
	}

	hr = DxcCreateInstance(CLSID_DxcLibrary, IID_PPV_ARGS(&library));
	if(FAILED(hr)) {
		Engine::ConsolePrint("Failed to create DXC library instance");
	}

	// HLSLファイルの読み込み
	hr = library->CreateBlobFromFile(filePath.c_str(), nullptr, &sourceBlob);
	if(FAILED(hr)) {
		Engine::ConsolePrint("Failed to load HLSL file");
	}

	// コンパイル
	Microsoft::WRL::ComPtr<IDxcOperationResult> result;
	hr = compiler->Compile(
		sourceBlob.Get(),	//- HLSLソースコード
		filePath.c_str(),	//- ファイルパス
		entryPoint.c_str(),	//- エントリーポイント関数名
		profile.c_str(),	//- ターゲットプロファイル
		nullptr, 0,			//- コンパイルオプション
		nullptr, 0,			//- 定数定義
		nullptr,			//- インクルードハンドラー
		&result);			//- コンパイル結果

	if(FAILED(hr)) {
		throw std::runtime_error("Failed to compile HLSL file");
	}

	// コンパイルエラーチェック
	HRESULT hrStatus;
	result->GetStatus(&hrStatus);
	if(FAILED(hrStatus)) {
		Microsoft::WRL::ComPtr<IDxcBlobEncoding> errors;
		hr = result->GetErrorBuffer(&errors);
		if(FAILED(hr)) {
			throw std::runtime_error("Failed to get error buffer");
		}

		std::wstring errorMessage(
			(wchar_t*)errors->GetBufferPointer(),
			errors->GetBufferSize() / sizeof(wchar_t));
		std::wcerr << L"Shader Compilation Failed: " << errorMessage << std::endl;

		throw std::runtime_error("Shader compilation failed");
	}

	Microsoft::WRL::ComPtr<IDxcBlob> shaderBlob;
	hr = result->GetResult(&shaderBlob);
	if(FAILED(hr)) {
		throw std::runtime_error("Failed to get shader blob");
	}

	return shaderBlob;
}

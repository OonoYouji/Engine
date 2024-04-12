#include <DirectXCommon.h>

#include <format>

#include <WinApp.h>
#include <Engine.h>
#include <Environment.h>
#include <Vector4.h>
#include "ImGuiManager.h"


#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dxcompiler.lib")

using namespace Microsoft::WRL;




/// ---------------------------
/// ↓ 初期化を行う
/// ---------------------------
void DirectXCommon::Initialize(WinApp* winApp) {

	p_winApp_ = winApp;

#ifdef _DEBUG

	///- DebugLayerを設定
	if(SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController_)))) {
		debugController_->EnableDebugLayer();
		debugController_->SetEnableGPUBasedValidation(TRUE);
	}

#endif // _DEBUG

	InitializeDXGIDevice();
	InitializeCommand();
	InitializeSwapChain();
	InitialiezRenderTarget();
	InitializeFence();


	InitializeDxcCompiler();
	InitializeRootSignature();
	InitializeInputLayout();
	InitializeBlendState();
	InitializeRasterizer();
	InitializeShaderBlob();
	InitializePSO();
	InitializeVertexResource();
	InitializeMaterialResource();
	InitializeWVPResource();
	InitializeViewport();

	InitializeTextureResource();


	worldTransform_.Init();
	camera_ = std::make_unique<Camera>();

}



/// ---------------------------
/// ↓ メンバ変数の解放などを行う
/// ---------------------------
void DirectXCommon::Finalize() {

	/// ---------------------------
	/// ↓ 生成した逆順に解放していく
	/// ---------------------------

	textureResource_.Reset();
	srvHeap_.Reset();

	wvpResource_.Reset();
	materialResource_.Reset();
	vertexResource_.Reset();
	graphicsPipelineState_.Reset();
	pixelShaderBlob_.Reset();
	vertexShaderBlob_.Reset();
	rootSignature_.Reset();
	errorBlob_.Reset();
	signatureBlob_.Reset();
	includeHandler_.Reset();
	dxcCompiler_.Reset();
	dxcUtils_.Reset();

	CloseHandle(fenceEvent_);
	fence_.Reset();
	rtvDescriptorHeap_.Reset();
	for(UINT i = 0; i < 2; i++) {
		swapChainResource_[i].Reset();
	}
	swapChain_.Reset();
	commandList_.Reset();
	commandAllocator_.Reset();
	commandQueue_.Reset();
	device_.Reset();
	useAdapter_.Reset();
	dxgiFactory_.Reset();
#ifdef _DEBUG
	debugController_.Reset();
#endif // _DEBUG



	/// ---------------------------
	/// ↓ 解放忘れがあれば停止
	/// ---------------------------
	ComPtr<IDXGIDebug1> debug;
	if(SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug)))) {
		debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
		debug->ReportLiveObjects(DXGI_DEBUG_APP, DXGI_DEBUG_RLO_ALL);
		debug->ReportLiveObjects(DXGI_DEBUG_D3D12, DXGI_DEBUG_RLO_ALL);
	}

}




/// ---------------------------
/// ↓ インスタンスを生成; 返す
/// ---------------------------
DirectXCommon* DirectXCommon::GetInstance() {
	static DirectXCommon instance;
	return &instance;
}


/// ---------------------------
/// ↓ DirextX12の初期化
/// ---------------------------
void DirectXCommon::InitializeDXGIDevice() {
	HRESULT result = S_FALSE;

	/// ---------------------------
	/// ↓ DXGIFactoryを生成; 失敗したらassertで止める
	/// ---------------------------
	dxgiFactory_ = nullptr;
	result = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory_));
	assert(SUCCEEDED(result));



	/// ---------------------------
	/// ↓ Adapterを生成; 
	/// ---------------------------
	useAdapter_ = nullptr;

	///- 良い順にアダプタを頼む
	for(UINT i = 0; dxgiFactory_->EnumAdapterByGpuPreference(i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&useAdapter_)) != DXGI_ERROR_NOT_FOUND; i++) {

		///- アダプターの情報を取得する
		DXGI_ADAPTER_DESC3 desc;

		result = useAdapter_->GetDesc3(&desc);
		assert(SUCCEEDED(result));

		///- ソフトウェアアダプタでなければ採用
		if(!(desc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE)) {
			///- 採用したアダプタ情報をログに出力;
			Engine::ConsolePrint(std::format(L"Use Adapter:{}\n", desc.Description));
			break;
		}

		///- ソフトフェアアダプタのときは無視
		useAdapter_ = nullptr;
	}

	///- for()が終了してもadapterが取得できなかった場合起動できない
	assert(useAdapter_ != nullptr);




	/// ---------------------------
	/// ↓ Deviceを生成; 
	/// ---------------------------
	device_ = nullptr;

	///- 機能レベルを高い順に用意
	const char* featureLevelStaring[] = { "12.2", "12.1","12.0" };
	D3D_FEATURE_LEVEL featureLevels[] = {
		D3D_FEATURE_LEVEL_12_2,
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0
	};

	///- 高い順に生成できるか試す
	for(size_t i = 0; i < _countof(featureLevels); i++) {
		// 生成
		result = D3D12CreateDevice(useAdapter_.Get(), featureLevels[i], IID_PPV_ARGS(&device_));
		// 指定した機能れhベルで生成できたか確認
		if(SUCCEEDED(result)) {
			// 生成後にログ出力
			Engine::ConsolePrint(std::format("FeatureLevel : {}\n", featureLevelStaring[i]));
			break;
		}
	}

	///- 生成できたか確認; 生成出来ていたらログ出力する
	assert(device_ != nullptr);
	Engine::ConsolePrint("Compile create D3D12Device!!!\n");



#ifdef _DEBUG
	/// ---------------------------
	/// ↓ エラー・警告が出たときにプログラムを停止
	/// ---------------------------
	ComPtr<ID3D12InfoQueue> infoQueue = nullptr;
	if(SUCCEEDED(device_->QueryInterface(IID_PPV_ARGS(&infoQueue)))) {
		//- ヤバいエラー
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
		//- エラー
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
		//- 警告
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);

		///- エラーと警告の抑制
		D3D12_MESSAGE_ID denyIds[] = {
			D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE
		};
		D3D12_MESSAGE_SEVERITY severities[] = { D3D12_MESSAGE_SEVERITY_INFO };
		D3D12_INFO_QUEUE_FILTER filter{};
		filter.DenyList.NumIDs = _countof(denyIds);
		filter.DenyList.pIDList = denyIds;
		filter.DenyList.NumSeverities = _countof(severities);
		filter.DenyList.pSeverityList = severities;
		infoQueue->PushStorageFilter(&filter);
	}
#endif // _DEBUG


}



/// ---------------------------
/// ↓ Command関係を初期化
/// ---------------------------
void DirectXCommon::InitializeCommand() {
	HRESULT result = S_FALSE;

	/// ---------------------------
	/// ↓ CommandQueueを生成
	/// ---------------------------
	commandQueue_ = nullptr;
	D3D12_COMMAND_QUEUE_DESC desc{};

	///- 生成; 生成できたか確認
	result = device_->CreateCommandQueue(&desc, IID_PPV_ARGS(&commandQueue_));
	assert(SUCCEEDED(result));



	/// ---------------------------
	/// ↓ CommandAllocatorを生成
	/// ---------------------------
	commandAllocator_ = nullptr;

	///- 生成; 生成できたか確認
	result = device_->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator_));
	assert(SUCCEEDED(result));



	/// ---------------------------
	/// ↓ CommandListを生成
	/// ---------------------------
	commandList_ = nullptr;

	///- 生成; 生成できたか確認
	result = device_->CreateCommandList(
		0,
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		commandAllocator_.Get(),
		nullptr,
		IID_PPV_ARGS(&commandList_)
	);
	assert(SUCCEEDED(result));


}



/// ---------------------------
/// ↓ SwapChainを初期化
/// ---------------------------
void DirectXCommon::InitializeSwapChain() {
	HRESULT result = S_FALSE;

	swapChain_ = nullptr;

	swapChainDesc_.Width = kWindowSize.x;
	swapChainDesc_.Height = kWindowSize.y;
	swapChainDesc_.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc_.SampleDesc.Count = 1;
	//- 描画のターゲットとして利用
	swapChainDesc_.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	//- ダブルバッファを使用
	swapChainDesc_.BufferCount = 2;
	//- モニターに移したら中身を破棄
	swapChainDesc_.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

	///- SwapChain1で仮に生成
	ComPtr<IDXGISwapChain1> swapChain1;
	result = dxgiFactory_->CreateSwapChainForHwnd(
		commandQueue_.Get(), p_winApp_->GetHWND(), &swapChainDesc_, nullptr, nullptr, &swapChain1
	);
	assert(SUCCEEDED(result));

	///- SwapChain4に引き渡す
	result = swapChain1->QueryInterface(IID_PPV_ARGS(&swapChain_));
	assert(SUCCEEDED(result));

}



/// ---------------------------
/// ↓ RTVを初期化
/// ---------------------------
void DirectXCommon::InitialiezRenderTarget() {
	HRESULT result = S_FALSE;


	/// ---------------------------
	/// ↓ DescriptorHeapを生成
	/// ---------------------------
	rtvDescriptorHeap_ = nullptr;
	rtvDescriptorHeap_ = CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 2, false);
	assert(rtvDescriptorHeap_);



	/// ---------------------------
	/// ↓ SwapChainResourceを生成
	/// ---------------------------
	for(size_t i = 0; i < 2; i++) {
		swapChainResource_[i] = nullptr;
	}

	///- 取得出来なければ起動できない
	for(UINT i = 0; i < 2; i++) {
		result = swapChain_->GetBuffer(i, IID_PPV_ARGS(&swapChainResource_[i]));
		assert(SUCCEEDED(result));
	}



	/// ---------------------------
	/// ↓ RTVの設定
	/// ---------------------------

	rtvDesc_.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	rtvDesc_.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

	///- ディスクリプタの先頭を取得
	D3D12_CPU_DESCRIPTOR_HANDLE rtvStartHandl = rtvDescriptorHeap_->GetCPUDescriptorHandleForHeapStart();

	///- rtvHandleの生成
	rtvHandles_[0] = rtvStartHandl;
	device_->CreateRenderTargetView(swapChainResource_[0].Get(), &rtvDesc_, rtvHandles_[0]);

	rtvHandles_[1].ptr = rtvHandles_[0].ptr + device_->GetDescriptorHandleIncrementSize(
		D3D12_DESCRIPTOR_HEAP_TYPE_RTV
	);
	device_->CreateRenderTargetView(swapChainResource_[1].Get(), &rtvDesc_, rtvHandles_[1]);

}



/// ---------------------------
/// ↓ Fenceを初期化
/// ---------------------------
void DirectXCommon::InitializeFence() {
	HRESULT result = S_FALSE;

	fence_ = nullptr;
	fenceValue_ = 0;

	result = device_->CreateFence(
		fenceValue_, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence_)
	);
	assert(SUCCEEDED(result));

	fenceEvent_ = CreateEvent(NULL, FALSE, FALSE, NULL);
	assert(fenceEvent_ != nullptr);


}



/// ---------------------------
/// ↓ DxcCompilerの初期化
/// ---------------------------
void DirectXCommon::InitializeDxcCompiler() {
	HRESULT result = S_FALSE;

	result = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils_));
	assert(SUCCEEDED(result));
	result = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxcCompiler_));
	assert(SUCCEEDED(result));

	///- includeに対応するために設定を行う
	result = dxcUtils_->CreateDefaultIncludeHandler(&includeHandler_);
	assert(SUCCEEDED(result));

}



/// ---------------------------
/// ↓ シェーダのコンパイル用関数
/// ---------------------------
IDxcBlob* DirectXCommon::CompileShader(const std::wstring& filePath, const wchar_t* profile) {
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
	IDxcBlob* shaderBlob = nullptr;
	result = shaderResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shaderBlob), nullptr);
	assert(SUCCEEDED(result));
	//- 成功したログ出力
	Engine::ConsolePrint(std::format(L"Compile Succeended, path:{}, profile:{}\n", filePath, profile));

	///- 不要になったリソースの解放
	shaderSource.Reset();
	shaderResult.Reset();

	return shaderBlob;
}



/// ---------------------------
/// ↓ RootSignatureの初期化
/// ---------------------------
void DirectXCommon::InitializeRootSignature() {
	HRESULT result = S_FALSE;

	///- RootSignatureの作成
	D3D12_ROOT_SIGNATURE_DESC desc{};
	desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	///- RootParameter作成; PixelShaderのMaterialとVertexShaderのTransform
	///- PixelShader
	rootParameters_[0].ParameterType =
		D3D12_ROOT_PARAMETER_TYPE_CBV;					//- CBVを使う
	rootParameters_[0].ShaderVisibility =
		D3D12_SHADER_VISIBILITY_PIXEL;					//- PixelShaderを使う
	rootParameters_[0].Descriptor.ShaderRegister = 0;	//- レジスタ番号0とバインド
	///- VertexShader
	rootParameters_[1].ParameterType =
		D3D12_ROOT_PARAMETER_TYPE_CBV;					//- CBVを使う
	rootParameters_[1].ShaderVisibility =
		D3D12_SHADER_VISIBILITY_VERTEX;					//- VertexShaderを使う
	rootParameters_[1].Descriptor.ShaderRegister = 0;	//- レジスタ番号0とバインド

	///- 
	InitializeDescriptorRange();
	rootParameters_[2].ParameterType =
		D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;		//- DescriptorTableを使う
	rootParameters_[2].ShaderVisibility =
		D3D12_SHADER_VISIBILITY_PIXEL;					//- PixelShaderで使う
	rootParameters_[2].DescriptorTable.pDescriptorRanges = descriptorRange_;
	rootParameters_[2].DescriptorTable.NumDescriptorRanges =
		_countof(descriptorRange_);						//- Tableで利用する数


	/// ----------------------------------------------
	/// ↓ Samplerの設定
	/// ----------------------------------------------

	staticSamplers_[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;		//- バイリニアフィルタ
	staticSamplers_[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;	//- 0~1の範囲外をリピート
	staticSamplers_[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers_[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers_[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;	//- 比較しない
	staticSamplers_[0].MaxLOD = D3D12_FLOAT32_MAX;	//- ありったけのMipMapを使う
	staticSamplers_[0].ShaderRegister = 0;			//- レジスタ番号0を使う
	staticSamplers_[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;	//- PixelShaderで使う



	desc.pParameters = rootParameters_;					//- ルートパラメータ配列へのポインタ
	desc.NumParameters = _countof(rootParameters_);		//- 配列の長さ

	desc.pStaticSamplers = staticSamplers_;
	desc.NumStaticSamplers = _countof(staticSamplers_);

	///- シリアライズしてバイナリ
	result = D3D12SerializeRootSignature(
		&desc,
		D3D_ROOT_SIGNATURE_VERSION_1,
		&signatureBlob_,
		&errorBlob_
	);
	if(FAILED(result)) {
		Engine::ConsolePrint(reinterpret_cast<char*>(errorBlob_->GetBufferPointer()));
		assert(false);
	}

	///- バイナリを元に生成
	result = device_->CreateRootSignature(
		0, signatureBlob_->GetBufferPointer(),
		signatureBlob_->GetBufferSize(),
		IID_PPV_ARGS(&rootSignature_)
	);
	assert(SUCCEEDED(result));

}



/// ---------------------------
/// ↓ InputLayoutの初期化
/// ---------------------------
void DirectXCommon::InitializeInputLayout() {

	///- InputLayout
	inputElementDescs_[0].SemanticName = "POSITION";
	inputElementDescs_[0].SemanticIndex = 0;
	inputElementDescs_[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs_[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputElementDescs_[1].SemanticName = "TEXCOORD";
	inputElementDescs_[1].SemanticIndex = 0;
	inputElementDescs_[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDescs_[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;


	inputLayoutDesc_.pInputElementDescs = inputElementDescs_;
	inputLayoutDesc_.NumElements = _countof(inputElementDescs_);

}



/// ---------------------------
/// ↓ BlendStateの初期化
/// ---------------------------
void DirectXCommon::InitializeBlendState() {

	///- すべての色要素を書き込む
	blendDesc_.RenderTarget[0].RenderTargetWriteMask =
		D3D12_COLOR_WRITE_ENABLE_ALL;

}



/// ---------------------------
/// ↓ Rasterizerの初期化
/// ---------------------------
void DirectXCommon::InitializeRasterizer() {

	rasterizerDesc_.CullMode = D3D12_CULL_MODE_BACK;	//- 裏面を表示しない
	rasterizerDesc_.FillMode = D3D12_FILL_MODE_SOLID;	//- 三角形の中を塗りつぶす

}



/// ---------------------------
/// ↓ ShaderBlobの初期化
/// ---------------------------
void DirectXCommon::InitializeShaderBlob() {

	///- Shaderをコンパイルする
	vertexShaderBlob_ = CompileShader(L"./Engine/Object3d.VS.hlsl", L"vs_6_0");
	assert(vertexShaderBlob_ != nullptr);

	pixelShaderBlob_ = CompileShader(L"./Engine/Object3d.PS.hlsl", L"ps_6_0");
	assert(pixelShaderBlob_ != nullptr);


}



/// ---------------------------
/// ↓ PSOの初期化
/// ---------------------------
void DirectXCommon::InitializePSO() {
	HRESULT result = S_FALSE;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC desc{};

	desc.pRootSignature = rootSignature_.Get();	//- RootSignature
	desc.InputLayout = inputLayoutDesc_;		//- InputLayout

	//- Shader
	desc.VS = {
		vertexShaderBlob_->GetBufferPointer(),
		vertexShaderBlob_->GetBufferSize()
	};
	desc.PS = {
		pixelShaderBlob_->GetBufferPointer(),
		pixelShaderBlob_->GetBufferSize()
	};

	desc.BlendState = blendDesc_;			//- BlendState
	desc.RasterizerState = rasterizerDesc_; //- RasterizerState

	///- 書き込むRTVの情報
	desc.NumRenderTargets = 1;
	desc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	///- 利用するトロポジ(形状)のタイプ; 三角形
	desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	///- 画面に色を打ち込みかの設定
	desc.SampleDesc.Count = 1;
	desc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	///- 実際に生成
	result = device_->CreateGraphicsPipelineState(
		&desc, IID_PPV_ARGS(&graphicsPipelineState_)
	);
	assert(SUCCEEDED(result));


}



/// ---------------------------
/// ↓ VertexResourceの初期化
/// ---------------------------
void DirectXCommon::InitializeVertexResource() {
	HRESULT result = S_FALSE;


	/// ---------------------------
	/// ↓ VertexResourceの初期化
	/// ---------------------------

	vertexResource_.Attach(CreateBufferResource(sizeof(VertexData) * 3));


	/// ---------------------------
	/// ↓ VertexBufferViewを作成
	/// ---------------------------

	///- 頂点バッファビューを作成
	///- リソースの先頭のアドレスから使う
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	///- 使用するリソースのサイズ; 頂点数分確保する
	vertexBufferView_.SizeInBytes = sizeof(VertexData) * 3;
	///- 1頂点あたりのサイズ
	vertexBufferView_.StrideInBytes = sizeof(VertexData);



	/// ---------------------------
	/// ↓ Resourceにデータを書き込む
	/// ---------------------------

	VertexData* vertexData = nullptr;
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));

	vertexData[0].position = { -0.5f,-0.5f,0.0f,1.0f };	//- 左下
	vertexData[0].texcoord = { 0.0f,1.0f };
	vertexData[1].position = { 0.0f,0.5f,0.0f,1.0f };	//- 上
	vertexData[1].texcoord = { 0.5f,0.0f };
	vertexData[2].position = { 0.5f,-0.5f,0.0f,1.0f };	//- 右下
	vertexData[2].texcoord = { 1.0f,1.0f };


}



/// ---------------------------
/// ↓ viewportとscissorの初期化
/// ---------------------------
void DirectXCommon::InitializeViewport() {

	///- ビューポート
	viewport_.Width = static_cast<FLOAT>(kWindowSize.x);
	viewport_.Height = static_cast<FLOAT>(kWindowSize.y);
	viewport_.TopLeftX = 0;
	viewport_.TopLeftY = 0;
	viewport_.MinDepth = 0.0f;
	viewport_.MaxDepth = 1.0f;

	///- シザー矩形
	scissorRect_.left = 0;
	scissorRect_.right = kWindowSize.x;
	scissorRect_.top = 0;
	scissorRect_.bottom = kWindowSize.y;

}



/// ---------------------------
/// ↓ MaterialResourceの初期化
/// ---------------------------
void DirectXCommon::InitializeMaterialResource() {

	///- マテリアルリソースの生成
	materialResource_.Attach(CreateBufferResource(sizeof(Vector4)));

	WriteColor({ 1.0f,0.0f,0.0f,1.0f });

}



/// ---------------------------
/// ↓ wvpResourceの初期化
/// ---------------------------
void DirectXCommon::InitializeWVPResource() {

	///- wvpリソースの生成
	wvpResource_.Attach(CreateBufferResource(sizeof(Matrix4x4)));

	WriteWVPResource(Mat4::MakeIdentity());

}



/// ---------------------------
/// ↓ wvpResourceへの書き込み
/// ---------------------------
void DirectXCommon::WriteWVPResource(const Mat4& Matrix) {

	///- データの書き込み
	Matrix4x4* wvpData = nullptr;
	wvpResource_->Map(0, nullptr, reinterpret_cast<void**>(&wvpData));
	*wvpData = Matrix;

}



/// ---------------------------
/// ↓ BufferResourceの生成
/// ---------------------------
ID3D12Resource* DirectXCommon::CreateBufferResource(size_t sizeInBytes) {
	HRESULT result = S_FALSE;
	ID3D12Resource* resource = nullptr;

	///- 頂点リソース用のヒープ設定
	D3D12_HEAP_PROPERTIES uploadHeapProperties{};
	uploadHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;

	///- 頂点リソースの設定
	D3D12_RESOURCE_DESC desc{};
	///- バッファリソース
	desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	desc.Width = sizeInBytes; //- リソースのサイズ(頂点数

	///- バッファの場合これらは1に設定する決まり
	desc.Height = 1;
	desc.DepthOrArraySize = 1;
	desc.MipLevels = 1;
	desc.SampleDesc.Count = 1;

	///- バッファの場合はこれにする決まり
	desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	///- 実際に頂点リソースを生成
	result = device_->CreateCommittedResource(
		&uploadHeapProperties,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&resource)
	);
	assert(SUCCEEDED(result));

	return resource;
}



/// ---------------------------
/// ↓ 画面を一定の色で染める
/// ---------------------------
void DirectXCommon::ClearRenderTarget() {
	UINT bbIndex = swapChain_->GetCurrentBackBufferIndex();

	///- 描画先のRTVを設定する
	commandList_->OMSetRenderTargets(
		1, &rtvHandles_[bbIndex], false, nullptr
	);

	///- 指定した色で画面をクリア
	float clearColor[] = { 0.1f,0.25f,0.5f,1.0f };
	commandList_->ClearRenderTargetView(
		rtvHandles_[bbIndex], clearColor, 0, nullptr
	);
}



/// ---------------------------
/// ↓ Textureデータを読む
/// ---------------------------
DirectX::ScratchImage DirectXCommon::LoadTexture(const std::string& filePath) {
	///- テクスチャを読み込む
	DirectX::ScratchImage image{};
	std::wstring filePathW = Engine::ConvertString(filePath);
	HRESULT result = DirectX::LoadFromWICFile(filePathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
	assert(SUCCEEDED(result));

	///- ミップマップの作成
	DirectX::ScratchImage mipImages{};
	result = DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_SRGB, 0, mipImages);
	assert(SUCCEEDED(result));

	return mipImages;
}



/// ---------------------------
/// ↓ DirextX12のTextureResourceを作成
/// ---------------------------
ComPtr<ID3D12Resource> DirectXCommon::CreateTextureResource(const DirectX::TexMetadata& metadata) {

	/// --------------------------------------
	/// ↓ metadataを基にResourceの設定
	/// --------------------------------------
	D3D12_RESOURCE_DESC desc{};
	desc.Width = UINT(metadata.width);		//- textureの幅
	desc.Height = UINT(metadata.height);	//- textureの高さ
	desc.MipLevels = UINT16(metadata.mipLevels);		//- mipmapの数
	desc.DepthOrArraySize = UINT16(metadata.arraySize);	//- 奥行き or 配列Textureの配列数
	desc.Format = metadata.format;	//- TextureのFormat
	desc.SampleDesc.Count = 1;	//- サンプリングカウント; 1固定
	desc.Dimension = D3D12_RESOURCE_DIMENSION(metadata.dimension);	//- Textureの次元数


	/// --------------------------------------
	/// ↓ 利用するHeapの設定
	/// --------------------------------------

	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_CUSTOM;	//- 細かい設定を行う
	heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK; //- WriteBackポリシーでCPUアクセス可能
	heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;	//- プロセッサの近くに配置


	/// --------------------------------------
	/// ↓ Resourceを生成
	/// --------------------------------------

	ComPtr<ID3D12Resource> resource;
	HRESULT result = device_->CreateCommittedResource(
		&heapProperties,		//- Heapの設定
		D3D12_HEAP_FLAG_NONE,	//- Heapの特殊な設定
		&desc,					//- Resourceの設定
		D3D12_RESOURCE_STATE_GENERIC_READ,	//- 初回のResourceState; Textureは基本読むだけ
		nullptr,				//- Clear最適値; 使わないのでnullptr
		IID_PPV_ARGS(&resource)
	);
	assert(SUCCEEDED(result));

	return resource;
}



/// ---------------------------
/// ↓ TextureResourceにデータを転送する
/// ---------------------------
void DirectXCommon::UploadTextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages) {
	///- meta情報を取得
	const DirectX::TexMetadata& metadata = mipImages.GetMetadata();

	///- 全MipMapについて
	for(size_t mipLevel = 0; mipLevel < metadata.mipLevels; ++mipLevel) {
		///- MipMapLevelを指定して各Imageを取得
		const DirectX::Image* img = mipImages.GetImage(mipLevel, 0, 0);
		///- Textureに転送
		HRESULT result = texture->WriteToSubresource(
			UINT(mipLevel),
			nullptr,		//- 全領域へコピー
			img->pixels,	//- 元データアドレス
			UINT(img->rowPitch),	//- 1ラインサイズ
			UINT(img->slicePitch)	//- 1枚サイズ
		);
		assert(SUCCEEDED(result));
	}

}



/// ---------------------------
/// ↓ TextureResourceの初期化
/// ---------------------------
void DirectXCommon::InitializeTextureResource() {

	srvHeap_ = CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 128, true);

	DirectX::ScratchImage mipImages = LoadTexture("./Resources/Images/uvChecker.png");
	const  DirectX::TexMetadata& metadata = mipImages.GetMetadata();
	textureResource_ = CreateTextureResource(metadata);
	UploadTextureData(textureResource_.Get(), mipImages);

	///- metadataを基にSRVの設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = metadata.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = UINT(metadata.mipLevels);

	///- SRVを作成するDescriptorHeapの場所を決める
	textureSrvHandleCPU_ = srvHeap_->GetCPUDescriptorHandleForHeapStart();
	textureSrvHandleGPU_ = srvHeap_->GetGPUDescriptorHandleForHeapStart();
	///- 先頭はImGuiが使っているのでその次を使う
	textureSrvHandleCPU_.ptr += device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	textureSrvHandleGPU_.ptr += device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	///- srvの生成
	device_->CreateShaderResourceView(textureResource_.Get(), &srvDesc, textureSrvHandleCPU_);

}



/// ---------------------------
/// ↓ DescriptorRangeの初期化
/// ---------------------------
void DirectXCommon::InitializeDescriptorRange() {

	descriptorRange_[0].BaseShaderRegister = 0; //- 0から始まる
	descriptorRange_[0].NumDescriptors = 1; //- 数は1つ
	descriptorRange_[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV; //- SRVを使う
	descriptorRange_[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

}



/// ---------------------------
/// ↓ DescriptorHeap作成関数
/// ---------------------------
ComPtr<ID3D12DescriptorHeap> DirectXCommon::CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible) {
	ComPtr<ID3D12DescriptorHeap> heap;
	D3D12_DESCRIPTOR_HEAP_DESC desc{};
	desc.Type = heapType;
	desc.NumDescriptors = numDescriptors;
	desc.Flags = shaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	HRESULT result = device_->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&heap));
	assert(SUCCEEDED(result));

	return heap;
}



void DirectXCommon::WriteColor(const Vector4& color) {
	///- マテリアルにデータを書き込む
	Vector4* materialData = nullptr;
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData));
	*materialData = color;
}



/// ---------------------------
/// ↓ 描画前処理
/// ---------------------------
void DirectXCommon::PreDraw() {
	UINT bbIndex = swapChain_->GetCurrentBackBufferIndex();


	/// ---------------------------
	/// ↓ バリアを貼る; commandListを積めるようにする
	/// ---------------------------
	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = swapChainResource_[bbIndex].Get();
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	commandList_->ResourceBarrier(1, &barrier);

	///- 画面を一定の色で染める
	ClearRenderTarget();

}



/// ---------------------------
/// ↓ 描画後処理
/// ---------------------------
void DirectXCommon::PostDraw() {
	HRESULT result = S_FALSE;
	UINT bbIndex = swapChain_->GetCurrentBackBufferIndex();

	/// ---------------------------
	/// ↓ バリアを貼る; これから描画処理用に変更
	/// ---------------------------
	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = swapChainResource_[bbIndex].Get();
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	commandList_->ResourceBarrier(1, &barrier);

	///- Commandを閉じる
	result = commandList_->Close();
	assert(SUCCEEDED(result));

	///- コマンドをキックする
	ID3D12CommandList* commandLists[] = { commandList_.Get() };
	commandQueue_->ExecuteCommandLists(1, commandLists);

	///- GPUとOSに画面の交換を行うように通知する
	swapChain_->Present(1, 0);

	///- GPUにSignalを送信
	fenceValue_++;
	commandQueue_->Signal(fence_.Get(), fenceValue_);

	///- GPUの処理が終わっていなければイベントを待機する
	if(fence_->GetCompletedValue() < fenceValue_) {
		fence_->SetEventOnCompletion(fenceValue_, fenceEvent_);
		WaitForSingleObject(fenceEvent_, INFINITE);
	}


	///- 次のフレーム用にコマンドリストを準備
	result = commandAllocator_->Reset();
	assert(SUCCEEDED(result));
	result = commandList_->Reset(commandAllocator_.Get(), nullptr);
	assert(SUCCEEDED(result));

}



/// ---------------------------
/// ↓ 三角形の描画(テスト
/// ---------------------------
void DirectXCommon::TestDraw() {

	///- viewport; scissorRectを設定
	commandList_->RSSetViewports(1, &viewport_);
	commandList_->RSSetScissorRects(1, &scissorRect_);

	///- RootSignatureを設定; PSOとは別に別途設定が必要
	commandList_->SetGraphicsRootSignature(rootSignature_.Get());
	commandList_->SetPipelineState(graphicsPipelineState_.Get());
	commandList_->IASetVertexBuffers(0, 1, &vertexBufferView_);

	ImGui::Begin("Triangle");
	ImGui::SliderFloat4("color", &color_.x, 0.0f, 1.0f);
	ImGui::End();

	//worldTransform_.rotate.y += 1.0f / 64.0f;
	worldTransform_.MakeWorldMatrix();
	WriteWVPResource(worldTransform_.worldMatrix * camera_->GetVpMatrix());
	WriteColor(color_);

	///- 形状を設定; PSOに設定している物とはまだ別; 同じものを設定すると考えておけばOK
	commandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	///- マテリアルのCBufferの場所を設定
	commandList_->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
	///- wvp用のCBufferの場所を設定
	commandList_->SetGraphicsRootConstantBufferView(1, wvpResource_->GetGPUVirtualAddress());
	///- DescriptorTableを設定する
	commandList_->SetGraphicsRootDescriptorTable(2, textureSrvHandleGPU_);

	///- 描画 (DrawCall)
	commandList_->DrawInstanced(3, 1, 0, 0);

}



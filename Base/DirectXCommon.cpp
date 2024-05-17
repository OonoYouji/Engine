#include <DirectXCommon.h>

#include <d3dx12.h>
#include <imgui.h>

#include <format>

#include <Engine.h>
#include <DxDescriptors.h>
#include <DxCommand.h>
#include <ShaderCompile.h>
#include <TextureManager.h>
#include <WinApp.h>
#include <Environment.h>
#include <ImGuiManager.h>
#include <Vector4.h>

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

	///- DirectX Commnadの初期化
	command_ = DxCommand::GetInstance();
	command_->Initialize(device_.Get());

	///- DescriptorHeapのまとまりの初期化
	descriptors_ = DxDescriptors::GetInstance();
	descriptors_->Initialize();


	InitializeSwapChain();
	InitialiezRenderTarget();
	InitializeFence();

	shaderCompile_ = std::make_unique<ShaderCompile>();
	shaderCompile_->Initialize();

	InitializeRootSignature();
	InitializeInputLayout();
	InitializeBlendState();
	InitializeRasterizer();
	InitializeShaderBlob();
	InitializePSO();
	InitializeViewport();

	InitializeDepthStencil();

}



/// ---------------------------
/// ↓ メンバ変数の解放などを行う
/// ---------------------------
void DirectXCommon::Finalize() {

	/// ---------------------------
	/// ↓ 生成した逆順に解放していく
	/// ---------------------------

	DxDescriptors::GetInstance()->Finalize();

	depthStencilResource_.Reset();

	graphicsPipelineState_.Reset();
	pixelShaderBlob_.Reset();
	vertexShaderBlob_.Reset();
	rootSignature_.Reset();
	errorBlob_.Reset();
	signatureBlob_.Reset();
	shaderCompile_.reset();

	CloseHandle(fenceEvent_);
	fence_.Reset();
	for(UINT i = 0; i < 2; i++) {
		swapChainResource_[i].Reset();
	}
	swapChain_.Reset();
	
	///- Commandの解放
	command_->Finalize();

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
		command_->GetQueue(), p_winApp_->GetHWND(), &swapChainDesc_, nullptr, nullptr, &swapChain1
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
	/// ↓ SwapChainResourceを生成
	/// ---------------------------
	swapChainResource_.resize(2);
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
	//rtvDesc_.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	rtvDesc_.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

	///- ディスクリプタの先頭を取得
	D3D12_CPU_DESCRIPTOR_HANDLE rtvStartHandl = DxDescriptors::GetInstance()->GetRTVHeap()->GetCPUDescriptorHandleForHeapStart();

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
/// ↓ RootSignatureの初期化
/// ---------------------------
void DirectXCommon::InitializeRootSignature() {
	HRESULT result = S_FALSE;

	///- RootSignatureの作成
	D3D12_ROOT_SIGNATURE_DESC desc{};
	desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;


	descriptorRange_[0].BaseShaderRegister = 0; //- 0から始まる
	descriptorRange_[0].NumDescriptors = 1; //- textureの数
	descriptorRange_[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV; //- SRVを使う
	descriptorRange_[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;


	///- RootParameter作成; PixelShaderのMaterialとVertexShaderのTransform
	///- PixelShader
	rootParameters_[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;		//- CBVを使う
	rootParameters_[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;	//- PixelShaderを使う
	rootParameters_[0].Descriptor.ShaderRegister = 0;	//- レジスタ番号0とバインド

	///- VertexShader
	rootParameters_[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;		//- CBVを使う
	rootParameters_[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;	//- VertexShaderを使う
	rootParameters_[1].Descriptor.ShaderRegister = 0;	//- レジスタ番号0とバインド

	///- texutre
	rootParameters_[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameters_[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters_[2].DescriptorTable.pDescriptorRanges = &descriptorRange_[0];
	rootParameters_[2].DescriptorTable.NumDescriptorRanges = descriptorRange_[0].NumDescriptors;

	///- light
	rootParameters_[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters_[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters_[3].Descriptor.ShaderRegister = 1;


	/// ----------------------------------------------
	/// ↓ Samplerの設定
	/// ----------------------------------------------

	staticSamplers_[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;		//- バイリニアフィルタ
	staticSamplers_[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;		//- 0~1の範囲外をリピート
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

	inputElementDescs_[2].SemanticName = "NORMAL";
	inputElementDescs_[2].SemanticIndex = 0;
	inputElementDescs_[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDescs_[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;


	inputLayoutDesc_.pInputElementDescs = inputElementDescs_;
	inputLayoutDesc_.NumElements = _countof(inputElementDescs_);

}



/// ---------------------------
/// ↓ BlendStateの初期化
/// ---------------------------
void DirectXCommon::InitializeBlendState() {

	///- すべての色要素を書き込む
	blendDesc_.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

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
	vertexShaderBlob_ = shaderCompile_->CompileShader(L"./Shader/Object3D/Object3d.VS.hlsl", L"vs_6_0");
	assert(vertexShaderBlob_ != nullptr);

	pixelShaderBlob_ = shaderCompile_->CompileShader(L"./Shader/Object3D/Object3d.PS.hlsl", L"ps_6_0");
	assert(pixelShaderBlob_ != nullptr);


}



/// ---------------------------
/// ↓ PSOの初期化
/// ---------------------------
void DirectXCommon::InitializePSO() {
	HRESULT result = S_FALSE;


	/// ---------------------------
	/// ↓ depthStencilの設定
	/// ---------------------------

	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	depthStencilDesc.DepthEnable = true;	//- Depth機能の有効化
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;	//- 書き込みする
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;	//- 比較関数の設定; 近ければ描画される


	/// ---------------------------
	/// ↓ PSOの設定
	/// ---------------------------

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
	///- DepthStencilの設定
	desc.DepthStencilState = depthStencilDesc;
	desc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	///- 実際に生成
	result = device_->CreateGraphicsPipelineState(
		&desc, IID_PPV_ARGS(&graphicsPipelineState_)
	);
	assert(SUCCEEDED(result));


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

	viewportMatrix_ = Matrix4x4::MakeViewportMatrix(
		viewport_.TopLeftY,
		viewport_.TopLeftY,
		viewport_.Width,
		viewport_.Height,
		viewport_.MinDepth,
		viewport_.MaxDepth
	);

	///- シザー矩形
	scissorRect_.left = 0;
	scissorRect_.right = kWindowSize.x;
	scissorRect_.top = 0;
	scissorRect_.bottom = kWindowSize.y;

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



void DirectXCommon::ClearDepthBuffer() {
	// 深度ステンシルビュー用デスクリプタヒープのハンドルを取得
	CD3DX12_CPU_DESCRIPTOR_HANDLE dsvH =
		CD3DX12_CPU_DESCRIPTOR_HANDLE(descriptors_->GetDSVHeap()->GetCPUDescriptorHandleForHeapStart());
	// 深度バッファのクリア
	command_->GetList()->ClearDepthStencilView(dsvH, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
}



/// ---------------------------
/// ↓ 画面を一定の色で染める
/// ---------------------------
void DirectXCommon::ClearRenderTarget() {
	UINT bbIndex = swapChain_->GetCurrentBackBufferIndex();

	///- 描画先のRTVを設定する
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = DxDescriptors::GetInstance()->GetDSVHeap()->GetCPUDescriptorHandleForHeapStart();
	command_->GetList()->OMSetRenderTargets(
		1, &rtvHandles_[bbIndex], false, &dsvHandle
	);

	///- 指定した色で画面をクリア
	float clearColor[] = { 0.1f,0.25f,0.5f,1.0f };
	command_->GetList()->ClearRenderTargetView(
		rtvHandles_[bbIndex], clearColor, 0, nullptr
	);
}




/// ---------------------------
/// ↓ DescriptorRangeの初期化
/// ---------------------------
void DirectXCommon::InitializeDescriptorRange() {

	descriptorRange_[0].BaseShaderRegister = 0; //- 0から始まる
	descriptorRange_[0].NumDescriptors = 2; //- textureの数
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



/// ---------------------------
/// ↓ DepthStencilの生成
/// ---------------------------
void DirectXCommon::InitializeDepthStencil() {

	depthStencilResource_ = CreateDepthStencilTextureResource(kWindowSize.x, kWindowSize.y);

	///- Heap上にDSVを構築する
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;

	///- DsvHeapの先頭にDsvを作る
	device_->CreateDepthStencilView(depthStencilResource_.Get(), &dsvDesc, DxDescriptors::GetInstance()->GetDSVHeap()->GetCPUDescriptorHandleForHeapStart());

}



/// ---------------------------
/// ↓ DepthStencilTextureを作成
/// ---------------------------
ComPtr<ID3D12Resource> DirectXCommon::CreateDepthStencilTextureResource(int32_t width, int32_t height) {

	///- 生成するResourceの設定
	D3D12_RESOURCE_DESC desc{};
	desc.Width = width;		//- Textureの幅
	desc.Height = height;	//- Textureの高さ
	desc.MipLevels = 1;		//- mipmapの数
	desc.DepthOrArraySize = 1;	//- 奥行き or 配列Textureの配列数
	desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;			//- DepthStencilとして利用可能なフォーマット
	desc.SampleDesc.Count = 1;	//- サンプリングカウント; 1固定
	desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;	//- 2次元
	desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;	//- DepthStencilとして使う通知

	///- 利用するHeapの設定
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT; //- VRAM上に作る

	///- 深度値のクリア最適化設定
	D3D12_CLEAR_VALUE depthClearValue{};
	depthClearValue.DepthStencil.Depth = 1.0f;		//- 1.0f(最大値)でクリア
	depthClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;	//- フォーマット; Resourceと合わせる

	///- Resourceの生成
	ComPtr<ID3D12Resource> resource = nullptr;
	HRESULT result = device_->CreateCommittedResource(
		&heapProperties,		//- Heapの設定
		D3D12_HEAP_FLAG_NONE,	//- Heapの特殊な設定; 特になし
		&desc,					//- Resourceの設定
		D3D12_RESOURCE_STATE_DEPTH_WRITE,	//- 深度値を書き込む状態にしておく
		&depthClearValue,		//- Clear最適値
		IID_PPV_ARGS(&resource)	//- 作成するResourceポインタへのポインタ
	);
	assert(SUCCEEDED(result));

	return resource;
}



/// ---------------------------
/// ↓ 描画前処理
/// ---------------------------
void DirectXCommon::PreDraw() {
	UINT bbIndex = swapChain_->GetCurrentBackBufferIndex();

	ID3D12GraphicsCommandList* commandList = command_->GetList();

	///- 深度をクリア
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = DxDescriptors::GetInstance()->GetDSVHeap()->GetCPUDescriptorHandleForHeapStart();
	commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

	///- 書き込み用にバリアーを貼る
	command_->CreateBarrier(bbIndex, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

	///- 画面を一定の色で染める
	ClearRenderTarget();

	///- viewport; scissorRectを設定
	commandList->RSSetViewports(1, &viewport_);
	commandList->RSSetScissorRects(1, &scissorRect_);

	///- RootSignatureを設定; PSOとは別に別途設定が必要
	commandList->SetGraphicsRootSignature(rootSignature_.Get());
	commandList->SetPipelineState(graphicsPipelineState_.Get());

}



/// ---------------------------
/// ↓ 描画後処理
/// ---------------------------
void DirectXCommon::PostDraw() {
	HRESULT result = S_FALSE;
	UINT bbIndex = swapChain_->GetCurrentBackBufferIndex();
	ID3D12GraphicsCommandList* commandList = command_->GetList();

	///- 描画用にバリアーを貼る
	command_->CreateBarrier(bbIndex, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);


	///- Commandを閉じる
	result = commandList->Close();
	assert(SUCCEEDED(result));

	///- コマンドをキックする
	ID3D12CommandList* commandLists[] = { commandList };
	command_->GetQueue()->ExecuteCommandLists(1, commandLists);

	ImGuiManager::GetInstance()->RenderMultiViewport();

	///- GPUとOSに画面の交換を行うように通知する
	swapChain_->Present(1, 0);

	///- GPUにSignalを送信
	fenceValue_++;
	command_->GetQueue()->Signal(fence_.Get(), fenceValue_);

	///- GPUの処理が終わっていなければイベントを待機する
	if(fence_->GetCompletedValue() < fenceValue_) {
		fence_->SetEventOnCompletion(fenceValue_, fenceEvent_);
		WaitForSingleObject(fenceEvent_, INFINITE);
	}


	///- 次のフレームのためにQueueをリセットする
	command_->ResetCommandList();
	ClearDepthBuffer();

}


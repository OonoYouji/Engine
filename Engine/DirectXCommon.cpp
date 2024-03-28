#include <DirectXCommon.h>

#include <Engine.h>
#include <DXCompile.h>
#include <Vector4.h>
#include <Matrix4x4.h>
#include <DXCompile.h>
#include <Environment.h>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "Winmm.lib")
#pragma comment(lib, "dxcompiler.lib")

using namespace Microsoft::WRL;


void DirectXCommon::Initialize(WinApp* winApp, int32_t backBufferWidth, int32_t backBufferHeight) {

	assert(winApp);

	p_winApp_ = winApp;

	backBufferWidth_ = backBufferWidth;
	backBufferHeight_ = backBufferHeight;

	/// デバイスの初期化
	InitializeDXGIDevice();
	/// コマンド関連の初期化
	InitializeCommand();
	/// スワップチェーンの生成
	CreateSwapChain();
	/// レンダーターゲット生成
	CreateFinalRenderTargets();
	/// フェンスの生成
	CreateFence();


	/// DXCの初期化
	InitializeDXC();
	///
	InitializeDescriptorRange();
	/// ルートシグネチャの生成
	CreateRootSignature();
	/// インプットレイアウトの設定
	InitializeInputLayout();
	/// ブレンドステートの設定
	InitializeBlendState();
	/// ラスタライザーステートの設定
	InitializeRasterizerState();
	/// 
	InitializeShader();
	/// PSOの生成
	InitializePSO();
	/// VertexResourceの生成
	InitializeVertexResource();
	///
	InitializeViewport();
	/// マテリアルリソースの生成
	CreateResource();
	///
	InitializeTextureResource();
	///
	CreateShaderResourceView();

}

void DirectXCommon::Finalize() {

	p_winApp_ = nullptr;

	/// -------------------------------
	/// ↓ オブジェクトの解放; 生成の逆順
	/// -------------------------------

	srvDescriptorHeap_->Release();

	intermediateResource_->Release();
	textureResource_->Release();

	wvpResource_->Release();
	materialResource_->Release();
	vertexResource_->Release();

	graphicsPipelineState_->Release();
	depthStencilResource_->Release();
	dsvDescriptorHeap_->Release();

	pixelShaderBlob_->Release();
	vertexShaderBlob_->Release();

	rootSignature_->Release();
	signatureBlob_->Release();
	if(errorBlob_) {
		errorBlob_->Release();
	}

	includeHandler_->Release();
	dxcUtils_->Release();
	dxcCompiler_->Release();


	CloseHandle(fenceEvent_);
	fence_->Release();

	swapChainResource_[1]->Release();
	swapChainResource_[0]->Release();
	rtvDescriptorHeap_->Release();
	
	swapChain_->Release();

	commandList_->Release();
	commandAllocator_->Release();
	commandQueue_->Release();
	
	device_->Release();
	useAdapter_->Release();
	dxgiFactory_->Release();

#ifdef _DEBUG
	debugController_->Release();
#endif // _DEBUG

	

}

DirectXCommon* DirectXCommon::GetInstance() {
	static DirectXCommon instance;
	return &instance;
}

void DirectXCommon::PreDraw() {


	UINT bbIndex = swapChain_->GetCurrentBackBufferIndex();

	//// バリアーを張る -----
	D3D12_RESOURCE_BARRIER barrier{};
	/// 今回のバリアはトランジション
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	/// Noneにしておく
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	/// バリアを張る対象のリソース; 現在のバックバッファに対して行う
	barrier.Transition.pResource = swapChainResource_[bbIndex].Get();
	/// 遷移前(現在)のResourceState
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	/// 遷移後のResourceState
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;

	/// TransitionBarrierを張る
	commandList_->ResourceBarrier(1, &barrier);

	ClearRenderTarget();


}

void DirectXCommon::PostDraw() {
	HRESULT hr = S_FALSE;

	UINT bbIndex = swapChain_->GetCurrentBackBufferIndex();

	//// バリアーを張る -----
	D3D12_RESOURCE_BARRIER barrier{};
	/// 今回のバリアはトランジション
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	/// Noneにしておく
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	/// バリアを張る対象のリソース; 現在のバックバッファに対して行う
	barrier.Transition.pResource = swapChainResource_[bbIndex].Get();
	/// 遷移前(現在)のResourceState
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	/// 遷移後のResourceState
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;

	/// TransitionBarrierを張る
	commandList_->ResourceBarrier(1, &barrier);


	/// コマンドリストの内容を確定させる(すべてのコマンドを積んでからCloseする)
	hr = commandList_->Close();
	assert(SUCCEEDED(hr));

	/// コマンドリストの実行
	ID3D12CommandList* commandLists[] = { commandList_.Get()};
	commandQueue_->ExecuteCommandLists(1, commandLists);

	/// GPUとOSに画面の交換を行うよう通知する
	swapChain_->Present(1, 0);


	fenceValue_++;
	commandQueue_->Signal(fence_.Get(), fenceValue_);
	if (fence_->GetCompletedValue() < fenceValue_) {
		fence_->SetEventOnCompletion(fenceValue_, fenceEvent_);
		WaitForSingleObject(fenceEvent_, INFINITE);
	}

	/// 次のフレームのコマンドリストを準備
	hr = commandAllocator_->Reset();
	assert(SUCCEEDED(hr));
	hr = commandList_->Reset(commandAllocator_.Get(), nullptr);
	assert(SUCCEEDED(hr));

}

void DirectXCommon::ClearRenderTarget() {

	/// これから書き込むバックバッファのインデックスを所得(二枚しかないので 0 か 1)
	UINT bbIndex = swapChain_->GetCurrentBackBufferIndex();

	dsvHandle_ = dsvDescriptorHeap_->GetCPUDescriptorHandleForHeapStart();
	
	/// 指定した深度で画面全体をクリアする
	commandList_->ClearDepthStencilView(dsvHandle_, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

	/// 描画先のRTVとDSVを設定する
	commandList_->OMSetRenderTargets(1, &rtvHandles_[bbIndex], false, &dsvHandle_);

	/// 指定した色で画面全体をクリアする
	float clearColor[] = { 0.1f, 0.25f, 0.5f, 1.0f }; /// 青っぽい色, RGBAの順番
	commandList_->ClearRenderTargetView(rtvHandles_[bbIndex], clearColor, 0, nullptr);

}

void DirectXCommon::DebugReleaseCheck() {
	//// 解放されていないものがあれば止まる -----
	ComPtr<IDXGIDebug1> debug = nullptr;
	if(SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug)))) {
		debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
		debug->ReportLiveObjects(DXGI_DEBUG_APP, DXGI_DEBUG_RLO_ALL);
		debug->ReportLiveObjects(DXGI_DEBUG_D3D12, DXGI_DEBUG_RLO_ALL);
		debug->Release();
	}
}


void DirectXCommon::InitializeDXGIDevice() {
	HRESULT hr = S_FALSE;


#ifdef _DEBUG

	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController_)))) {
		/// デバッグレイヤーを有効化する
		debugController_->EnableDebugLayer();
		/// さらにGPU側でもチェックできるようにする
		debugController_->SetEnableGPUBasedValidation(TRUE);
	}

#endif // _DEBUG

	/// DXGIファクトリーの生成
	hr = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory_));
	assert(SUCCEEDED(hr));


	/// 使用するアダプタ用変数

	/// いい順にアダプタを頼む
	for (UINT i = 0; dxgiFactory_->EnumAdapterByGpuPreference(i,
		DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&useAdapter_)) != DXGI_ERROR_NOT_FOUND; i++) {

		/// アダプタの情報を取得する
		DXGI_ADAPTER_DESC3 adapterDesc{};
		hr = useAdapter_->GetDesc3(&adapterDesc);
		assert(SUCCEEDED(hr)); /// 所得出来ないのは一大事

		/// ソフトウェアアダプタでなければ採用
		if (!(adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE)) {
			/// 採用したアダプタの情報をログに出力
			Engine::ConsolePrint(std::format(L"Use Adapter:{}\n", adapterDesc.Description));
			break;
		}

		/// ソフトウェアアダプタの場合は無視
		useAdapter_ = nullptr;

	}

	/// 適切なアダプタがなければ起動できない
	assert(useAdapter_ != nullptr);



	D3D_FEATURE_LEVEL featureLevels[] = {
		D3D_FEATURE_LEVEL_12_2,
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0
	};

	const char* featureLovelString[] = { "12.2", "12.1", "12.0" };

	/// 高い順に生成できるか試す
	for (size_t i = 0; i < _countof(featureLevels); i++) {

		/// 採用したアダプターでデバイスを生成
		hr = D3D12CreateDevice(useAdapter_.Get(), featureLevels[i], IID_PPV_ARGS(&device_));

		/// 指定した機能レベルでデバイスが生成できるかを確認
		if (SUCCEEDED(hr)) {
			/// 生成できたのでログ出力を行ってループを抜ける
			Engine::ConsolePrint(std::format("FeatureLevel : {}", featureLovelString[i]));
			break;
		}

	}

	/// デバイスの生成が失敗したら起動できない
	assert(device_ != nullptr);

	Engine::ConsolePrint("Complete create D3D12Device!!!!!\n");

#ifdef _DEBUG

	ID3D12InfoQueue* infoQueue = nullptr;
	if (SUCCEEDED(device_->QueryInterface(IID_PPV_ARGS(&infoQueue)))) {
		/// ヤバいエラーのとき止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
		/// エラーのときに止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
		/// 警告時に止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);
		infoQueue->Release();
	}

#endif // _DEBUG


}

void DirectXCommon::InitializeCommand() {
	HRESULT hr = S_FALSE;


	/// コマンドキューを生成する ------------------------
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
	hr = device_->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&commandQueue_));
	/// コマンドキューの生成が上手くいかないと起動できない
	assert(SUCCEEDED(hr));


	/// コマンドアロケータを生成する ----------------------
	hr = device_->CreateCommandAllocator(
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(&commandAllocator_)
	);
	/// 生成に失敗したら起動できない
	assert(SUCCEEDED(hr));


	/// コマンドリストを生成 ----------------------------
	hr = device_->CreateCommandList(
		0,
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		commandAllocator_.Get(),
		nullptr,
		IID_PPV_ARGS(&commandList_)
	);
	/// 生成に失敗したら起動できない
	assert(SUCCEEDED(hr));


}

void DirectXCommon::CreateSwapChain() {
	HRESULT hr = S_FALSE;

	/// スワップチェーンを生成する
	//// 画面の幅と高さ; windowのクライアント領域と同じものにしておく;
	swapChainDesc_.Width = backBufferWidth_;
	swapChainDesc_.Height = backBufferHeight_;
	/// 色の形式
	swapChainDesc_.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	/// マルチサンプルしない
	swapChainDesc_.SampleDesc.Count = 1;
	/// 描画のターゲットとして利用する
	swapChainDesc_.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	/// ダブルバッファ
	swapChainDesc_.BufferCount = 2;
	/// モニタに移したら中身を破棄
	swapChainDesc_.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;


	//// コマンドキュー、ウィンドウハンドル、設定を渡して生成する
	ComPtr<IDXGISwapChain1> swapChain1;
	hr = dxgiFactory_->CreateSwapChainForHwnd(
		commandQueue_.Get(),
		p_winApp_->GetHWND(),
		&swapChainDesc_,
		nullptr,
		nullptr,
		&swapChain1
	);

	/// SwapChain4を得る
	swapChain1->QueryInterface(IID_PPV_ARGS(&swapChain_));

	/// 失敗したら起動できない
	assert(SUCCEEDED(hr));

	//swapChain1->Release();

}



void DirectXCommon::CreateFinalRenderTargets() {
	HRESULT hr = S_FALSE;

	///// ディスクリプタヒープの生成 /////
	/// レンダーターゲットビュー用
	/// ダブルバッファ用に2つ; 多くても構わない
	rtvDescriptorHeap_ = CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 2, false);
	/// 生成できなかったら起動できない
	assert(SUCCEEDED(hr));


	hr = swapChain_->GetBuffer(0, IID_PPV_ARGS(&swapChainResource_[0]));
	assert(SUCCEEDED(hr));
	hr = swapChain_->GetBuffer(1, IID_PPV_ARGS(&swapChainResource_[1]));
	assert(SUCCEEDED(hr));


	/// RTVの設定
	rtvDesc_.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	rtvDesc_.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

	/// ディスクリプタの先頭を所得する
	D3D12_CPU_DESCRIPTOR_HANDLE rtvStartHandle = rtvDescriptorHeap_->GetCPUDescriptorHandleForHeapStart();


	/// まず1つ目を作る;1つ目は最初のところに作る;作る場所をこちらで指定してあげる必要がある
	rtvHandles_[0] = rtvStartHandle;
	device_->CreateRenderTargetView(swapChainResource_[0].Get(), &rtvDesc_, rtvHandles_[0]);
	/// 2つ目のディスクリプタハンドルを得る
	rtvHandles_[1].ptr = rtvHandles_[0].ptr + device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	/// 2つ目を作る
	device_->CreateRenderTargetView(swapChainResource_[1].Get(), &rtvDesc_, rtvHandles_[1]);


}

void DirectXCommon::CreateFence() {

	HRESULT hr = S_FALSE;

	hr = device_->CreateFence(fenceValue_, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence_));
	assert(SUCCEEDED(hr));

	fenceEvent_ = CreateEvent(NULL, FALSE, FALSE, NULL);
	assert(fenceEvent_ != nullptr);

}

void DirectXCommon::InitializeDXC() {

	HRESULT hr = S_FALSE;

	hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils_));
	assert(SUCCEEDED(hr));

	hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxcCompiler_));
	assert(SUCCEEDED(hr));

	hr = dxcUtils_->CreateDefaultIncludeHandler(&includeHandler_);
	assert(SUCCEEDED(hr));

}

void DirectXCommon::CreateRootSignature() {

	HRESULT hr = S_FALSE;

	/// RootSigneture作成
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSigneture{};
	descriptionRootSigneture.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;


	/// RootSignature作成; 複数設定できるので配列; 今回は1つしか設定しないので長さ1の配列;
	rootParameters_[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;		// CBVを使う
	rootParameters_[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;	// PixelShaderで使う
	rootParameters_[0].Descriptor.ShaderRegister = 0;						// レジスタ番号0とバインド

	rootParameters_[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;		// CBVを使う
	rootParameters_[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;	// VertexShaderを使う
	rootParameters_[1].Descriptor.ShaderRegister = 0;						// レジスタ番号0を使う

	rootParameters_[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;		 // DescriptorTableを使う
	rootParameters_[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;				 // PixelShaderを使う
	rootParameters_[2].DescriptorTable.pDescriptorRanges = descriptorRange_;			 // Tableの中身の配列を指定
	rootParameters_[2].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange_); // Tableで利用する数


	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR; //- バイリニアフィルタ
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP; //- 0~1の範囲外をリピート
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER; //- 比較しない
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX; //- ありったけのMipmapを使う
	staticSamplers[0].ShaderRegister = 0; //- レジスタ番号0を使う
	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; //- PixelShaderで使う

	descriptionRootSigneture.pStaticSamplers = staticSamplers;
	descriptionRootSigneture.NumStaticSamplers = _countof(staticSamplers);
	descriptionRootSigneture.pParameters = rootParameters_;					// ルートパラメータ配列へのポインタ
	descriptionRootSigneture.NumParameters = _countof(rootParameters_);		// 配列の長さs

	/// シリアライズしてバイナリする

	hr = D3D12SerializeRootSignature(
		&descriptionRootSigneture,
		D3D_ROOT_SIGNATURE_VERSION_1,
		&signatureBlob_,
		&errorBlob_
	);

	if(FAILED(hr)) {
		Engine::ConsolePrint(reinterpret_cast<char*>(errorBlob_->GetBufferPointer()));
		assert(false);
	}

	/// バイナリを元に生成
	hr = device_->CreateRootSignature(
		0,
		signatureBlob_->GetBufferPointer(),
		signatureBlob_->GetBufferSize(),
		IID_PPV_ARGS(&rootSignature_)
	);
	assert(SUCCEEDED(hr));

}

void DirectXCommon::TestDraw(const Matrix4x4& worldMatrix) {

	commandList_->RSSetViewports(1, &viewport_);
	commandList_->RSSetScissorRects(1, &scissorRect_);

	/// RootSignatureを設定; PS0に設定しているけど別途設定が必要
	commandList_->SetGraphicsRootSignature(rootSignature_.Get());
	commandList_->SetPipelineState(graphicsPipelineState_.Get());

	VertexData* vertexData = nullptr;
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));

	/// 一枚目の三角形
	vertexData[0].position = { -0.5f,-0.5f,0.0f,1.0f };
	vertexData[0].texcoord = Vec2f{ 0.0f,1.0f };

	vertexData[1].position = { 0.0f,0.5f,0.0f,1.0f };
	vertexData[1].texcoord = Vec2f{ 0.5f,0.0f };

	vertexData[2].position = { 0.5f,-0.5f,0.0f,1.0f };
	vertexData[2].texcoord = Vec2f{ 1.0f,1.0f };

	/// 二枚目の三角形
	vertexData[3].position = { -0.5f,-0.5f,0.5f,1.0f };
	vertexData[3].texcoord = Vec2f{ 0.0f,1.0f };

	vertexData[4].position = { 0.0f,0.0f,0.0f,1.0f };
	vertexData[4].texcoord = Vec2f{ 0.5f,0.0f };

	vertexData[5].position = { 0.5f,-0.5f,-0.5f,1.0f };
	vertexData[5].texcoord = Vec2f{ 1.0f,1.0f };

	commandList_->IASetVertexBuffers(0, 1, &vertexBufferView_);

	/// 形状を設定; PS0に設定している物とはまた別; 同じものを設定すると考えておけばいい
	commandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	/// マテリアルCBufferの場所を設定
	commandList_->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());

	/// wvp用のCBufferの場所を指定
	Matrix4x4* wvpData = nullptr;
	wvpResource_->Map(0, nullptr, reinterpret_cast<void**>(&wvpData));
	*wvpData = worldMatrix * Engine::GetCamera()->GetVpMatrix();
	commandList_->SetGraphicsRootConstantBufferView(1, wvpResource_->GetGPUVirtualAddress());

	/// SRVのDescriptorTableの先頭を設定; 2はrootParameter[2]である;
	commandList_->SetGraphicsRootDescriptorTable(2, textureSrvHandleGPU_);

	/// 描画! (DrawCall/ドローコール); 3頂点*2で1つのインスタンス; インスタンスについては今後
	commandList_->DrawInstanced(6, 1, 0, 0);
}

void DirectXCommon::InitializeInputLayout() {

	/// InputLayout
	inputElemntDescs_[0].SemanticName = "POSITION";
	inputElemntDescs_[0].SemanticIndex = 0;
	inputElemntDescs_[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElemntDescs_[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputElemntDescs_[1].SemanticName = "TEXCOORD";
	inputElemntDescs_[1].SemanticIndex = 0;
	inputElemntDescs_[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElemntDescs_[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputlayoutDesc_.pInputElementDescs = inputElemntDescs_;
	inputlayoutDesc_.NumElements = _countof(inputElemntDescs_);

}

void DirectXCommon::InitializeBlendState() {
	/// BlendStateの設定
	/// すべての色要素を書き込む
	blendDesc_.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
}

void DirectXCommon::InitializeRasterizerState() {

	/// RasterizerStateの設定
	/// 裏面(時計回り)を表示しない
	rasterizerDesc_.CullMode = D3D12_CULL_MODE_BACK;
	/// 三角形の中を塗りつぶす
	rasterizerDesc_.FillMode = D3D12_FILL_MODE_SOLID;

}

void DirectXCommon::InitializeShader() {

	vertexShaderBlob_ = CompileShader(Engine::ConvertString("./Engine/Object3d.VS.hlsl"), L"vs_6_0");
	assert(vertexShaderBlob_ != nullptr);

	pixelShaderBlob_ = CompileShader(Engine::ConvertString("./Engine/Object3d.PS.hlsl"), L"ps_6_0");
	assert(pixelShaderBlob_ != nullptr);

}


IDxcBlob* DirectXCommon::CompileShader(const std::wstring& filePath, const wchar_t* profile) {

	HRESULT hr = S_FALSE;

	Engine::ConsolePrint(std::format(L"Begin Compile, path:{}, profile:{}\n", filePath, profile));

	/// hlslファイルを読み込む
	ComPtr<IDxcBlobEncoding> shaderSource = nullptr;
	hr = dxcUtils_->LoadFile(filePath.c_str(), nullptr, &shaderSource);
	assert(SUCCEEDED(hr));

	/// ファイルの内容を設定する
	DxcBuffer shaderSourceBuffer;
	shaderSourceBuffer.Ptr = shaderSource->GetBufferPointer();
	shaderSourceBuffer.Size = shaderSource->GetBufferSize();
	shaderSourceBuffer.Encoding = DXC_CP_UTF8; // UTF8の文字コードであることを通知

	LPCWSTR arguments[] = {
		filePath.c_str(),			/// コンパイル対象のhlslファイル名
		L"-E", L"main",			/// エントリーポイントの指定。基本的にmain以外指定しない
		L"-T", profile,			/// ShaderProfileの設定
		L"-Zi", L"-Qembed_debug",	/// デバッグ用の情報を埋め込む
		L"-Od",					/// 最適化を外しておく	
		L"-Zpr",					/// メモリのレイアウトは行優先
	};

	/// 実際にShaderをコンパイルする
	ComPtr<IDxcResult> shaderResult = nullptr;
	hr = dxcCompiler_->Compile(
		&shaderSourceBuffer,
		arguments,
		_countof(arguments),
		includeHandler_.Get(),
		IID_PPV_ARGS(&shaderResult)
	);
	assert(SUCCEEDED(hr));


	/// 警告・エラーが出ていたらログに出して止める
	ComPtr<IDxcBlobUtf8> shaderError = nullptr;
	shaderResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&shaderError), nullptr);
	if(shaderError != nullptr && shaderError->GetStringLength() != 0) {
		Engine::ConsolePrint(shaderError->GetStringPointer());
		assert(false);
	}

	/// コンパイル結果から実行用のバイナリ部分を取得
	IDxcBlob* shaderBlob = nullptr;
	hr = shaderResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shaderBlob), nullptr);
	assert(SUCCEEDED(hr));

	/// 成功したログの出力
	Engine::ConsolePrint(std::format(L"Compile Succeded, path:{}, prifile:{}\n", filePath, profile));

	/// リソースの解放
	shaderSource->Release();
	shaderResult->Release();
	shaderError->Release();

	return shaderBlob;
}

void DirectXCommon::InitializePSO() {

	HRESULT hr = S_FALSE;

	/// DepthBufferStencilResourceの生成
	depthStencilResource_ = CreateDepthStenciltextureResource(kWindowSize.x, kWindowSize.y);
	dsvDescriptorHeap_ = CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1, false);
	// DSVの設定
	dsvDesc_.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; //- Format; 基本的にはResourceに合わせる
	dsvDesc_.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D; //- 2dTexture
	device_->CreateDepthStencilView(
		depthStencilResource_.Get(),
		&dsvDesc_,
		dsvDescriptorHeap_->GetCPUDescriptorHandleForHeapStart()
	);

	/// DepthStencilStateの設定
	depthStencilDesc_.DepthEnable = true; //- Depthの機能を有効化する
	depthStencilDesc_.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL; //- 書き込みします
	depthStencilDesc_.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL; //- 比較関数はLessEqual; 近ければ描画される

	// PSOに代入しDSVのFormatの設定を行う
	graphicsPipelineStateDesc_.DepthStencilState = depthStencilDesc_;
	graphicsPipelineStateDesc_.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;



	graphicsPipelineStateDesc_.pRootSignature = rootSignature_.Get();
	graphicsPipelineStateDesc_.InputLayout = inputlayoutDesc_;

	/// VertexShader
	graphicsPipelineStateDesc_.VS = {
		vertexShaderBlob_->GetBufferPointer(),
		vertexShaderBlob_->GetBufferSize()
	};

	/// PixelShader
	graphicsPipelineStateDesc_.PS = {
		pixelShaderBlob_->GetBufferPointer(),
		pixelShaderBlob_->GetBufferSize()
	};

	graphicsPipelineStateDesc_.BlendState = blendDesc_;
	graphicsPipelineStateDesc_.RasterizerState = rasterizerDesc_;

	/// 書き込むRTVの情報
	graphicsPipelineStateDesc_.NumRenderTargets = 1;
	graphicsPipelineStateDesc_.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

	/// 利用するトロポジ(形状)のタイプ; 三角形
	graphicsPipelineStateDesc_.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	/// どのような画面に色を打ち込むかの設定(気にしなくていい)
	graphicsPipelineStateDesc_.SampleDesc.Count = 1;
	graphicsPipelineStateDesc_.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	/// 実際に生成
	hr = device_->CreateGraphicsPipelineState(
		&graphicsPipelineStateDesc_,
		IID_PPV_ARGS(&graphicsPipelineState_)
	);
	assert(SUCCEEDED(hr));


}



void DirectXCommon::InitializeVertexResource() {

	HRESULT hr = S_FALSE;

	/// 頂点リソース用のヒープの設定
	D3D12_HEAP_PROPERTIES uploadHeapProperties{};
	uploadHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD; // UploadHeapを使う

	/// 頂点リソースのの設定
	D3D12_RESOURCE_DESC vertexResourceDesc{};
	/// バッファリソース; テクスチャの場合はまた別の設定を行う
	vertexResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	vertexResourceDesc.Width = sizeof(VertexData) * 6; // リソースのサイズ; 今回はVector4を3頂点分

	/// バッファの場合はこれらは1にする決まり
	vertexResourceDesc.Height = 1;
	vertexResourceDesc.DepthOrArraySize = 1;
	vertexResourceDesc.MipLevels = 1;
	vertexResourceDesc.SampleDesc.Count = 1;

	/// バッファの場合はこれにする決まり
	vertexResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	hr = device_->CreateCommittedResource(
		&uploadHeapProperties,
		D3D12_HEAP_FLAG_NONE,
		&vertexResourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertexResource_)
	);
	assert(SUCCEEDED(hr));


	/// --------------------------------
	/// ↓ 頂点バッファビューを作成
	/// --------------------------------
	
	/// リソースの先頭アドレスから使う
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();

	/// 使用するリソースのサイズは頂点3つ分のサイズ
	vertexBufferView_.SizeInBytes = sizeof(VertexData) * 6;
	/// 1頂点あたりのサイズ
	vertexBufferView_.StrideInBytes = sizeof(VertexData);

}

void DirectXCommon::InitializeViewport() {

	viewport_.Width = static_cast<float>(kWindowSize.x);
	viewport_.Height = static_cast<float>(kWindowSize.y);

	viewport_.TopLeftX = 0;
	viewport_.TopLeftY = 0;
	viewport_.MinDepth = 0.0f;
	viewport_.MaxDepth = 1.0f;

	/// シザー矩形
	scissorRect_.left = 0;
	scissorRect_.right = static_cast<LONG>(kWindowSize.x);
	scissorRect_.top = 0;
	scissorRect_.bottom = static_cast<LONG>(kWindowSize.y);
}

void DirectXCommon::CreateResource() {

	
	/// ------------------------------
	/// ↓ マテリアルリソースの生成
	/// ------------------------------
	
	materialResource_ = CreateBufferResource(sizeof(Vector4));
	Vector4* materialData = nullptr;
	/// 書き込むためのアドレスを取得
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData));
	/// 書き込み
	*materialData = Vector4(1.0f, 1.0f, 1.0f, 1.0f);


	/// ------------------------------
	/// ↓ wvpリソースの生成
	/// ------------------------------

	wvpResource_ = CreateBufferResource(sizeof(Matrix4x4));
	Matrix4x4* wvpData = nullptr;
	/// 書き込むためのアドレスの取得
	wvpResource_->Map(0, nullptr, reinterpret_cast<void**>(&wvpData));
	/// 書き込み
	Matrix4x4 worldMatrix = Matrix4x4::MakeIdentity();
	*wvpData = worldMatrix;

}


ID3D12Resource* DirectXCommon::CreateBufferResource(size_t sizeInBytes) {
	HRESULT hr = S_FALSE;
	ID3D12Resource* resource = nullptr;

	/// 頂点リソース用のヒープの設定
	D3D12_HEAP_PROPERTIES uploadHeapProperties{};
	uploadHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD; // UploadHeapを使う

	/// 頂点リソースのの設定
	D3D12_RESOURCE_DESC vertexResourceDesc{};
	/// バッファリソース; テクスチャの場合はまた別の設定を行う
	vertexResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	vertexResourceDesc.Width = sizeInBytes; // リソースのサイズ

	/// バッファの場合はこれらは1にする決まり
	vertexResourceDesc.Height = 1;
	vertexResourceDesc.DepthOrArraySize = 1;
	vertexResourceDesc.MipLevels = 1;
	vertexResourceDesc.SampleDesc.Count = 1;

	/// バッファの場合はこれにする決まり
	vertexResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	hr = device_->CreateCommittedResource(
		&uploadHeapProperties,
		D3D12_HEAP_FLAG_NONE,
		&vertexResourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&resource)
	);
	assert(SUCCEEDED(hr));

	return resource;
}

void DirectXCommon::CreateShaderResourceView() {

	/// srvDescriptorHeapの設定
	srvDescriptorHeap_ = CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 128, true);

	/// metaDataを基にSRVの設定
	srvDesc_.Format = metaData_.format;
	srvDesc_.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc_.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;		//- 2Dテクスチャ
	srvDesc_.Texture2D.MipLevels = UINT(metaData_.mipLevels);

	/// SRVを作成するDescriptorHeapの場所を決める
	textureSrvHandleCPU_ = srvDescriptorHeap_->GetCPUDescriptorHandleForHeapStart();
	textureSrvHandleGPU_ = srvDescriptorHeap_->GetGPUDescriptorHandleForHeapStart();

	/// 先頭はImGuiが使っているのでその次を使う
	textureSrvHandleCPU_.ptr += device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	textureSrvHandleGPU_.ptr += device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	/// SRVの生成
	device_->CreateShaderResourceView(textureResource_.Get(), &srvDesc_, textureSrvHandleCPU_);
}

void DirectXCommon::InitializeTextureResource() {

	/// Textureを読んで転送する
	mipImages_ = LoadTexture("./Resources/Images/uvChecker.png");
	metaData_ = mipImages_.GetMetadata();
	textureResource_ = CreateTextureResource(metaData_);

	intermediateResource_ = UploadTextureData(textureResource_.Get(), mipImages_);

}

DirectX::ScratchImage DirectXCommon::LoadTexture(const std::string& filePath) {
	DirectX::ScratchImage image{};
	std::wstring filePathW = Engine::ConvertString(filePath);
	HRESULT hr = DirectX::LoadFromWICFile(filePathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
	assert(SUCCEEDED(hr));

	/// ミップマップの作成
	DirectX::ScratchImage mipImage{};
	hr = DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_SRGB, 0, mipImage);
	assert(SUCCEEDED(hr));

	return mipImage;
}


ID3D12Resource* DirectXCommon::CreateTextureResource(const DirectX::TexMetadata& metaData) {

	/// metaDataを基にResourceの設定
	D3D12_RESOURCE_DESC desc{};
	desc.Width = UINT(metaData.width);								//- Textureの幅
	desc.Height = UINT(metaData.height);							//- Textureの高さ
	desc.MipLevels = UINT16(metaData.mipLevels);					//- mipmapの数
	desc.DepthOrArraySize = UINT16(metaData.arraySize);				//- 奥行き or 配列Textureの配列数
	desc.Format = metaData.format;									//- Textureのformat
	desc.SampleDesc.Count = 1;										//- サンプリングカウント; 1固定
	desc.Dimension = D3D12_RESOURCE_DIMENSION(metaData.dimension);	//- Textureの次元数; 普段使っているのは2次元

	/// 利用するHeapの設定; 非常に特殊な運用; 02_04exで一般的なケース版がある
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_CUSTOM;						//- 細かい設定を行う
	heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;	//- WriteBackポリシー
	heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;			//- プロセッサの近くに配置

	/// Resourceの生成
	ID3D12Resource* resource = nullptr;
	HRESULT hr = DirectXCommon::GetInstance()->GetDevice()->CreateCommittedResource(
		&heapProperties,					//- Heapの設定
		D3D12_HEAP_FLAG_NONE,				//- Heapの特殊な設定; 特になし
		&desc,							//- Resourceの設定
		D3D12_RESOURCE_STATE_COPY_DEST,	//- データ転送される設定
		nullptr,							//- Clear最適解; 使わないのでnullptr
		IID_PPV_ARGS(&resource)			//- 作成するResourceポインタへのポインタ
	);
	assert(SUCCEEDED(hr));

	return resource;
}

void DirectXCommon::InitializeDescriptorRange() {

	descriptorRange_[0].BaseShaderRegister = 0;							//- 0から始まる
	descriptorRange_[0].NumDescriptors = 1;								//- 数は1つ
	descriptorRange_[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;		//- SRVを使う
	descriptorRange_[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;	//- Offsetを自動計算

}


ID3D12Resource* DirectXCommon::CreateDepthStenciltextureResource(int32_t width, int32_t height) {

	/// 生成するResourceの設定
	D3D12_RESOURCE_DESC desc{};
	desc.Width = width;	//- Textureの幅
	desc.Height = height;	//- Textureの高さ
	desc.MipLevels = 1;	//- mipMapの数
	desc.DepthOrArraySize = 1;	//- 奥行き or 配列Textureの配列数
	desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;	//- DepthStencilとして利用可能なフォーマット
	desc.SampleDesc.Count = 1;	//- サンプリングカウント; 1固定
	desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;	//- 2次元
	desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;	//- DepthStencilとして使う通知


	/// 利用するHeapの設定
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;


	/// 震度値のクリア
	D3D12_CLEAR_VALUE depthClearValue{};
	depthClearValue.DepthStencil.Depth = 1.0f; //- 1.0f(最大値)でクリア
	depthClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; //- フォーマット; Resourceと合わせる


	/// Resourceの生成
	ID3D12Resource* resource = nullptr;
	HRESULT hr = device_->CreateCommittedResource(
		&heapProperties,	//- Heapの設定
		D3D12_HEAP_FLAG_NONE,	//- Heapの特殊な設定; 特になし
		&desc,	//-　Resourceの設定
		D3D12_RESOURCE_STATE_DEPTH_WRITE,	//- 深度値を書き込む状態にしておく
		&depthClearValue,	//- Clear最適化
		IID_PPV_ARGS(&resource)	//- 作成するResourceへのポインタ
	);

	/// 生成に失敗したら停止
	assert(SUCCEEDED(hr));

	return resource;
}


/// 属性 [[nodiscard]] 戻り値を破棄しては行けないことを示す; 
[[nodiscard]]
ID3D12Resource* DirectXCommon::UploadTextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImage) {

	std::vector<D3D12_SUBRESOURCE_DATA> subresources;

	/// 読み込んだデータからDirectX12用のSubresourceの配列を作成する
	DirectX::PrepareUpload(device_.Get(), mipImage.GetImages(), mipImage.GetImageCount(), mipImage.GetMetadata(), subresources);
	/// Subresourceの数を基にコピー元となるIntermediateResourceに必要なサイズを計算する
	uint64_t intermediateSize = GetRequiredIntermediateSize(texture, 0, UINT(subresources.size()));
	/// 上記で計算した値を使って作成
	ID3D12Resource* intermediateResource = CreateBufferResource(intermediateSize);

	/// intermediateResourceにSubResourceのデータを書き込む; Textureに転送するコマンドを積む
	UpdateSubresources(commandList_.Get(), texture, intermediateResource, 0, 0, UINT(subresources.size()), subresources.data());

	//- CPUとGPUを取り持つためのResourceなので IntermediateResource(中間リソース)と呼ぶ


	/// Textureへの転送後は利用できるよう,D3D12_RESOURCE_STATE_COPY_DESCからD3D12_RESOURCE_STATE_GENERIC_READへResourceStateを変更する
	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = texture;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;
	commandList_->ResourceBarrier(1, &barrier);

	return intermediateResource;
}

ID3D12DescriptorHeap* DirectXCommon::CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible) {

	ID3D12DescriptorHeap* descriptorHeap = nullptr;
	D3D12_DESCRIPTOR_HEAP_DESC desc{};
	desc.Type = heapType;
	desc.NumDescriptors = numDescriptors;
	desc.Flags = shaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	HRESULT hr = device_->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&descriptorHeap));
	assert(SUCCEEDED(hr));
	return descriptorHeap;

}

void DirectXCommon::SetRenderTargets(const D3D12_CPU_DESCRIPTOR_HANDLE& dsvHandle) {
	UINT bbIndex = swapChain_->GetCurrentBackBufferIndex();

	commandList_->OMSetRenderTargets(1, &rtvHandles_[bbIndex], false, &dsvHandle);
}
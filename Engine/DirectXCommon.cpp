#include <DirectXCommon.h>

#include <Engine.h>
#include <Vector4.h>

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



	/// <summary>
	/// DXCの初期化
	/// </summary>
	InitializeDXC();

	/// <summary>
	/// ルートシグネチャの生成
	/// </summary>
	CreateRootSignature();

	/// <summary>
	/// インプットレイアウトの設定
	/// </summary>
	SetingInputLayout();

	/// <summary>
	/// ブレンドステートの設定
	/// </summary>
	SetingBlendState();

	/// <summary>
	/// ラスタライザーステートの設定
	/// </summary>
	SetingRasterizerState();

	/// <summary>
	/// 
	/// </summary>
	SetingShader();

	/// <summary>
	/// 
	/// </summary>
	CreatePSO();

	/// <summary>
	/// VertexResourceの生成
	/// </summary>
	CreateVertexResource();

	/// <summary>
	/// VertexBufferViewの生成
	/// </summary>
	CreateVBV();

	WriteVertexData();

	InitializeViewport();

}

void DirectXCommon::Finalize() {


	//// オブジェクトの解放 -----
	fence_->Release();
	rtvDescriptorHeap_->Release();
	swapChainResource_[0]->Release();
	swapChainResource_[1]->Release();
	swapChain_->Release();
	commandList_->Release();
	commandAllocator_->Release();
	commandQueue_->Release();
	device_->Release();
	dxgiFactory_->Release();

	vertexResource_->Release();
	graphicsPipelineState_->Release();
	rootSignature_->Release();
	pixelShaderBlob_->Release();
	vertexShaderBlob_->Release();


	//// 解放されていないものがあれば止まる -----
	IDXGIDebug1* debug;
	if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug)))) {
		debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
		debug->ReportLiveObjects(DXGI_DEBUG_APP, DXGI_DEBUG_RLO_ALL);
		debug->ReportLiveObjects(DXGI_DEBUG_D3D12, DXGI_DEBUG_RLO_ALL);
		debug->Release();
	}

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
	barrier.Transition.pResource = swapChainResource_[bbIndex];
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
	barrier.Transition.pResource = swapChainResource_[bbIndex];
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
	ID3D12CommandList* commandLists[] = { commandList_ };
	commandQueue_->ExecuteCommandLists(1, commandLists);

	/// GPUとOSに画面の交換を行うよう通知する
	swapChain_->Present(1, 0);


	fenceValue_++;
	commandQueue_->Signal(fence_, fenceValue_);
	if (fence_->GetCompletedValue() < fenceValue_) {
		HANDLE fenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
		assert(fenceEvent != nullptr);
		fence_->SetEventOnCompletion(fenceValue_, fenceEvent);
		WaitForSingleObject(fenceEvent, INFINITE);
	}

	/// 次のフレームのコマンドリストを準備
	hr = commandAllocator_->Reset();
	assert(SUCCEEDED(hr));
	hr = commandList_->Reset(commandAllocator_, nullptr);
	assert(SUCCEEDED(hr));

}

void DirectXCommon::ClearRenderTarget() {

	/// これから書き込むバックバッファのインデックスを所得(二枚しかないので 0 か 1)
	UINT bbIndex = swapChain_->GetCurrentBackBufferIndex();

	/// 描画先のRTVを設定する
	commandList_->OMSetRenderTargets(1, &rtvHandles_[bbIndex], false, nullptr);

	/// 指定した色で画面全体をクリアする
	float clearColor[] = { 0.1f, 0.25f, 0.5f, 1.0f }; /// 青っぽい色, RGBAの順番
	commandList_->ClearRenderTargetView(rtvHandles_[bbIndex], clearColor, 0, nullptr);

}

IDxcBlob* DirectXCommon::CompileShader(const std::wstring& filePath, const wchar_t* profile) {

	HRESULT hr = S_FALSE;

	Engine::ConsolePrint(std::format(L"Begin Compile, path:{}, profile:{}\n", filePath, profile));

	/// hlslファイルを読み込む
	IDxcBlobEncoding* shaderSource = nullptr;
	hr = dxcUtils_->LoadFile(filePath.c_str(), nullptr, &shaderSource);
	assert(SUCCEEDED(hr));

	/// ファイルの内容を設定する
	DxcBuffer shaderSourceBuffer;
	shaderSourceBuffer.Ptr = shaderSource->GetBufferPointer();
	shaderSourceBuffer.Size = shaderSource->GetBufferSize();
	shaderSourceBuffer.Encoding = DXC_CP_UTF8; // UTF8の文字コードであることを通知

	LPCWSTR arguments[] = {
		filePath.c_str(),			/// コンパイル対象のhlslファイル名
		L"-E", L"main",				/// エントリーポイントの指定。基本的にmain以外指定しない
		L"-T", profile,				/// ShaderProfileの設定
		L"-Zi", L"-Qembeb_debug",	/// デバッグ用の情報を埋め込む
		L"-Od",						/// 最適化を外しておく	
		L"-Zpr",					/// メモリのレイアウトは行優先
	};

	/// 実際にShaderをコンパイルする
	IDxcResult* shaderResult = nullptr;
	hr = dxcCompiler_->Compile(
		&shaderSourceBuffer,
		arguments,
		_countof(arguments),
		includeHandler_,
		IID_PPV_ARGS(&shaderResult)
	);
	assert(SUCCEEDED(hr));


	/// 警告・エラーが出ていたらログに出して止める
	IDxcBlobUtf8* shaderError = nullptr;
	shaderResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&shaderError), nullptr);
	if (shaderError != nullptr && shaderError->GetStringLength() != 0) {
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


	return shaderBlob;
}

void DirectXCommon::TestDraw() {

	commandList_->RSSetViewports(1, &viewport_);
	commandList_->RSSetScissorRects(1, &scissorRect_);

	/// RootSignatureを設定; PS0に設定しているけど別途設定が必要
	commandList_->SetGraphicsRootSignature(rootSignature_);
	commandList_->SetPipelineState(graphicsPipelineState_);
	commandList_->IASetVertexBuffers(0, 1, &vertexBufferView_);

	/// 形状を設定; PS0に設定している物とはまた別; 同じものを設定すると考えておけばいい
	commandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	/// 描画! (DrawCall/ドローコール); 3頂点で1つのインスタンス; インスタンスについては今後
	commandList_->DrawInstanced(3, 1, 0, 0);

}


void DirectXCommon::InitializeDXGIDevice() {
	HRESULT hr = S_FALSE;


#ifdef _DEBUG

	ID3D12Debug1* debugController = nullptr;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
		/// デバッグレイヤーを有効化する
		debugController->EnableDebugLayer();
		/// さらにGPU側でもチェックできるようにする
		debugController->SetEnableGPUBasedValidation(TRUE);
	}

#endif // _DEBUG

	/// DXGIファクトリーの生成
	hr = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory_));
	assert(SUCCEEDED(hr));


	/// 使用するアダプタ用変数
	IDXGIAdapter4* useAdapter = nullptr;

	/// いい順にアダプタを頼む
	for (UINT i = 0; dxgiFactory_->EnumAdapterByGpuPreference(i,
		DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&useAdapter)) != DXGI_ERROR_NOT_FOUND; i++) {

		/// アダプタの情報を取得する
		DXGI_ADAPTER_DESC3 adapterDesc{};
		hr = useAdapter->GetDesc3(&adapterDesc);
		assert(SUCCEEDED(hr)); /// 所得出来ないのは一大事

		/// ソフトウェアアダプタでなければ採用
		if (!(adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE)) {
			/// 採用したアダプタの情報をログに出力
			Engine::ConsolePrint(std::format(L"Use Adapter:{}\n", adapterDesc.Description));
			break;
		}

		/// ソフトウェアアダプタの場合は無視
		useAdapter = nullptr;

	}

	/// 適切なアダプタがなければ起動できない
	assert(useAdapter != nullptr);



	D3D_FEATURE_LEVEL featureLevels[] = {
		D3D_FEATURE_LEVEL_12_2,
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0
	};

	const char* featureLovelString[] = { "12.2", "12.1", "12.0" };

	/// 高い順に生成できるか試す
	for (size_t i = 0; i < _countof(featureLevels); i++) {

		/// 採用したアダプターでデバイスを生成
		hr = D3D12CreateDevice(useAdapter, featureLevels[i], IID_PPV_ARGS(&device_));

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

		/// 解放
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
		commandAllocator_,
		nullptr,
		IID_PPV_ARGS(&commandList_)
	);
	/// 生成に失敗したら起動できない
	assert(SUCCEEDED(hr));


}

void DirectXCommon::CreateSwapChain() {
	HRESULT hr = S_FALSE;

	/// スワップチェーンを生成する
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
	//// 画面の幅と高さ; windowのクライアント領域と同じものにしておく;
	swapChainDesc.Width = backBufferWidth_;
	swapChainDesc.Height = backBufferHeight_;
	/// 色の形式
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	/// マルチサンプルしない
	swapChainDesc.SampleDesc.Count = 1;
	/// 描画のターゲットとして利用する
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	/// ダブルバッファ
	swapChainDesc.BufferCount = 2;
	/// モニタに移したら中身を破棄
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;


	//// コマンドキュー、ウィンドウハンドル、設定を渡して生成する
	hr = dxgiFactory_->CreateSwapChainForHwnd(
		commandQueue_,
		p_winApp_->GetHWND(),
		&swapChainDesc,
		nullptr,
		nullptr,
		reinterpret_cast<IDXGISwapChain1**>(&swapChain_)
	);

	/// 失敗したら起動できない
	assert(SUCCEEDED(hr));



}

void DirectXCommon::CreateFinalRenderTargets() {
	HRESULT hr = S_FALSE;

	/*DXGI_SWAP_CHAIN_DESC swcDesc = {};
	hr = swapChain_->GetDesc(&swcDesc);
	assert(SUCCEEDED(hr));*/

	///// ディスクリプタヒープの生成 /////
	D3D12_DESCRIPTOR_HEAP_DESC rtvDescriptorHeapDesc{};
	/// レンダーターゲットビュー用
	rtvDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	/// ダブルバッファ用に2つ; 多くても構わない
	rtvDescriptorHeapDesc.NumDescriptors = 2;
	hr = device_->CreateDescriptorHeap(&rtvDescriptorHeapDesc, IID_PPV_ARGS(&rtvDescriptorHeap_));
	/// 生成できなかったら起動できない
	assert(SUCCEEDED(hr));


	/// SwapChainからResourceを引っ張ってくる
	//for (int i = 0; i < 2; i++) {
	//	hr = swapChain_->GetBuffer(i, IID_PPV_ARGS(&swapChainResource_[i]));
	//	assert(SUCCEEDED(hr));
	//}

	swapChainResource_[0] = nullptr;
	swapChainResource_[1] = nullptr;

	hr = swapChain_->GetBuffer(0, IID_PPV_ARGS(&swapChainResource_[0]));
	assert(SUCCEEDED(hr));
	hr = swapChain_->GetBuffer(1, IID_PPV_ARGS(&swapChainResource_[1]));
	assert(SUCCEEDED(hr));


	/// RTVの設定
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

	/// ディスクリプタの先頭を所得する
	D3D12_CPU_DESCRIPTOR_HANDLE rtvStartHandle = rtvDescriptorHeap_->GetCPUDescriptorHandleForHeapStart();


	/// まず1つ目を作る;1つ目は最初のところに作る;作る場所をこちらで指定してあげる必要がある
	rtvHandles_[0] = rtvStartHandle;
	device_->CreateRenderTargetView(swapChainResource_[0], &rtvDesc, rtvHandles_[0]);
	/// 2つ目のディスクリプタハンドルを得る
	rtvHandles_[1].ptr = rtvHandles_[0].ptr + device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	/// 2つ目を作る
	device_->CreateRenderTargetView(swapChainResource_[1], &rtvDesc, rtvHandles_[1]);


}

void DirectXCommon::CreateFence() {

	HRESULT hr = S_FALSE;

	hr = device_->CreateFence(fenceValue_, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence_));
	assert(SUCCEEDED(hr));

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

	/// シリアライズしてバイナリする
	ID3DBlob* signatureBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;
	hr = D3D12SerializeRootSignature(
		&descriptionRootSigneture,
		D3D_ROOT_SIGNATURE_VERSION_1,
		&signatureBlob,
		&errorBlob
	);

	if (FAILED(hr)) {
		Engine::ConsolePrint(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(false);
	}

	/// バイナリを元に生成
	hr = device_->CreateRootSignature(
		0,
		signatureBlob->GetBufferPointer(),
		signatureBlob->GetBufferSize(),
		IID_PPV_ARGS(&rootSignature_)
	);
	assert(SUCCEEDED(hr));

}

void DirectXCommon::SetingInputLayout() {

	/// InputLayout
	inputElemntDescs_[0].SemanticName = "POSITION";
	inputElemntDescs_[0].SemanticIndex = 0;
	inputElemntDescs_[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElemntDescs_[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;


	inputlayoutDesc_.pInputElementDescs = inputElemntDescs_;
	inputlayoutDesc_.NumElements = _countof(inputElemntDescs_);

}

void DirectXCommon::SetingBlendState() {

	/// BlendStateの設定
	/// すべての色要素を書き込む
	blendDesc_.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

}

void DirectXCommon::SetingRasterizerState() {

	/// RasterizerStateの設定
	/// 裏面(時計回り)を表示しない
	rasterizerDesc_.CullMode = D3D12_CULL_MODE_BACK;
	/// 三角形の中を塗りつぶす
	rasterizerDesc_.FillMode = D3D12_FILL_MODE_SOLID;

}

void DirectXCommon::SetingShader() {

	
	vertexShaderBlob_ = CompileShader(Engine::ConvertString("./Engine/Object3d.VS.hlsl"), L"vs_6_0");
	assert(vertexShaderBlob_ != nullptr);

	pixelShaderBlob_ = CompileShader(Engine::ConvertString("./Engine/Object3d.PS.hlsl"), L"ps_6_0");
	assert(pixelShaderBlob_ != nullptr);

}

void DirectXCommon::CreatePSO() {

	HRESULT hr = S_FALSE;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	graphicsPipelineStateDesc.pRootSignature = rootSignature_;
	graphicsPipelineStateDesc.InputLayout = inputlayoutDesc_;

	/// VertexShader
	graphicsPipelineStateDesc.VS = {
		vertexShaderBlob_->GetBufferPointer(),
		vertexShaderBlob_->GetBufferSize()
	};

	/// PixelShader
	graphicsPipelineStateDesc.PS = {
		pixelShaderBlob_->GetBufferPointer(),
		pixelShaderBlob_->GetBufferSize()
	};

	graphicsPipelineStateDesc.BlendState = blendDesc_;
	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc_;

	/// 書き込むRTVの情報
	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

	/// 利用するトロポジ(形状)のタイプ; 三角形
	graphicsPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	/// どのような画面に色を打ち込むかの設定(気にしなくていい)
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	/// 実際に生成
	hr = device_->CreateGraphicsPipelineState(
		&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&graphicsPipelineState_)
	);
	assert(SUCCEEDED(hr));



}


void DirectXCommon::CreateVertexResource() {

	HRESULT hr = S_FALSE;

	/// 頂点リソース用のヒープの設定
	D3D12_HEAP_PROPERTIES uploadHeapProperties{};
	uploadHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD; // UploadHeapを使う

	/// 頂点リソースのの設定
	D3D12_RESOURCE_DESC vertexResourceDesc{};
	/// バッファリソース; テクスチャの場合はまた別の設定を行う
	vertexResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	vertexResourceDesc.Width = sizeof(Vector4) * 3; // リソースのサイズ; 今回はVector4を3頂点分

	/// バッファの場合はこれらは1にする決まり
	vertexResourceDesc.Height = 1;
	vertexResourceDesc.DepthOrArraySize = 1;
	vertexResourceDesc.MipLevels = 1;
	vertexResourceDesc.SampleDesc.Count = 1;

	/// バッファの場合はこれにする決まり
	hr = device_->CreateCommittedResource(
		&uploadHeapProperties,
		D3D12_HEAP_FLAG_NONE,
		&vertexResourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertexResource_)
	);
	assert(SUCCEEDED(hr));

}

void DirectXCommon::CreateVBV() {

	/// 頂点バッファビューを作成

	/// リソースの先頭アドレスから使う
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();

	/// 使用するリソースのサイズは頂点3つ分のサイズ
	vertexBufferView_.SizeInBytes = sizeof(Vector4) * 3;
	/// 1頂点あたりのサイズ
	vertexBufferView_.StrideInBytes = sizeof(Vector4);

}

void DirectXCommon::WriteVertexData() {

	Vector4* vertexData = nullptr;
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));

	// 左下
	vertexData[0] = { -0.5f,-0.5f,0.0f,1.0f };
	// 上
	vertexData[1] = { 0.0f,0.5f,0.0f,1.0f };
	// 右下
	vertexData[1] = { 0.5f,-0.5f,0.0f,1.0f };

}

void DirectXCommon::InitializeViewport() {

	/// viewport
	viewport_.Width = static_cast<float>(backBufferWidth_);
	viewport_.Height = static_cast<float>(backBufferHeight_);

	viewport_.TopLeftX = 0;
	viewport_.TopLeftY = 0;
	viewport_.MinDepth = 0.0f;
	viewport_.MaxDepth = 1.0f;

	/// シザー矩形
	scissorRect_.left = 0;
	scissorRect_.right = static_cast<LONG>(backBufferWidth_);
	scissorRect_.top = 0;
	scissorRect_.bottom = static_cast<LONG>(backBufferHeight_);

}

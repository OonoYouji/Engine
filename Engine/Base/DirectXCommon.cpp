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
#include <ShaderBlob.h>
#include <PipelineStateObject.h>
#include <PipelineStateObjectManager.h>
#include <Vector4.h>
#include <RenderTexture.h>

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

	PipelineStateObjectManager* psoManager = PipelineStateObjectManager::GetInstance();
	psoManager->Initialize(device_.Get());


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

	CloseHandle(fenceEvent_);
	fence_.Reset();
	renderTextures_.clear();
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
	
	renderTextures_.resize(2);
	for(uint32_t index = 0; index < 2; index++) {
		renderTextures_[index].reset(new RenderTexture());
		renderTextures_[index]->Initialize(UINT(index));
	}

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
ComPtr<ID3D12Resource> DirectXCommon::CreateBufferResource(size_t sizeInBytes, D3D12_HEAP_TYPE heapType) {
	HRESULT result = S_FALSE;
	ComPtr<ID3D12Resource> resource = nullptr;

	///- 頂点リソース用のヒープ設定
	D3D12_HEAP_PROPERTIES uploadHeapProperties{};
	uploadHeapProperties.Type = heapType;

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


void DirectXCommon::CommnadExecuteAndWait() {
	command_->Close(); //- commandのcloseと実行
	WaitExecution();
	command_->ResetCommandList();
}

void DirectXCommon::SetRenderTarget() {
	UINT bbIndex = swapChain_->GetCurrentBackBufferIndex();
	renderTextures_[bbIndex]->SetRenderTarget();
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
	renderTextures_[bbIndex]->CreateBarrier(D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
	renderTextures_[bbIndex]->SetRenderTarget();
	renderTextures_[bbIndex]->Clear({ 0.1f,0.25f,0.5f,1.0f });

	///- viewport; scissorRectを設定
	commandList->RSSetViewports(1, &viewport_);
	commandList->RSSetScissorRects(1, &scissorRect_);
}



/// ---------------------------
/// ↓ 描画後処理
/// ---------------------------
void DirectXCommon::PostDraw() {
	UINT bbIndex = swapChain_->GetCurrentBackBufferIndex();

	///- 描画用にバリアーを貼る
	renderTextures_[bbIndex]->CreateBarrier(D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);

	///- コマンドのクローズとキック
	command_->Close();

	///- GPUとOSに画面の交換を行うように通知する
	swapChain_->Present(1, 0);

	WaitExecution();

	///- 次のフレームのためにQueueをリセットする
	command_->ResetCommandList();
	ClearDepthBuffer();

}

void DirectXCommon::WaitExecution() {
	///- GPUにSignalを送信
	fenceValue_++;
	command_->GetQueue()->Signal(fence_.Get(), fenceValue_);

	///- GPUの処理が終わっていなければイベントを待機する
	if(fence_->GetCompletedValue() < fenceValue_) {
		fence_->SetEventOnCompletion(fenceValue_, fenceEvent_);
		WaitForSingleObject(fenceEvent_, INFINITE);
	}
}



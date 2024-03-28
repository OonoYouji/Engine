#include <DirectXCommon.h>

#include <WinApp.h>
#include <Engine.h>
#include <Environment.h>

#include <format>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")


using namespace Microsoft::WRL;

/// ---------------------------
/// ↓ 初期化を行う
/// ---------------------------
void DirectXCommon::Initialize(WinApp* winApp) {

	p_winApp_ = winApp;

	InitializeDXGIDevice();
	InitializeCommand();
	InitializeSwapChain();
	InitialiezRenderTarget();


}



/// ---------------------------
/// ↓ メンバ変数の解放などを行う
/// ---------------------------
void DirectXCommon::Finalize() {}



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

	DXGI_SWAP_CHAIN_DESC1 desc{};
	desc.Width = kWindowSize.x;
	desc.Height = kWindowSize.y;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	//- 描画のターゲットとして利用
	desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	//- ダブルバッファを使用
	desc.BufferCount = 2;
	//- モニターに移したら中身を破棄
	desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

	///- SwapChain1で仮に生成
	ComPtr<IDXGISwapChain1> swapChain1;
	result = dxgiFactory_->CreateSwapChainForHwnd(
		commandQueue_.Get(), p_winApp_->GetHWND(), &desc, nullptr, nullptr, &swapChain1
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
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc{};
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.NumDescriptors = 2; //- ダブルバッファなので2つ

	///- 生成; 生成できたか確認
	result = device_->CreateDescriptorHeap(
		&rtvHeapDesc, IID_PPV_ARGS(&rtvDescriptorHeap_)
	);
	assert(SUCCEEDED(result));



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

	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

	///- ディスクリプタの先頭を取得
	D3D12_CPU_DESCRIPTOR_HANDLE rtvStartHandl = rtvDescriptorHeap_->GetCPUDescriptorHandleForHeapStart();

	///- rtvHandleの生成
	rtvHandles_[0] = rtvStartHandl;
	device_->CreateRenderTargetView(swapChainResource_[0].Get(), &rtvDesc, rtvHandles_[0]);

	rtvHandles_[1].ptr = rtvHandles_[0].ptr + device_->GetDescriptorHandleIncrementSize(
		D3D12_DESCRIPTOR_HEAP_TYPE_RTV
	);
	device_->CreateRenderTargetView(swapChainResource_[1].Get(), &rtvDesc, rtvHandles_[1]);

}



/// ---------------------------
/// ↓ 描画前処理
/// ---------------------------
void DirectXCommon::PreDraw() {}



/// ---------------------------
/// ↓ 描画後処理
/// ---------------------------
void DirectXCommon::PostDraw() {
	HRESULT result = S_FALSE;
	UINT bbIndex = swapChain_->GetCurrentBackBufferIndex();

	commandList_->OMSetRenderTargets(
		1, &rtvHandles_[bbIndex], false, nullptr
	);

	///- 指定した色で画面をクリア
	float clearColor[] = { 0.1f,0.25f,0.5f,1.0f };
	commandList_->ClearRenderTargetView(
		rtvHandles_[bbIndex], clearColor, 0, nullptr
	);

	///- Commandを閉じる
	result = commandList_->Close();
	assert(SUCCEEDED(result));


	///- コマンドをキックする
	ID3D12CommandList* commandLists[] = { commandList_.Get() };
	commandQueue_->ExecuteCommandLists(1, commandLists);

	///- GPUとOSに画面の交換を行うように通知する
	swapChain_->Present(1, 0);

	///- 次のフレーム用にコマンドリストを準備
	result = commandAllocator_->Reset();
	assert(SUCCEEDED(result));
	result = commandList_->Reset(commandAllocator_.Get(), nullptr);
	assert(SUCCEEDED(result));

}



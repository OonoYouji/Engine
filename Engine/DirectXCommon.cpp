#include <DirectXCommon.h>

#include <Engine.h>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")


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

}

DirectXCommon* DirectXCommon::GetInstance() {
	static DirectXCommon instance;
	return &instance;
}

void DirectXCommon::PreDraw() {




	ClearRenderTarget();


}

void DirectXCommon::PostDraw() {
	HRESULT hr = S_FALSE;


	/// コマンドリストの内容を確定させる(すべてのコマンドを積んでからCloseする)
	hr = commandList_->Close();
	assert(SUCCEEDED(hr));

	/// コマンドリストの実行
	ID3D12CommandList* commandLists[] = { commandList_ };
	commandQueue_->ExecuteCommandLists(1, commandLists);

	/// GPUとOSに画面の交換を行うよう通知する
	swapChain_->Present(1, 0);

	
	
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
	commandList_->OMSetRenderTargets(1, &rtvHandle_[bbIndex], false, nullptr);

	/// 指定した色で画面全体をクリアする
	float clearColor[] = { 0.1f, 0.25f, 0.5f, 1.0f }; /// 青っぽい色, RGBAの順番
	commandList_->ClearRenderTargetView(rtvHandle_[bbIndex], clearColor, 0, nullptr);

}

void DirectXCommon::InitializeDXGIDevice() {
	HRESULT hr = S_FALSE;


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
	assert(useAdapter);



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
	assert(device_);

	Engine::ConsolePrint("Complete create D3D12Device!!!!!\n");

}

void DirectXCommon::InitializeCommand() {
	HRESULT hr = S_FALSE;

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

	/// コマンドキューを生成する ------------------------
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
	hr = device_->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&commandQueue_));
	/// コマンドキューの生成が上手くいかないと起動できない
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
	for (int i = 0; i < 2; i++) {
		hr = swapChain_->GetBuffer(i, IID_PPV_ARGS(&swapChainResource_[i]));
		assert(SUCCEEDED(hr));
	}


	/// RTVの設定
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

	/// ディスクリプタの先頭を所得する
	D3D12_CPU_DESCRIPTOR_HANDLE rtvStartHandle = rtvDescriptorHeap_->GetCPUDescriptorHandleForHeapStart();

	/// RTVを二つ作るのでディスクリプタを二つ用意
	rtvHandle_[2];

	/// まず1つ目を作る;1つ目は最初のところに作る;作る場所をこちらで指定してあげる必要がある
	rtvHandle_[0] = rtvStartHandle;
	device_->CreateRenderTargetView(swapChainResource_[0], &rtvDesc, rtvHandle_[0]);
	/// 2つ目のディスクリプタハンドルを得る
	rtvHandle_[1].ptr = rtvHandle_[0].ptr + device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	/// 2つ目を作る
	device_->CreateRenderTargetView(swapChainResource_[1], &rtvDesc, rtvHandle_[1]);


}

#pragma once

#include <d3d12.h>
#include <dxgi1_6.h>
#include <dxgidebug.h>
#include <cassert>
#include <vector>
#include <chrono>
#include <wrl.h>
#include <cstdlib>
#include <dxcapi.h>

#include <WinApp.h>
#include <Vector4.h>
#include <Vector3.h>

class DirectXCommon final {
private:

	DirectXCommon() = default;
	~DirectXCommon() = default;

private:

	/// WinAppクラスへのポインタ;
	WinApp* p_winApp_;

	/// デバイス
	IDXGIFactory7* dxgiFactory_ = nullptr;
	ID3D12Device* device_ = nullptr;
	IDXGIAdapter4* useAdapter_ = nullptr;

	/// コマンド関連
	ID3D12CommandAllocator* commandAllocator_ = nullptr;
	ID3D12GraphicsCommandList* commandList_ = nullptr;
	ID3D12CommandQueue* commandQueue_ = nullptr;

	/// スワップチェーン
	IDXGISwapChain4* swapChain_ = nullptr;
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc_;

	/// RTV
	ID3D12DescriptorHeap* rtvDescriptorHeap_ = nullptr;
	ID3D12Resource* swapChainResource_[2] = { nullptr };
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles_[2];
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc_;

	/// Fence
	ID3D12Fence* fence_ = nullptr;
	uint64_t fenceValue_ = 0;
	HANDLE fenceEvent_;

	/// debug
	ID3D12Debug1* debugController_ = nullptr;

	/// windowのサイズ
	int32_t backBufferWidth_ = 0;
	int32_t backBufferHeight_ = 0;
	

public:

	/// <summary>
	/// このクラスの初期化
	/// </summary>
	void Initialize(WinApp* winApp,
		int32_t backBufferWidth = WinApp::kWindowWidth_,
		int32_t backBufferHeight = WinApp::kWindowHeigth_);


	void Finalize();

	/// <summary>
	///	DirectXCommonクラスへのポインタ
	/// </summary>
	static DirectXCommon* GetInstance();

	/// <summary>
	/// 描画前処理
	/// </summary>
	void PreDraw();

	/// <summary>
	/// 描画後処理
	/// </summary>
	void PostDraw();

	/// <summary>
	/// レンダーターゲットのクリア
	/// </summary>
	void ClearRenderTarget();


	ID3D12DescriptorHeap* CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible);

	void SetRenderTargets(const D3D12_CPU_DESCRIPTOR_HANDLE& dsvHandle);


	ID3D12Device* GetDevice() { return device_; }
	ID3D12GraphicsCommandList* GetCommandList() { return commandList_; }

	const D3D12_RENDER_TARGET_VIEW_DESC& GetRTVDesc() const { return rtvDesc_; }
	const DXGI_SWAP_CHAIN_DESC1& GetSwapChainDesc() const { return swapChainDesc_; }

private:

	/// <summary>
	/// デバイスの初期化
	/// </summary>
	void InitializeDXGIDevice();

	/// <summary>
	/// コマンド関連の初期化
	/// </summary>
	void InitializeCommand();

	/// <summary>
	/// スワップチェーンの生成
	/// </summary>
	void CreateSwapChain();

	/// <summary>
	/// レンダーターゲット生成
	/// </summary>
	void CreateFinalRenderTargets();

	/// <summary>
	/// フェンスの生成
	/// </summary>
	void CreateFence();


private:

	/// 代入演算子、コピーコンストラクタの禁止
	DirectXCommon(const DirectXCommon& other) = delete;
	DirectXCommon(DirectXCommon&& other) = delete;
	DirectXCommon& operator= (const DirectXCommon& other) = delete;

};
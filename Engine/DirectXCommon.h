#pragma once

#include <d3d12.h>
#include <dxgi1_6.h>
#include <cassert>
#include <vector>

#include <WinApp.h>

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

	/// コマンド関連
	ID3D12CommandAllocator* commandAllocator_ = nullptr;
	ID3D12GraphicsCommandList* commandList_ = nullptr;
	ID3D12CommandQueue* commandQueue_ = nullptr;

	/// スワップチェーン
	IDXGISwapChain4* swapChain_ = nullptr;

	///
	ID3D12DescriptorHeap* rtvDescriptorHeap_ = nullptr;
	ID3D12Resource* swapChainResource_[2] = { nullptr };

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

	/// <summary>
	///	DirectXCommonクラスへのポインタ
	/// </summary>
	static DirectXCommon* GetInstance();

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

private:

	/// 代入演算子、コピーコンストラクタの禁止
	DirectXCommon(const DirectXCommon& other) = delete;
	DirectXCommon(DirectXCommon&& other) = delete;
	DirectXCommon& operator= (const DirectXCommon& other) = delete;

};
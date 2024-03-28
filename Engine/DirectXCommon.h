#pragma once

#include <d3d12.h>
#include <dxgi1_6.h>
#include <cassert>

/// ComPtr用
#include <wrl/client.h>

using namespace Microsoft::WRL;

class WinApp;


/// -------------------------
/// DirextX12の汎用クラス
/// -------------------------
class DirectXCommon final {
private:

	/// -----------------------------------
	/// ↓ メンバ変数
	/// -----------------------------------

	/// WinAppクラスへのポインタ;
	WinApp* p_winApp_;

	///- DirectX12 初期化
	ComPtr<IDXGIFactory7> dxgiFactory_;
	ComPtr<IDXGIAdapter4> useAdapter_;
	ComPtr<ID3D12Device> device_;

	///- 画面の色を変えよう
	ComPtr<ID3D12CommandQueue> commandQueue_;
	ComPtr<ID3D12CommandAllocator> commandAllocator_;
	ComPtr<ID3D12GraphicsCommandList> commandList_;
	
	ComPtr<IDXGISwapChain4> swapChain_;
	
	ComPtr<ID3D12DescriptorHeap> rtvDescriptorHeap_;
	ComPtr<ID3D12Resource> swapChainResource_[2];
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles_[2];

	///- エラー放置ダメ、ゼッタイ
	ComPtr<ID3D12Debug1> debugController_;

private:

	/// -----------------------------------
	/// ↓ privateなメンバ関数
	/// -----------------------------------

	void InitializeDXGIDevice();
	
	void InitializeCommand();

	void InitializeSwapChain();

	void InitialiezRenderTarget();


public:

	/// -----------------------------------
	/// ↓ publicなメンバ関数
	/// -----------------------------------


	/// <summary>
	/// このクラスの初期化
	/// </summary>
	void Initialize(WinApp* winApp);

	/// <summary>
	/// 終了処理
	/// </summary>
	void Finalize();

	/// <summary>
	///	DirectXCommonクラスへのポインタ
	/// </summary>
	static DirectXCommon* GetInstance();


	void PreDraw();

	void PostDraw();


private:

	DirectXCommon() = default;
	~DirectXCommon() = default;

	/// 代入演算子、コピーコンストラクタの禁止
	DirectXCommon(const DirectXCommon& other) = delete;
	DirectXCommon(DirectXCommon&& other) = delete;
	DirectXCommon& operator= (const DirectXCommon& other) = delete;

};
#pragma once

#include <d3d12.h>
#include <dxgi1_6.h>
#include <dxgidebug.h>
#include <dxcapi.h>
#include <DirectXTex.h>
#include <wrl/client.h>

#include <string>
#include <cassert>
#include <cmath>	
#include <memory>

#include <WorldTransform.h>
#include <Camera.h>
#include "Vector2.h"
#include "Vector4.h"

using namespace Microsoft::WRL;

class WinApp;
class DxCommand;
class DxDescriptors;
class ShaderCompile;
class ShaderBlob;
class PipelineStateObject;
class PipelineStateObjectManager;


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

	///- DirectX Command
	DxCommand* command_;
	///- Descriptors
	DxDescriptors* descriptors_;

	ComPtr<IDXGISwapChain4> swapChain_;
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc_;

	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc_;
	std::vector<ComPtr<ID3D12Resource>>swapChainResource_;
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles_[2];

	///- エラー放置ダメ、ゼッタイ
	ComPtr<ID3D12Debug1> debugController_;

	///- 完璧な画面クリアを目指す
	ComPtr<ID3D12Fence> fence_;
	uint64_t fenceValue_;
	HANDLE fenceEvent_;



	D3D12_VIEWPORT viewport_;
	Matrix4x4 viewportMatrix_;
	D3D12_RECT scissorRect_;

	///- 前後関係
	ComPtr<ID3D12Resource> depthStencilResource_;

private:

	/// -----------------------------------
	/// ↓ privateなメンバ関数
	/// -----------------------------------

	void InitializeDXGIDevice();

	void InitializeSwapChain();

	void InitialiezRenderTarget();

	void InitializeFence();

	void InitializeViewport();

	void ClearRenderTarget();

	ComPtr<ID3D12Resource> CreateDepthStencilTextureResource(int32_t width, int32_t height);

	void InitializeDepthStencil();

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

	/// <summary>
	/// 描画前処理
	/// </summary>
	void PreDraw();

	/// <summary>
	/// 描画後処理
	/// </summary>
	void PostDraw();

	void WaitExecution();

	ID3D12Device* GetDevice() { return device_.Get(); }

	const DXGI_SWAP_CHAIN_DESC1& GetSwapChainDesc() const { return swapChainDesc_; }

	const D3D12_RENDER_TARGET_VIEW_DESC& GetRTVDesc() const { return rtvDesc_; }

	ComPtr<ID3D12Resource> CreateBufferResource(size_t sizeInBytes);
	
	const Matrix4x4& GetViewportMatrix() const { return viewportMatrix_; }

	std::vector<ComPtr<ID3D12Resource>> GetSwapChainResource() { return swapChainResource_; }

	void ClearDepthBuffer();

private:

	DirectXCommon() = default;
	~DirectXCommon() = default;

	/// 代入演算子、コピーコンストラクタの禁止
	DirectXCommon(const DirectXCommon& other) = delete;
	DirectXCommon(DirectXCommon&& other) = delete;
	DirectXCommon& operator= (const DirectXCommon& other) = delete;

};
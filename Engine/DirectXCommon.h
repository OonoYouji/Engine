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

	/// RTV
	ID3D12DescriptorHeap* rtvDescriptorHeap_ = nullptr;
	ID3D12Resource* swapChainResource_[2] = { nullptr };
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles_[2];

	/// Fence
	ID3D12Fence* fence_ = nullptr;
	uint64_t fenceValue_ = 0;

	/// windowのサイズ
	int32_t backBufferWidth_ = 0;
	int32_t backBufferHeight_ = 0;



	/// DXCの初期化
	IDxcUtils* dxcUtils_ = nullptr;
	IDxcCompiler3* dxcCompiler_ = nullptr;
	IDxcIncludeHandler* includeHandler_ = nullptr;

	/// RootSignatureの生成
	ID3D12RootSignature* rootSignature_ = nullptr;
	D3D12_ROOT_PARAMETER rootParameters_[1];
	/// InputLayout
	D3D12_INPUT_ELEMENT_DESC inputElemntDescs_[1];
	D3D12_INPUT_LAYOUT_DESC inputlayoutDesc_;
	/// BlendState
	D3D12_BLEND_DESC blendDesc_;
	/// RasterizerState
	D3D12_RASTERIZER_DESC rasterizerDesc_{};
	/// Shader
	IDxcBlob* vertexShaderBlob_ = nullptr;
	IDxcBlob* pixelShaderBlob_ = nullptr;

	/// PSO
	ID3D12PipelineState* graphicsPipelineState_ = nullptr;

	/// VertexResource
	ID3D12Resource* vertexResource_ = nullptr;

	/// VBV
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_;

	/// ビューポート
	D3D12_VIEWPORT viewport_;
	/// シザー矩形
	D3D12_RECT scissorRect_;

	/// MaterialResource
	ID3D12Resource* materialResource_ = nullptr;

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


	IDxcBlob* CompileShader(const std::wstring& filePath, const wchar_t* profile);


	void TestDraw(const Vector4& v1, const Vector4& v2, const Vector4& v3);

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



	/// <summary>
	/// DXCの初期化
	/// </summary>
	void InitializeDXC();

	/// <summary>
	/// ルートシグネチャの生成
	/// </summary>
	void CreateRootSignature();

	/// <summary>
	/// インプットレイアウトの設定
	/// </summary>
	void SetingInputLayout();

	/// <summary>
	/// ブレンドステートの設定
	/// </summary>
	void SetingBlendState();

	/// <summary>
	/// ラスタライザーステートの設定
	/// </summary>
	void SetingRasterizerState();

	/// <summary>
	/// 
	/// </summary>
	void SetingShader();

	/// <summary>
	/// 
	/// </summary>
	void CreatePSO();

	/// <summary>
	/// VertexResourceの生成
	/// </summary>
	void CreateVertexResource();

	ID3D12Resource* CreateBufferResource(size_t sizeInBytes);

	/// <summary>
	/// VertexBufferViewの生成
	/// </summary>
	void CreateVBV();

	void WriteVertexData(const Vector4& v1, const Vector4& v2, const Vector4& v3);

	void InitializeViewport();

	void CreateMaterialResource();

private:

	/// 代入演算子、コピーコンストラクタの禁止
	DirectXCommon(const DirectXCommon& other) = delete;
	DirectXCommon(DirectXCommon&& other) = delete;
	DirectXCommon& operator= (const DirectXCommon& other) = delete;

};
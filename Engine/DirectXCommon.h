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
#include <Vector2.h>
#include <Vector3.h>
#include <Vector4.h>
#include <Matrix4x4.h>

#include <d3dx12.h>
#include <DirectXTex.h>

using namespace Microsoft::WRL;

class DirectXCommon final {
private:

	DirectXCommon() = default;
	~DirectXCommon() = default;

private:

	/// <summary>
	/// 図形の各頂点が持つデータ
	/// </summary>
	struct VertexData {
		Vector4 position;
		Vec2f texcoord;
	};

	/// WinAppクラスへのポインタ;
	WinApp* p_winApp_;

	/// デバイス
	ComPtr<IDXGIFactory7> dxgiFactory_ = nullptr;
	ComPtr<ID3D12Device> device_ = nullptr;
	ComPtr<IDXGIAdapter4> useAdapter_ = nullptr;

	/// コマンド関連
	ComPtr<ID3D12CommandAllocator> commandAllocator_ = nullptr;
	ComPtr<ID3D12GraphicsCommandList> commandList_ = nullptr;
	ComPtr<ID3D12CommandQueue> commandQueue_ = nullptr;

	/// スワップチェーン
	ComPtr<IDXGISwapChain4> swapChain_ = nullptr;
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc_;

	/// RTV
	ComPtr<ID3D12DescriptorHeap> rtvDescriptorHeap_ = nullptr;
	ComPtr<ID3D12Resource> swapChainResource_[2] = { nullptr };
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles_[2];
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc_;

	/// Fence
	ComPtr<ID3D12Fence> fence_ = nullptr;
	uint64_t fenceValue_ = 0;
	HANDLE fenceEvent_;

	/// debug
	ComPtr<ID3D12Debug1> debugController_ = nullptr;

	/// windowのサイズ
	int32_t backBufferWidth_ = 0;
	int32_t backBufferHeight_ = 0;
	
	/// DSV
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle_;


	/// -------------------------------
	/// ↓ DXCompile class の移植
	/// -------------------------------

	/// DirectXCompile
	ComPtr<IDxcUtils> dxcUtils_ = nullptr;
	ComPtr<IDxcCompiler3> dxcCompiler_ = nullptr;
	ComPtr<IDxcIncludeHandler> includeHandler_ = nullptr;

	/// RootSignature
	ComPtr<ID3D12RootSignature> rootSignature_ = nullptr;
	D3D12_ROOT_PARAMETER rootParameters_[3];
	ComPtr<ID3DBlob> signatureBlob_ = nullptr;
	ComPtr<ID3DBlob> errorBlob_ = nullptr;

	/// InputLayout
	D3D12_INPUT_ELEMENT_DESC inputElemntDescs_[2];
	D3D12_INPUT_LAYOUT_DESC inputlayoutDesc_;

	/// BlendState
	D3D12_BLEND_DESC blendDesc_;

	/// RasterizerState
	D3D12_RASTERIZER_DESC rasterizerDesc_;

	/// Shader
	ComPtr<IDxcBlob> vertexShaderBlob_ = nullptr;
	ComPtr<IDxcBlob> pixelShaderBlob_ = nullptr;

	/// PSO
	ComPtr<ID3D12PipelineState> graphicsPipelineState_ = nullptr;
	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc_;

	/// VertexResource
	ComPtr<ID3D12Resource> vertexResource_ = nullptr;

	/// VBV
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_;

	/// ビューポート
	D3D12_VIEWPORT viewport_;

	/// シザー矩形
	D3D12_RECT scissorRect_;

	/// MaterialResource
	ComPtr<ID3D12Resource> materialResource_ = nullptr;

	/// wvpResource
	ComPtr<ID3D12Resource> wvpResource_ = nullptr;

	/// srvDescriptorHeap
	ComPtr<ID3D12DescriptorHeap> srvDescriptorHeap_ = nullptr;
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc_;
	D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU_;
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU_;

	/// texture
	DirectX::ScratchImage mipImages_;
	DirectX::TexMetadata metaData_;
	ComPtr<ID3D12Resource> textureResource_ = nullptr;

	/// descriptorRange
	D3D12_DESCRIPTOR_RANGE descriptorRange_[1];
	ComPtr<ID3D12Resource> intermediateResource_ = nullptr;

	/// DepthBuffer
	ComPtr<ID3D12Resource> depthStencilResource_ = nullptr;
	ComPtr<ID3D12DescriptorHeap> dsvDescriptorHeap_ = nullptr;
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc_;
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc_;

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

	/// <summary>
	/// ゲームの終了時に解放忘れがないかチェックする
	/// </summary>
	void DebugReleaseCheck();


	void TestDraw(const Matrix4x4& worldMatrix);


	ID3D12DescriptorHeap* CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible);

	void SetRenderTargets(const D3D12_CPU_DESCRIPTOR_HANDLE& dsvHandle);


	ID3D12Device* GetDevice() { return device_.Get(); }
	ID3D12GraphicsCommandList* GetCommandList() { return commandList_.Get(); }

	const D3D12_RENDER_TARGET_VIEW_DESC& GetRTVDesc() const { return rtvDesc_; }
	const DXGI_SWAP_CHAIN_DESC1& GetSwapChainDesc() const { return swapChainDesc_; }

	ID3D12DescriptorHeap* GetSrvDescriptorHeap() const { return srvDescriptorHeap_.Get(); }


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


	/// ----------------------------------
	/// DXCompile class の移植
	/// ----------------------------------

	/// <summary>
	/// DirectXCompileの初期化
	/// </summary>
	void InitializeDXC();

	/// <summary>
	/// RootSignatureの生成
	/// </summary>
	void CreateRootSignature();

	/// <summary>
	/// インプットレイアウトの初期化
	/// </summary>
	void InitializeInputLayout();

	/// <summary>
	/// ブレンドステートの初期化
	/// </summary>
	void InitializeBlendState();

	/// <summary>
	/// ラスタライザーステートの初期化
	/// </summary>
	void InitializeRasterizerState();

	/// <summary>
	/// シェーダーの初期化
	/// </summary>
	void InitializeShader();

	/// <summary>
	/// シェーダーをコンパイルする
	/// </summary>
	IDxcBlob* CompileShader(const std::wstring& filePath, const wchar_t* profile);

	/// <summary>
	/// パイプラインステートの初期化
	/// </summary>
	void InitializePSO();
	
	/// <summary>
	/// VertexResourceの初期化
	/// </summary>
	void InitializeVertexResource();

	/// <summary>
	/// ビューポート; シザー矩形の初期化
	/// </summary>
	void InitializeViewport();

	/// <summary>
	/// material; wvpのResourceの生成
	/// </summary>
	void CreateResource();

	/// <summary>
	/// Resourceの生成
	/// </summary>
	ID3D12Resource* CreateBufferResource(size_t sizeInBytes);

	/// <summary>
	/// シェーダーリソースの生成
	/// </summary>
	void CreateShaderResourceView();

	/// <summary>
	/// TextureResourceの初期化
	/// </summary>
	void InitializeTextureResource();

	/// <summary>
	/// テクスチャの読み込み
	/// </summary>
	DirectX::ScratchImage LoadTexture(const std::string& filePath);

	/// <summary>
	/// テクスチャリソースの生成
	/// </summary>
	ID3D12Resource* CreateTextureResource(const DirectX::TexMetadata& metaData);

	/// <summary>
	/// デスクリプターレンジの初期化
	/// </summary>
	void InitializeDescriptorRange();

	/// <summary>
	/// デプスバッファステンシル
	/// </summary>
	ID3D12Resource* CreateDepthStenciltextureResource(int32_t width, int32_t height);

	/// <summary>
	/// TextureData
	/// </summary>
	ID3D12Resource* UploadTextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImage);


private:

	/// 代入演算子、コピーコンストラクタの禁止
	DirectXCommon(const DirectXCommon& other) = delete;
	DirectXCommon(DirectXCommon&& other) = delete;
	DirectXCommon& operator= (const DirectXCommon& other) = delete;

};
#pragma once

#include <d3d12.h>
#include <dxgi1_6.h>
#include <cassert>
#include <cmath>	
#include <dxgidebug.h>
#include <dxcapi.h>

/// ComPtr用
#include <wrl/client.h>

#include <string>


#include <WorldTransform.h>
#include <Camera.h>
#include <memory>


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

	///- 完璧な画面クリアを目指す
	ComPtr<ID3D12Fence> fence_;
	uint64_t fenceValue_;
	HANDLE fenceEvent_;

	///- 三角形を表示しよう
	ComPtr<IDxcUtils> dxcUtils_;
	ComPtr<IDxcCompiler3> dxcCompiler_;
	ComPtr<IDxcIncludeHandler> includeHandler_;

	ComPtr<ID3DBlob> signatureBlob_;
	ComPtr<ID3DBlob> errorBlob_;
	ComPtr<ID3D12RootSignature> rootSignature_;

	D3D12_INPUT_ELEMENT_DESC inputElementDescs_[1];
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc_;

	D3D12_BLEND_DESC blendDesc_;

	D3D12_RASTERIZER_DESC rasterizerDesc_;

	ComPtr<IDxcBlob> vertexShaderBlob_;
	ComPtr<IDxcBlob> pixelShaderBlob_;

	ComPtr<ID3D12PipelineState> graphicsPipelineState_;

	ComPtr<ID3D12Resource> vertexResource_;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_;

	D3D12_VIEWPORT viewport_;
	D3D12_RECT scissorRect_;

	///- 三角形の色を変えよう
	D3D12_ROOT_PARAMETER rootParameters_[2];
	ComPtr<ID3D12Resource> materialResource_;

	///- 三角形を動かそう
	ComPtr<ID3D12Resource> wvpResource_;

	WorldTransform worldTransform_;
	std::unique_ptr<Camera> camera_;

private:

	/// -----------------------------------
	/// ↓ privateなメンバ関数
	/// -----------------------------------

	void InitializeDXGIDevice();
	
	void InitializeCommand();

	void InitializeSwapChain();

	void InitialiezRenderTarget();

	void InitializeFence();

	void InitializeDxcCompiler();

	IDxcBlob* CompileShader(const std::wstring& filePath, const wchar_t* profile);

	void InitializeRootSignature();

	void InitializeInputLayout();

	void InitializeBlendState();

	void InitializeRasterizer();

	void InitializeShaderBlob();

	void InitializePSO();

	void InitializeVertexResource();

	void InitializeViewport();

	void InitializeMaterialResource();

	void InitializeWVPResource();

	void WriteWVPResource(const Mat4& matrix);

	ID3D12Resource* CreateBufferResource(size_t sizeInBytes);

	void ClearRenderTarget();


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


	void TestDraw();

private:

	DirectXCommon() = default;
	~DirectXCommon() = default;

	/// 代入演算子、コピーコンストラクタの禁止
	DirectXCommon(const DirectXCommon& other) = delete;
	DirectXCommon(DirectXCommon&& other) = delete;
	DirectXCommon& operator= (const DirectXCommon& other) = delete;

};
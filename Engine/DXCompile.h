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

#include <Vector3.h>
#include <Vector4.h>

class DXCompile final {
private:


	DXCompile() = default;
	~DXCompile();

public:

	static DXCompile* GetInstance();

	void Initialize();
	void Finalize();


	IDxcBlob* CompileShader(const std::wstring& filePath, const wchar_t* profile);

	void TestDraw(const Vector4& v1, const Vector4& v2, const Vector4& v3, const Vec3f& scale, const Vec3f& rotate, Vec3f& translate);


private:

	/// DXCの初期化
	IDxcUtils* dxcUtils_ = nullptr;
	IDxcCompiler3* dxcCompiler_ = nullptr;
	IDxcIncludeHandler* includeHandler_ = nullptr;

	/// RootSignatureの生成
	ID3D12RootSignature* rootSignature_ = nullptr;
	D3D12_ROOT_PARAMETER rootParameters_[2];
	ID3DBlob* signatureBlob_ = nullptr;
	ID3DBlob* errorBlob_ = nullptr;
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
	/// wvpResource
	ID3D12Resource* wvpResource_ = nullptr;



private:


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

	/// <summary>
	/// マテリアルリソースの生成
	/// </summary>
	void CreateMaterialResource();

	/// <summary>
	/// wvp用のリソース生成
	/// </summary>
	void CreateWVPResource(const Vec3f& scale, const Vec3f& rotate, const Vec3f& translate);

private:

	/// 代入演算子、コピーコンストラクタの禁止
	DXCompile(const DXCompile& other) = delete;
	DXCompile(DXCompile&& other) = delete;
	DXCompile& operator= (const DXCompile& other) = delete;

};
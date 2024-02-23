#pragma once


#include <d3d12.h>
#include <dxgi1_6.h>
#include <cassert>
#include <vector>
#include <chrono>
#include <wrl.h>
#include <cstdlib>
#include <dxcapi.h>
#include <DirectXTex.h>

#include <Vector2.h>
#include <Vector3.h>
#include <Vector4.h>

class DXCompile final {
private:


	DXCompile();
	~DXCompile();

public:

	static DXCompile* GetInstance();

	void Initialize();
	void Finalize();


	IDxcBlob* CompileShader(const std::wstring& filePath, const wchar_t* profile);

	void TestDraw(const Vector4& v1, const Vector4& v2, const Vector4& v3, const Vec3f& scale, const Vec3f& rotate, Vec3f& translate);

	ID3D12DescriptorHeap* GetSrvDescriptorHeap() const { return srvDescriptorHeap_; }

private:

	/// DXCの初期化
	IDxcUtils* dxcUtils_ = nullptr;
	IDxcCompiler3* dxcCompiler_ = nullptr;
	IDxcIncludeHandler* includeHandler_ = nullptr;

	/// RootSignatureの生成
	ID3D12RootSignature* rootSignature_ = nullptr;
	D3D12_ROOT_PARAMETER rootParameters_[3];
	ID3DBlob* signatureBlob_ = nullptr;
	ID3DBlob* errorBlob_ = nullptr;
	/// InputLayout
	D3D12_INPUT_ELEMENT_DESC inputElemntDescs_[2];
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

	/// srvDescriptorHeap
	ID3D12DescriptorHeap* srvDescriptorHeap_ = nullptr;
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc_;
	D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU_;
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU_;

	/// texture
	DirectX::ScratchImage mipImages_;
	DirectX::TexMetadata metaData_;
	ID3D12Resource* textureResource_;

	struct VertexData {
		Vector4 position;
		Vec2f texcoord;
	};

	/// descriptorRange
	D3D12_DESCRIPTOR_RANGE descriptorRange_[1];


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


	DirectX::ScratchImage LoadTexture(const std::string& filePath);

	ID3D12Resource* CreateTextureResource(const DirectX::TexMetadata& metaData);

	void UploadTextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImage);

	void CreateShaderResourceView();

	void InitializeDescriptorRange();

private:

	/// 代入演算子、コピーコンストラクタの禁止
	DXCompile(const DXCompile& other) = delete;
	DXCompile(DXCompile&& other) = delete;
	DXCompile& operator= (const DXCompile& other) = delete;

};
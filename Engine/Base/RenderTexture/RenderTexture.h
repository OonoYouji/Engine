#pragma once

#include <d3d12.h>
#include <wrl/client.h>

#include <Vector4.h>
#include <TextureManager.h>

using namespace Microsoft::WRL;

class DxDescriptors;
class DirectXCommon;
class DxCommand;
class IScene;


/// ===================================================
/// offscreen用render target texture
/// ===================================================
class RenderTexture {
public:

	RenderTexture();
	~RenderTexture();

	/// ===================================================
	/// public : methods
	/// ===================================================

	void InitializeSwapChain(UINT buffer, const Vector4& clearColor);
	void Initialize(UINT width, UINT height, const Vector4& clearColor, DXGI_FORMAT srvFormat = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB);
	void InitializeUAV(UINT width, UINT height, const Vector4& clearColor);
		
	void SetRenderTarget();

	void Clear();

	void CreateBarrier(D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after);

	const TextureManager::Texture& GetTexture() const;

	void ImGuiImage();

	void SetName(const std::string& name);

	void SetSceneLink(IScene* link);

private:

	/// ===================================================
	/// private : methods
	/// ===================================================

	void CreateRenderTargetBuffer();

	void CreateOffScreenBuffer();

	void CreateSRV(DXGI_FORMAT format);

	void CreateUAV();

private:

	/// ===================================================
	/// other class pointer
	/// ===================================================
	DirectXCommon* dxCommon_ = nullptr;
	DxDescriptors* dxDescriptors_ = nullptr;
	DxCommand* dxCommand_ = nullptr;

	/// ===================================================
	/// private : objects
	/// ===================================================

	ComPtr<ID3D12Resource> targetBuffer_;
	D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle_{};
	D3D12_CPU_DESCRIPTOR_HANDLE cpuHandleUAV_{};
	D3D12_RESOURCE_STATES currentState_{};

	D3D12_CPU_DESCRIPTOR_HANDLE srvCpuHandle_{};
	D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHandle_{};

	D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle_{};

	TextureManager::Texture texture_;

	UINT width_, height_;
	Vector4 clearColor_{};

	std::string name_ = "NONE";
	IScene* sceneLink_ = nullptr;
	
};
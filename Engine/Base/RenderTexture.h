#pragma once

#include <d3d12.h>
#include <wrl/client.h>

#include <Vector4.h>
#include <TextureManager.h>

using namespace Microsoft::WRL;

class DxDescriptors;
class DirectXCommon;
class DxCommand;


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

	void Initialize(UINT buffer, const Vector4& clearColor);
	void InitializeOffScreen(UINT width, UINT height, const Vector4& clearColor);

	void SetRenderTarget();

	void Clear();

	void CreateBarrier(D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after);

	void CopyBuffer();

	const TextureManager::Texture& GetTexture() const;

private:

	/// ===================================================
	/// private : methods
	/// ===================================================

	void CreateRenderTargetBuffer();

	void CreateOffScreenBuffer();

	void CreateSRV();


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
	D3D12_RESOURCE_STATES currentState_{};

	D3D12_CPU_DESCRIPTOR_HANDLE srvCpuHandle_{};
	D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHandle_{};

	TextureManager::Texture texture_;

	UINT width_, height_;
	Vector4 clearColor_{};


};
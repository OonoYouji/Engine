#pragma once

#include <d3d12.h>
#include <wrl/client.h>

class WinApp;
class DirectXCommon;
using namespace Microsoft::WRL;

class ImGuiManager final {
private:

	ImGuiManager() = default;
	~ImGuiManager() = default;

private:

	/// SRV
	ComPtr<ID3D12DescriptorHeap> p_srvDescriptorHeap_ = nullptr;

	DirectXCommon* p_directXCommon_;
	ComPtr<ID3D12GraphicsCommandList> p_commandList_;

public:

	static ImGuiManager* GetInstance();

	void Initialize(WinApp* winApp, DirectXCommon* dxCommon);
	void Finalize();

	void BeginFrame();
	void EndFrame();

};
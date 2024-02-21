#pragma once

#include <d3d12.h>

class WinApp;
class DirectXCommon;

class ImGuiManager final {
private:

	ImGuiManager() = default;
	~ImGuiManager() = default;

private:

	/// SRV
	ID3D12DescriptorHeap* srvDescriptorHeap_ = nullptr;

	DirectXCommon* p_directXCommon_;
	ID3D12GraphicsCommandList* p_commandList_;

public:

	static ImGuiManager* GetInstance();

	void Initialize(WinApp* winApp, DirectXCommon* dxCommon);
	void Finalize();

	void BeginFrame();
	void EndFrame();

};
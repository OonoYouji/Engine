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

	ComPtr<ID3D12DescriptorHeap> srvHeap_;
	DirectXCommon* dxCommon_;

public:

	static ImGuiManager* GetInstance();

	void Initialize(WinApp* winApp, DirectXCommon* dxCommon);
	void Finalize();

	void BeginFrame();
	void EndFrame();

};
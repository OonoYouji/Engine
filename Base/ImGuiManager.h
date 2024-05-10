#pragma once

#include <d3d12.h>
#include <wrl/client.h>


#include "Externals/imgui/imgui.h"
#include "Externals/imgui/imgui_impl_dx12.h"
#include "Externals/imgui/imgui_impl_win32.h"


class WinApp;
class DirectXCommon;
using namespace Microsoft::WRL;

class ImGuiManager final {
private:

	ImGuiManager() = default;
	~ImGuiManager() = default;

	DirectXCommon* dxCommon_;

public:

	static ImGuiManager* GetInstance();

	void Initialize(WinApp* winApp, DirectXCommon* dxCommon);
	void Finalize();

	void RenderMultiViewport();

	void BeginFrame();
	void EndFrame();

};
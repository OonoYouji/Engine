#include <ImGuiManager.h>

#include <WinApp.h>
#include <DirectXCommon.h>
#include <DXCompile.h>

#include "Externals/imgui/imgui.h"
#include "Externals/imgui/imgui_impl_dx12.h"
#include "Externals/imgui/imgui_impl_win32.h"


ImGuiManager* ImGuiManager::GetInstance() {
	static ImGuiManager instance;
	return &instance;
}



void ImGuiManager::Initialize(WinApp* winApp, DirectXCommon* dxCommon) {

	dxCommon_ = dxCommon;

	srvHeap_ = dxCommon_->CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 128, true);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(winApp->GetHWND());
	ImGui_ImplDX12_Init(
		dxCommon_->GetDevice(),
		dxCommon_->GetSwapChainDesc().BufferCount,
		dxCommon_->GetRTVDesc().Format,
		srvHeap_.Get(),
		srvHeap_->GetCPUDescriptorHandleForHeapStart(),
		srvHeap_->GetGPUDescriptorHandleForHeapStart()
	);
}

void ImGuiManager::Finalize() {

	dxCommon_ = nullptr;
	srvHeap_.Reset();
	
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

}

void ImGuiManager::BeginFrame() {

	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ID3D12DescriptorHeap* descriptorHeap[] = { srvHeap_.Get() };
	dxCommon_->GetCommandList()->SetDescriptorHeaps(1, descriptorHeap);


}

void ImGuiManager::EndFrame() {

	ImGui::Render();
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), dxCommon_->GetCommandList());

}

#include <ImGuiManager.h>

#include <WinApp.h>
#include <DirectXCommon.h>


ImGuiManager* ImGuiManager::GetInstance() {
	static ImGuiManager instance;
	return &instance;
}



void ImGuiManager::Initialize(WinApp* winApp, DirectXCommon* dxCommon) {

	dxCommon_ = dxCommon;

	ID3D12DescriptorHeap* srvHeap = dxCommon_->GetSrvHeap();

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(winApp->GetHWND());
	ImGui_ImplDX12_Init(
		dxCommon_->GetDevice(),
		dxCommon_->GetSwapChainDesc().BufferCount,
		dxCommon_->GetRTVDesc().Format,
		srvHeap,
		srvHeap->GetCPUDescriptorHandleForHeapStart(),
		srvHeap->GetGPUDescriptorHandleForHeapStart()
	);
}

void ImGuiManager::Finalize() {

	dxCommon_ = nullptr;
	
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

}

void ImGuiManager::BeginFrame() {

	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ID3D12DescriptorHeap* descriptorHeap[] = { dxCommon_->GetSrvHeap() };
	dxCommon_->GetCommandList()->SetDescriptorHeaps(1, descriptorHeap);


}

void ImGuiManager::EndFrame() {

	ImGui::Render();
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), dxCommon_->GetCommandList());

}

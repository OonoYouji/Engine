#include <ImGuiManager.h>

#include <WinApp.h>
#include <DirectXCommon.h>
#include <DxDescriptors.h>
#include <DxCommand.h>


ImGuiManager* ImGuiManager::GetInstance() {
	static ImGuiManager instance;
	return &instance;
}



void ImGuiManager::Initialize(WinApp* winApp, DirectXCommon* dxCommon) {

	dxCommon_ = dxCommon;

	ID3D12DescriptorHeap* srvHeap = DxDescriptors::GetInstance()->GetSRVHeap();

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGuiIO& imGuiIO = ImGui::GetIO();
	imGuiIO.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	imGuiIO.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;


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


void ImGuiManager::RenderMultiViewport() {

	// Update and Render additional Platform Windows
	if(ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}

}

void ImGuiManager::BeginFrame() {



	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ID3D12DescriptorHeap* descriptorHeap[] = { DxDescriptors::GetInstance()->GetSRVHeap() };
	DxCommand::GetInstance()->GetList()->SetDescriptorHeaps(1, descriptorHeap);


}

void ImGuiManager::EndFrame() {



	


	ImGui::Render();
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), DxCommand::GetInstance()->GetList());

}

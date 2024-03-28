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

	p_directXCommon_ = dxCommon;

	p_srvDescriptorHeap_ = DirectXCommon::GetInstance()->GetSrvDescriptorHeap().Get();


	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(winApp->GetHWND());
	ImGui_ImplDX12_Init(
		p_directXCommon_->GetDevice().Get(),
		p_directXCommon_->GetSwapChainDesc().BufferCount,
		p_directXCommon_->GetRTVDesc().Format,
		p_srvDescriptorHeap_.Get(),
		p_srvDescriptorHeap_->GetCPUDescriptorHandleForHeapStart(),
		p_srvDescriptorHeap_->GetGPUDescriptorHandleForHeapStart()
	);
}

void ImGuiManager::Finalize() {

	p_directXCommon_ = nullptr;
	p_srvDescriptorHeap_.Reset();
	p_commandList_.Reset();

	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

}

void ImGuiManager::BeginFrame() {

	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	p_commandList_ = p_directXCommon_->GetCommandList();

	ID3D12DescriptorHeap* descriptorHeap[] = { p_srvDescriptorHeap_.Get() };
	p_commandList_->SetDescriptorHeaps(1, descriptorHeap);


}

void ImGuiManager::EndFrame() {

	ImGui::Render();
	//ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), p_commandList_);

}

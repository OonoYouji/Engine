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
	//imGuiIO.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;


	ImGui::StyleColorsDark();
	StyleSetting();
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

	//// Update and Render additional Platform Windows
	//if(ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
	//	ImGui::UpdatePlatformWindows();
	//	ImGui::RenderPlatformWindowsDefault();
	//}

}

void ImGuiManager::BeginFrame() {



	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ID3D12DescriptorHeap* descriptorHeap[] = { DxDescriptors::GetInstance()->GetSRVHeap() };
	DxCommand::GetInstance()->GetList()->SetDescriptorHeaps(1, descriptorHeap);






#ifdef _DEBUG
	ImGui::Begin("main window", NULL, ImGuiWindowFlags_NoTitleBar |  ImGuiWindowFlags_NoResize);
	ImGui::SetWindowSize(ImVec2(1280.0f, 720.0f));
	ImGui::End();



	/// -------------------------------------------------------------
	/// ↓ 固定Tabの表示
	/// -------------------------------------------------------------
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove;

	ImGui::Begin("Tab A", nullptr, window_flags);
	ImGui::End();

	ImGui::Begin("Tab B", nullptr, window_flags);
	ImGui::End();

	ImGui::Begin("Tab C", nullptr, window_flags);
	ImGui::End();

	ImGui::Begin("Tab D", nullptr, window_flags);
	ImGui::End();
#endif // _DEBUG


}

void ImGuiManager::EndFrame() {






	ImGui::Render();
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), DxCommand::GetInstance()->GetList());

}

void ImGuiManager::StyleSetting() {
	ImGuiStyle& style = ImGui::GetStyle();

	style.Colors[ImGuiCol_FrameBg] = ColorToVec4(0x3e3e3e8a);
	style.Colors[ImGuiCol_TitleBgActive] = ColorToVec4(0x4c4c4cff);
	style.Colors[ImGuiCol_CheckMark] = ColorToVec4(0xffffffff);
	style.Colors[ImGuiCol_ScrollbarGrab] = ColorToVec4(0xffffffff);
	style.Colors[ImGuiCol_Button] = ColorToVec4(0x3e3e3e8a);
	style.Colors[ImGuiCol_Header] = ColorToVec4(0xcccccc4f);
	style.Colors[ImGuiCol_Tab] = ColorToVec4(0x333333ff);
	style.Colors[ImGuiCol_TabActive] = ColorToVec4(0xaaaaaaff);

}

ImVec4 ImGuiManager::ColorToVec4(uint32_t color) {
	ImVec4 colorf = {
		((color >> 24) & 0xff) / 255.0f, //- R
		((color >> 16) & 0xff) / 255.0f, //- G
		((color >> 8) & 0xff) / 255.0f,  //- B
		((color >> 0) & 0xff) / 255.0f   //- A
	}; 


	return colorf;
}

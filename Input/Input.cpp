#include "Input.h"

#include "ImGuiManager.h"

#pragma comment (lib, "dinput8.lib")
#pragma comment (lib, "dxguid.lib")


Input* Input::GetInstance() {
	static Input instance;
	return &instance;
}



void Input::Init() {

	mouse_ = Mouse();
}



void Input::Begin() {
#ifdef _DEBUG
	ImGui::Begin("Input");

	ImGui::DragFloat2("mousePos", &mousePos_.x, 0.0f);

	ImGui::Text("mouseLeftButton: %d", mouse_.leftButton_);

	ImGui::End();
#endif // _DEBUG

	mouse_.preLeftButton_ = mouse_.leftButton_;

}

#include "Input.h"

#include "ImGuiManager.h"



Input* Input::GetInstance() {
	static Input instance;
	return &instance;
}



void Input::Init() {
}



void Input::Begin() {
#ifdef _DEBUG
	ImGui::Begin("Input");

	ImGui::DragFloat2("mousePos", &mousePos_.x, 0.0f);

	ImGui::End();
#endif // _DEBUG

}

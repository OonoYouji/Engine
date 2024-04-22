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

	if(ImGui::TreeNode("Mouse")) {

		ImGui::DragFloat2("pos", &mouse_.position.x, 0.0f);

		ImGui::Text("leftButton: %d", mouse_.leftButton);
		ImGui::Text("rightButton: %d", mouse_.rightButton);

		ImGui::TreePop();
	}

	ImGui::End();
#endif // _DEBUG

	mouse_.preLeftButton = mouse_.leftButton;
	mouse_.preRightButton = mouse_.rightButton;

}

void Input::Reset() {

	mouse_.leftButton = false;
	mouse_.rightButton = false;

}

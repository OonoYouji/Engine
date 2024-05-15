#include "Input.h"

#pragma comment (lib, "dinput8.lib")
#pragma comment (lib, "dxguid.lib")

#include <cassert>

#include "ImGuiManager.h"
#include <WinApp.h>


Input* Input::GetInstance() {
	static Input instance;
	return &instance;
}



void Input::Init() {

	mouse_ = Mouse();

}

void Input::Initialize(WinApp* winApp) {
	HRESULT result = S_FALSE;

	mouse_ = Mouse();


	/// ---------------------------------
	/// ↓ DirectInputオブジェクトの生成
	/// ---------------------------------
	result = DirectInput8Create(
		winApp->GetWNDCLASS().hInstance,
		DIRECTINPUT_VERSION,
		IID_IDirectInput8,
		(void**)(&directInput_),
		nullptr
	);

	assert(SUCCEEDED(result) && "DirectInputの生成に失敗しました");


	/// ---------------------------------
	/// ↓ キーボードデバイスの生成
	/// ---------------------------------

	result = directInput_->CreateDevice(
		GUID_SysKeyboard, &keyboard_, NULL);

	assert(SUCCEEDED(result) && "キーボードデバイスの生成に失敗しました");

	///- 入力データ形式のセット
	result = keyboard_->SetDataFormat(&c_dfDIKeyboard); //- 標準形式
	assert(SUCCEEDED(result));

	///- 排他制御レベルのセット
	// DISCL_FOREGROUND   : 画面が手前にある場合のみ入力を受け付ける
	// DISCL_NONEXCLUSIVE : デバイスをこのアプリだけで占有しない
	// DISCL_NOWINKEY     : Windowsキーを無効にする

	result = keyboard_->SetCooperativeLevel(
		winApp->GetHWND(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(result));



}


void Input::Finalize() {
	directInput_.Reset();
	keyboard_.Reset();
}



void Input::Begin() {

	mouse_.preLeftButton = mouse_.leftButton;
	mouse_.preRightButton = mouse_.rightButton;

	///- キーボード情報の取得開始
	//keyboard_->Acquire();
	//memcpy(preKeys_, keys_, 256);
	//keyboard_->GetDeviceState(sizeof(keys_), keys_);


#ifdef _DEBUG
	ImGui::Begin("Inputs");

	if(ImGui::TreeNode("Mouse")) {

		ImGui::DragFloat2("pos", &mouse_.position.x, 0.0f);

		ImGui::Text("leftButton: %d", mouse_.leftButton);
		ImGui::Text("rightButton: %d", mouse_.rightButton);

		ImGui::TreePop();
	}

	if(ImGui::TreeNodeEx("PressKey", true)) {

		/*for(uint32_t index = 0; index < 256; ++index) {
			if(keys_[index]) {
				ImGui::Text("Key:" + char(index));
			}
		}*/

		ImGui::TreePop();
	}

	ImGui::End();
#endif // _DEBUG



}

void Input::Reset() {

	mouse_.leftButton = false;
	mouse_.rightButton = false;

}

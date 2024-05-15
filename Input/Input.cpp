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



void Input::Initialize(WinApp* winApp) {
	HRESULT result = S_FALSE;

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



	/// ---------------------------------
	/// ↓ マウスデバイスの生成
	/// ---------------------------------

	result = directInput_->CreateDevice(
		GUID_SysMouse, &mouse_, NULL);
	assert(SUCCEEDED(result) && "マウスデバイスの生成に失敗しました");

	result = mouse_->SetDataFormat(&c_dfDIMouse);
	assert(SUCCEEDED(result));

	result = mouse_->SetCooperativeLevel(
		winApp->GetHWND(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(result));

}


void Input::Finalize() {
	directInput_.Reset();
	keyboard_.Reset();
	mouse_.Reset();
}



void Input::Begin() {

	///- キーボード情報の取得開始
	keyboard_->Acquire();
	memcpy(preKeys_, keys_, 256);
	keyboard_->GetDeviceState(sizeof(keys_), keys_);

	///- マウスの情報を所得開始
	mouse_->Acquire();
	mouse_->GetDeviceState(sizeof(mouseState_), &mouseState_);

#ifdef _DEBUG
	ImGui::Begin("Inputs");

	if(ImGui::TreeNodeEx("Keyboard", true)) {

		for(uint32_t index = 0; index < 256; ++index) {
			if(keys_[index]) {
				ImGui::Text("Key: %d", index);
			}
		}

		ImGui::TreePop();
	}

	if(ImGui::TreeNodeEx("Mouse", true)) {

		float position[] = {
			static_cast<float>(mouseState_.lX),
			static_cast<float>(mouseState_.lY),
			static_cast<float>(mouseState_.lZ)
		};
		ImGui::DragFloat3("velocity", &position[0], 0.0f);

		for(uint32_t index = 0; index < 4; ++index) {
			if(mouseState_.rgbButtons[index]) {
				ImGui::Text("Button: %d", index);
			}
		}

		ImGui::TreePop();
	}

	ImGui::End();
#endif // _DEBUG



}

#include "Input.h"

#pragma comment (lib, "dinput8.lib")
#pragma comment (lib, "dxguid.lib")
#pragma comment (lib, "Xinput.lib")

#include <cassert>
#include <iostream>

#include "ImGuiManager.h"
#include <WinApp.h>


///- instanceを設定
InputManager* Input::manager_ = InputManager::GetInstance();


InputManager* InputManager::GetInstance() {
	static InputManager instance;
	return &instance;
}



void InputManager::Initialize(WinApp* winApp) {
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

	result = mouse_->SetDataFormat(&c_dfDIMouse2);
	assert(SUCCEEDED(result));

	result = mouse_->SetCooperativeLevel(
		winApp->GetHWND(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(result));




}


void InputManager::Finalize() {
	directInput_.Reset();
	keyboard_.Reset();
	mouse_.Reset();
}



void InputManager::Begin() {

	///- キーボード情報の取得開始
	keyboard_->Acquire();
	memcpy(preKeys_, keys_, 256);
	keyboard_->GetDeviceState(sizeof(keys_), keys_);

	///- マウスの情報を取得開始
	mouse_->Acquire();
	mouse_->GetDeviceState(sizeof(mouseState_), &mouseState_);
	POINT mousePos{};
	GetCursorPos(&mousePos);
	ScreenToClient(WinApp::GetInstance()->GetHWND(), &mousePos);
	mousePosition_ = {
		static_cast<float>(mousePos.x),
		static_cast<float>(mousePos.y)
	};


	//ZeroMemory(&gamePad_, sizeof(XINPUT_STATE));
	preGamePad_ = gamePad_;
	isGamePadActive_ = XInputGetState(0, &gamePad_) == ERROR_SUCCESS;



	DebugDraw(true);
}


void InputManager::DebugDraw([[maybe_unused]] bool isDraw) {
#ifdef _DEBUG
	if(!isDraw) { return; }
	ImGui::Begin("Inputs");


	/// --------------------------------
	/// ↓ Keyboard
	/// --------------------------------
	if(ImGui::TreeNodeEx("Keyboard", true)) {

		for(uint32_t index = 0; index < 256; ++index) {
			if(Input::PressKey(index)) {
				ImGui::Text("Key: %d", index);
			}
		}

		ImGui::TreePop();
	}

	/// --------------------------------
	/// ↓ Mouse
	/// --------------------------------
	if(ImGui::TreeNodeEx("Mouse", true)) {

		ImGui::DragFloat2("Position", &mousePosition_.x, 0.0f);

		float position[] = {
			static_cast<float>(mouseState_.lX),
			static_cast<float>(mouseState_.lY),
			static_cast<float>(mouseState_.lZ)
		};
		ImGui::DragFloat3("velocity", &position[0], 0.0f);

		for(uint32_t index = 0; index < 8; ++index) {
			if(mouseState_.rgbButtons[index]) {
				ImGui::Text("Button: %d", index);
			}
		}

		ImGui::TreePop();
	}

	/// --------------------------------
	/// ↓ GamePad
	/// --------------------------------

	if(ImGui::TreeNodeEx("GamePad", true)) {

		GAMEPAD_STATE buttons[] = {
			BUTTON_UP, BUTTON_DOWN, BUTTON_LEFT, BUTTON_RIGHT,
			BUTTON_BACK, BUTTON_START,
			BUTTON_L_STICK, BUTTON_R_STICK,
			BUTTON_LB, BUTTON_RB,
			BUTTON_A, BUTTON_B, BUTTON_X, BUTTON_Y
		};

		for(GAMEPAD_STATE state : buttons) {
			if(Input::PressGamePad(state)) {
				switch(state) {
				case BUTTON_UP:		 ImGui::Text("Key: UP");		break;
				case BUTTON_DOWN:	 ImGui::Text("Key: Down");		break;
				case BUTTON_LEFT:	 ImGui::Text("Key: Left");		break;
				case BUTTON_RIGHT:	 ImGui::Text("Key: Right");		break;
				case BUTTON_BACK:	 ImGui::Text("Key: Back");		break;
				case BUTTON_START:	 ImGui::Text("Key: Start");		break;
				case BUTTON_L_STICK: ImGui::Text("Key: L_Stick");	break;
				case BUTTON_R_STICK: ImGui::Text("Key: R_Stick");	break;
				case BUTTON_LB:		 ImGui::Text("Key: LB");		break;
				case BUTTON_RB:		 ImGui::Text("Key: RB");		break;
				case BUTTON_A:		 ImGui::Text("Key: A");			break;
				case BUTTON_B:		 ImGui::Text("Key: B");			break;
				case BUTTON_X:		 ImGui::Text("Key: X");			break;
				case BUTTON_Y:		 ImGui::Text("Key: Y");			break;
				}
			}
		}

		static int value = 0;
		if(Input::PressGamePadLT(&value)) {
			ImGui::Text("Key: LT, %d", value);
		}

		if(Input::PressGamePadRT(&value)) {
			ImGui::Text("Key: RT, %d", value);
		}

		Vec2f lStickDir = Input::GamePad_L_Stick();
		Vec2f rStickDir = Input::GamePad_R_Stick();
		ImGui::Text("L_stick direction: %0.2f, %0.2f", lStickDir.x, lStickDir.y);
		ImGui::Text("R_stick direction: %0.2f, %0.2f", rStickDir.x, rStickDir.y);

		ImGui::TreePop();
	}

	ImGui::End();
#endif // _DEBUG
}


bool Input::PressGamePad(GAMEPAD_STATE state) {
	return (manager_->gamePad_.Gamepad.wButtons & (state)) == (state);
}

bool Input::TriggerGamePad(GAMEPAD_STATE state) {
	return PressKey(state) && !((manager_->preGamePad_.Gamepad.wButtons & (state)) == (state));
}

bool Input::ReleaseGamePad(GAMEPAD_STATE state) {
	return !PressKey(state) && ((manager_->preGamePad_.Gamepad.wButtons & (state)) == (state));
}

bool Input::PressGamePadLT(int* const value) {
	const BYTE kThreshold = 30;
	if(value) {
		*value = manager_->gamePad_.Gamepad.bLeftTrigger;
	}
	return (manager_->gamePad_.Gamepad.bLeftTrigger > kThreshold);
}

bool Input::PressGamePadRT(int* const value) {
	const BYTE kThreshold = 30;
	if(value) {
		*value = manager_->gamePad_.Gamepad.bRightTrigger;
	}
	return (manager_->gamePad_.Gamepad.bRightTrigger > kThreshold);
}

Vec2f Input::GamePad_L_Stick() {
	//const float kMaxAxis = 32767.0f;
	return Vec2f(
		float(manager_->gamePad_.Gamepad.sThumbLX),
		float(manager_->gamePad_.Gamepad.sThumbLY)
	);
}

Vec2f Input::GamePad_R_Stick() {
	//const float kMaxAxis = 32767.0f;
	return Vec2f(
		float(manager_->gamePad_.Gamepad.sThumbRX),
		float(manager_->gamePad_.Gamepad.sThumbRY)
	);
}

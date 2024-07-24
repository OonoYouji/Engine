#pragma once

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <wrl/client.h>
#include <Xinput.h>

#include <cmath>

#include "Vector2.h"

using namespace Microsoft::WRL;
class WinApp;


enum GAMEPAD_STATE {
	BUTTON_UP      = XINPUT_GAMEPAD_DPAD_UP,
	BUTTON_DOWN    = XINPUT_GAMEPAD_DPAD_DOWN,
	BUTTON_LEFT    = XINPUT_GAMEPAD_DPAD_LEFT,
	BUTTON_RIGHT   = XINPUT_GAMEPAD_DPAD_RIGHT,
	BUTTON_BACK    = XINPUT_GAMEPAD_BACK,
	BUTTON_START   = XINPUT_GAMEPAD_START,
	BUTTON_L_STICK = XINPUT_GAMEPAD_LEFT_THUMB,
	BUTTON_R_STICK = XINPUT_GAMEPAD_RIGHT_THUMB,
	BUTTON_LB      = XINPUT_GAMEPAD_LEFT_SHOULDER,
	BUTTON_RB      = XINPUT_GAMEPAD_RIGHT_SHOULDER,
	BUTTON_A       = XINPUT_GAMEPAD_A,
	BUTTON_B       = XINPUT_GAMEPAD_B,
	BUTTON_X       = XINPUT_GAMEPAD_X,
	BUTTON_Y       = XINPUT_GAMEPAD_Y,
};

/// <summary>
/// 入力処理を行うクラス
/// </summary>
class InputManager final {
	friend class Input;
public:


	/// <summary>
	/// インスタンスの取得
	/// </summary>
	/// <returns></returns>
	static InputManager* GetInstance();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(WinApp* winApp);

	/// <summary>
	/// 終了処理
	/// </summary>
	void Finalize();

	/// <summary>
	/// 更新の最初に行う
	/// </summary>
	void Begin();

	/// <summary>
	/// imguiでデバッグ表示
	/// </summary>
	void DebugDraw(bool isDraw);


private:

	ComPtr<IDirectInput8> directInput_;
	ComPtr<IDirectInputDevice8> keyboard_;
	ComPtr<IDirectInputDevice8> mouse_;

	///- キーボードの入力状況
	BYTE keys_[256];
	BYTE preKeys_[256];

	DIMOUSESTATE2 mouseState_;
	Vec2f mousePosition_;

	XINPUT_STATE gamePad_;
	XINPUT_STATE preGamePad_;
	bool isGamePadActive_{};

private:

	InputManager() = default;
	~InputManager() = default;

	InputManager& operator=(const InputManager&) = delete;
	InputManager(const InputManager&) = delete;
	InputManager(InputManager&&) = delete;

};


/// <summary>
/// 入力の所得を行うクラス
/// </summary>
class Input final {
public:
	static bool PressKey(uint8_t keyCode) { return manager_->keys_[keyCode]; }
	static bool TriggerKey(uint8_t keyCode) { return manager_->keys_[keyCode] && !manager_->preKeys_[keyCode]; }
	static bool ReleaseKey(uint8_t keyCode) { return !manager_->keys_[keyCode] && manager_->preKeys_[keyCode]; }

	static bool PressMouse(uint8_t mouseCode) { return manager_->mouseState_.rgbButtons[mouseCode]; }
	static Vec2f GetMousePosition() { return manager_->mousePosition_; }
	static Vec2f MouseVelocity() {
		return Vec2f(static_cast<float>(manager_->mouseState_.lX), static_cast<float>(manager_->mouseState_.lY));
	}
	static float  MouseWheel() { return static_cast<float>(manager_->mouseState_.lZ); }

	static bool PressGamePad(GAMEPAD_STATE state);
	static bool TriggerGamePad(GAMEPAD_STATE state);
	static bool ReleaseGamePad(GAMEPAD_STATE state);

	static bool PressGamePadLT(int* const value);
	static bool PressGamePadRT(int* const value);
	
	static Vec2f GamePad_L_Stick();
	static Vec2f GamePad_R_Stick();
	
private:

	static InputManager* manager_;


	Input() = default;
	~Input() = default;
	Input& operator=(const Input&) = delete;
	Input(const Input&) = delete;
	Input(Input&&) = delete;
};

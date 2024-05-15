#pragma once

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <wrl/client.h>

#include <cmath>

#include "Vector2.h"

using namespace Microsoft::WRL;
class WinApp;

///- Inputクラスのインスタンス
//#define INPUT Input::GetInstance()


/// <summary>
/// 入力全般
/// </summary>
class Input final {
public:


	/// <summary>
	/// インスタンスの取得
	/// </summary>
	/// <returns></returns>
	static Input* GetInstance();



	void Initialize(WinApp* winApp);
	void Finalize();

	void Begin();


	/// -------------------------------------
	/// ↓ キーボードの入力処理
	/// -------------------------------------


	bool PressKey(uint8_t keyCode) const { return keys_[keyCode]; }
	bool TriggerKey(uint8_t keyCode) const { return keys_[keyCode] && !preKeys_[keyCode]; }
	bool ReleaseKey(uint8_t keyCode) const { return !keys_[keyCode] && preKeys_[keyCode]; }


private:

	ComPtr<IDirectInput8> directInput_;
	ComPtr<IDirectInputDevice8> keyboard_;
	ComPtr<IDirectInputDevice8> mouse_;

	///- キーボードの入力状況
	BYTE keys_[256];
	BYTE preKeys_[256];

	DIMOUSESTATE mouseState_;

private:

	Input() = default;
	~Input() = default;

	Input& operator=(const Input&) = delete;
	Input(const Input&) = delete;
	Input(Input&&) = delete;

};



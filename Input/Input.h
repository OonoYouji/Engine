#pragma once

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <wrl/client.h>

#include <cmath>

#include "Vector2.h"

using namespace Microsoft::WRL;
class WinApp;


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

	DIMOUSESTATE mouseState_;

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
private:

	static InputManager* manager_;


	Input() = default;
	~Input() = default;
	Input& operator=(const Input&) = delete;
	Input(const Input&) = delete;
	Input(Input&&) = delete;
};

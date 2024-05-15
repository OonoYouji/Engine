#pragma once

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <wrl/client.h>

#include "Vector2.h"

using namespace Microsoft::WRL;
class WinApp;


class Input final {
public:

	struct Mouse {

		///- 左ボタン
		bool leftButton = false;
		bool preLeftButton = false;

		///- 右ボタン
		bool rightButton = false;
		bool preRightButton = false;

		Vec2f position = { 0.0f,0.0f };

	};


	static Input* GetInstance();

	void Init();

	void Initialize(WinApp* winApp);
	void Finalize();

	void Begin();

	/// <summary>
	/// winAppでセットする用; 他での使用禁止
	/// </summary>
	void SetMousePos(const Vec2f& position) {
		mouse_.position = position;
	}

	void SetMouseLeftButton(bool isPush) {
		mouse_.leftButton = isPush;
	}

	void SetMouseRightButton(bool isPush) {
		mouse_.rightButton = isPush;
	}


	const Vec2f& GetMousePos() const { return mouse_.position; }
	const Mouse& GetMouse() const { return mouse_; }

	void Reset();


	BYTE* Key() { return keys_; }

private:

	Mouse mouse_;

	ComPtr<IDirectInput8> directInput_;
	ComPtr<IDirectInputDevice8> keyboard_;

	BYTE keys_[256];
	BYTE preKeys_[256];

private:

	Input() = default;
	~Input() = default;

	Input& operator=(const Input&) = delete;
	Input(const Input&) = delete;
	Input(Input&&) = delete;

};



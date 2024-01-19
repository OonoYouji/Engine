#pragma once

#include <Windows.h>
#include <cstdint>

class WinApp final {
private:

	WinApp() = default;
	~WinApp() = default;

	/// 代入演算子、コピーコンストラクタの禁止;
	WinApp(const WinApp& other) = delete;
	WinApp(WinApp&& other) = delete;
	const WinApp& operator= (const WinApp& other) = delete;

private:

	const int32_t kClientWidth_ = 1280;
	const int32_t kClientHeight_ = 720;

	WNDCLASS wc_;
	RECT wrc_;
	HWND hwnd_;
	MSG msg_;


public:

	void Initialize();

	static WinApp* GetInstance();

	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	void SetWindowClass();

	void SetWindowSize();

	void CreateGameWindow();


	UINT ProcessMessage();


};
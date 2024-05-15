#include <WinApp.h>

#include "Input.h"

#include <windowsx.h>
#include "Externals/imgui/imgui.h"
#include "Externals/imgui/imgui_impl_dx12.h"
#include "Externals/imgui/imgui_impl_win32.h"


extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);


void WinApp::CreateGameWindow(const wchar_t* title, UINT windowStyle, int sizeX, int sizeY) {

	// COM初期化
	CoInitializeEx(nullptr, COINIT_MULTITHREADED);

	windowStyle_ = windowStyle;

	SetWindowClass();
	//SetWindowSize();

	wrc_ = { 0,0,sizeX, sizeY };
	AdjustWindowRect(&wrc_, WS_OVERLAPPEDWINDOW, false);

	hwnd_ = CreateWindow(
		wc_.lpszClassName,
		title,
		windowStyle_,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		wrc_.right - wrc_.left,
		wrc_.bottom - wrc_.top,
		nullptr,
		nullptr,
		wc_.hInstance,
		nullptr
	);

	/// windowを表示
	ShowWindow(hwnd_, SW_SHOW);

}


WinApp* WinApp::GetInstance() {
	static WinApp instance;
	return &instance;
}

LRESULT WinApp::WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {

	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam)) {
		return true;
	}

	switch (msg) {
	case WM_DESTROY: // windowが破棄された
		/// OSにアプリの終了を伝える
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hwnd, msg, wparam, lparam);
}

void WinApp::SetWindowClass() {
	//// windowClassを設定する ------
	wc_.lpfnWndProc = WindowProc;
	wc_.lpszClassName = L"Engine";
	wc_.hInstance = GetModuleHandle(nullptr);
	wc_.hCursor = LoadCursor(nullptr, IDC_ARROW);

	/// windowClassを登録
	RegisterClass(&wc_);
}

UINT WinApp::ProcessMessage() {
	
	if (PeekMessage(&msg_, nullptr, 0, 0, PM_REMOVE)) {
		TranslateMessage(&msg_);
		DispatchMessage(&msg_);
	}

	/// 終了メッセージが来たら抜ける
	if (msg_.message == WM_QUIT) {
		return true;
	}

	return false;
}

void WinApp::TerminateGameWindow() {
	// ウィンドウクラスを登録解除
	UnregisterClass(wc_.lpszClassName, wc_.hInstance);

	// COM 終了
	CoUninitialize();

	CloseWindow(hwnd_);
}

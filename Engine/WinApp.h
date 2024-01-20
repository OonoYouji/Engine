#pragma once

#include <Windows.h>
#include <cstdint>

class WinApp final {
public:

	/// <summary>
	/// WinAppへのポインタを所得
	/// </summary>
	/// <returns></returns>
	static WinApp* GetInstance();

	/// windowのサイズ
	static const int kWindowWidth_ = 1280;
	static const int kWindowHeigth_ = 720;

private:

	/// Constructor & Destructor ///
	WinApp() = default;
	~WinApp() = default;

private:


	WNDCLASS wc_;
	RECT wrc_;
	HWND hwnd_;
	MSG msg_;
	UINT windowStyle_;


public:

	void CreateGameWindow(const wchar_t* title, UINT windowStyle, int sizeX, int sizeY);

	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	void SetWindowClass();

	UINT ProcessMessage();

	void TerminateGameWindow();

	HWND GetHWND() const { return hwnd_; }

private:

	/// 代入演算子、コピーコンストラクタの禁止;
	WinApp(const WinApp& other) = delete;
	WinApp(WinApp&& other) = delete;
	const WinApp& operator= (const WinApp& other) = delete;

};
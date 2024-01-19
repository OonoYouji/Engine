#pragma once

#include <Windows.h>

class WinApp final {
private:

	WinApp() = default;
	~WinApp() = default;

	/// 代入演算子、コピーコンストラクタの禁止;
	WinApp(const WinApp& other) = delete;
	WinApp(WinApp&& other) = delete;
	const WinApp& operator= (const WinApp& other) = delete;

public:

	/// <summary>
	/// インスタンス確保
	/// </summary>
	/// <returns>WinAppへのポインタ</returns>
	static WinApp* GetInstance();




};
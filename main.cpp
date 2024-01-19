#include <Windows.h>

#include "Engine/WinApp.h"


/// windowsアプリでエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	WinApp* winApp = WinApp::GetInstance();
	winApp->Initialize();
	
	while (!winApp->ProcessMessage()) {

	}

	return 0;
}

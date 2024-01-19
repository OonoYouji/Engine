#include "WinApp.h"



WinApp* WinApp::GetInstance() {
	static WinApp instance;
	return &instance;
}

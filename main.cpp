#include <Windows.h>

#include "Engine.h"


/// windowsアプリでエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	Engine::Initialize("cg2", 1280, 720);

	while (!Engine::ProcessMessage()) {

	}

	return 0;
}


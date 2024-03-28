#include <Engine.h>

#include <GameManager.h>
#include <Environment.h>

/// windowsアプリでエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	Engine::Initialize(kWindowTitle);
	GameManager::Init();

	GameManager::Run();

	GameManager::Finalize();
	Engine::Finalize();

	return 0;
}


#include <Engine.h>

const Vec2 kWindowSize = { 1280, 720 };
const std::string kWindowTitle = "大野陽司";

/// windowsアプリでエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	Engine::Initialize(kWindowTitle, kWindowSize);

	while (!Engine::ProcessMessage()) {
		Engine::BeginFrame();





		Engine::EndFrame();
	}

	Engine::Finalize();

	return 0;
}


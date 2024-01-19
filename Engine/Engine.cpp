#include "Engine.h"

#include <Windows.h>
#include <assert.h>
#include <memory>

#include "WinApp.h"

namespace {


	std::wstring ConvertString(const std::string& str) {
		if (str.empty()) {
			return std::wstring();
		}

		auto sizeNeeded = MultiByteToWideChar(
			CP_UTF8, 0, reinterpret_cast<const char*>(&str[0]), static_cast<int>(str.size()), NULL, 0);
		if (sizeNeeded == 0) {
			return std::wstring();
		}
		std::wstring result(sizeNeeded, 0);
		MultiByteToWideChar(
			CP_UTF8, 0, reinterpret_cast<const char*>(&str[0]), static_cast<int>(str.size()),
			&result[0], sizeNeeded);
		return result;
	}


	class EngineSystem {
		friend class Engine;
	private:

		WinApp* winApp_ = nullptr;

	public:
		void Initialize();

		int ProcessMessage();

	};


	void EngineSystem::Initialize() {
		winApp_ = WinApp::GetInstance();
	}

	int EngineSystem::ProcessMessage() {
		return winApp_->ProcessMessage() ? 1 : 0;
	}


	WinApp* sWinApp = nullptr;
	std::unique_ptr<EngineSystem> engineSystem = nullptr;
}




void Engine::Initialize() {

	assert(!sWinApp);

	sWinApp = WinApp::GetInstance();
	sWinApp->Initialize();

	engineSystem = std::make_unique<EngineSystem>();
	engineSystem->Initialize();

}

void Engine::Finalize() {
	engineSystem.reset();

	// ゲームウィンドウの破棄
	sWinApp->TerminateGameWindow();

}

void Engine::ConsolePrint(const std::string& message) {
	OutputDebugStringA(message.c_str());
}

int Engine::ProcessMessage() {
	return engineSystem->ProcessMessage();
}

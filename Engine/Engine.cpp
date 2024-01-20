#include <Engine.h>

#include <Windows.h>
#include <assert.h>
#include <memory>

#include <WinApp.h>
#include <DirectXCommon.h>

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
		DirectXCommon* directXCommon_ = nullptr;

	public:
		
		void Initialize();

		/// <summary>
		/// フレームの最初に行う処理
		/// </summary>
		void BeginFrame();

		/// <summary>
		/// フレームの最後に行う処理
		/// </summary>
		void EndFrame();

		/// <summary>
		/// windowが終了したかどうか
		/// </summary>
		/// <returns></returns>
		int ProcessMessage();

	};


	void EngineSystem::Initialize() {
		winApp_ = WinApp::GetInstance();
		directXCommon_ = DirectXCommon::GetInstance();
	}

	void EngineSystem::BeginFrame() {
		
		directXCommon_->PreDraw();

	}

	void EngineSystem::EndFrame() {

		directXCommon_->PostDraw();

	}

	int EngineSystem::ProcessMessage() {
		return winApp_->ProcessMessage() ? true : false;
	}


	WinApp* sWinApp = nullptr;
	DirectXCommon* sDirectXCommon = nullptr;

	std::unique_ptr<EngineSystem> sEngineSystem = nullptr;

} //// namespace




void Engine::Initialize(const std::string& title, const Vec2& windowSize) {

	assert(!sWinApp);
	assert(!sDirectXCommon);
	
	
	const int width = windowSize.x;
	const int height = windowSize.y;
	auto&& titleString = ConvertString(title);
	sWinApp = WinApp::GetInstance();
	sWinApp->CreateGameWindow(
		titleString.c_str(), 
		WS_OVERLAPPEDWINDOW & ~(WS_MAXIMIZEBOX | WS_THICKFRAME), 
		width, height
	);

	/// DirectXの初期化
	sDirectXCommon = DirectXCommon::GetInstance();
	sDirectXCommon->Initialize(sWinApp, width, height);

	sEngineSystem = std::make_unique<EngineSystem>();
	sEngineSystem->Initialize();

}

void Engine::Finalize() {
	sEngineSystem.reset();

	// ゲームウィンドウの破棄
	sWinApp->TerminateGameWindow();

}

void Engine::BeginFrame() {
	sEngineSystem->BeginFrame();
}

void Engine::EndFrame() {
	sEngineSystem->EndFrame();
}

void Engine::ConsolePrint(const std::string& message) {
	OutputDebugStringA(message.c_str());
}

void Engine::ConsolePrint(const std::wstring& message) {
	OutputDebugStringW(message.c_str());
}

int Engine::ProcessMessage() {
	return sEngineSystem->ProcessMessage();
}

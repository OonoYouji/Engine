#include <Engine.h>

#include <Windows.h>
#include <assert.h>
#include <memory>
#include <array>

#include <DirectXCommon.h>
#include <WinApp.h>
#include <PipelineStateObjectManager.h>
#include <TextureManager.h>
#include <ImGuiManager.h>
#include <Input.h>
#include <DirectionalLight.h>

#include <Environment.h>

#include <Camera.h>
#include <ModelManager.h>
#include <GameObjectManager.h>
#include <SpriteManager.h>
#include <RenderTextureManager.h>
#include <FrameTimer.h>


template<class T>
void SafeDelete(T* t) {
	delete t;
	t = nullptr;
}

namespace {

	Vector4 FloatColor(uint32_t color) {
		Vector4 colorf = {
			((color >> 24) & 0xFF) / 255.0f,
			((color >> 16) & 0xFF) / 255.0f,
			((color >> 8) & 0xFF) / 255.0f,
			((color >> 0) & 0xFF) / 255.0f
		};

		return colorf;
	}

	class EngineSystem {
		friend class Engine;
	public:

		~EngineSystem();

	private:

		WinApp* winApp_ = nullptr;
		DirectXCommon* directXCommon_ = nullptr;
		ImGuiManager* imGuiManager_ = nullptr;
		TextureManager* textureManager_ = nullptr;

		InputManager* input_ = nullptr;
		Camera* camera_ = nullptr;

		uint32_t indexTriangle_ = 0;

		struct VertexPosColor {
			Vector3 pos;   // xyz座標
			Vector4 color; // RGBA
		};

	public:

		void Initialize();

		void BeginFrame();
		void PreDraw();
		void EndFrame();
		int ProcessMessage();

		std::wstring ConvertString(const std::string& str);

		inline Camera* GetCamera();

		void Reset();

	};


	EngineSystem::~EngineSystem() {}

	void EngineSystem::Initialize() {
		camera_ = new Camera();
		winApp_ = WinApp::GetInstance();
		directXCommon_ = DirectXCommon::GetInstance();
		imGuiManager_ = ImGuiManager::GetInstance();
		textureManager_ = TextureManager::GetInstance();
		input_ = InputManager::GetInstance();
		//camera_ = std::make_unique<Camera>();
	}

	void EngineSystem::BeginFrame() {

		imGuiManager_->BeginFrame();
		input_->Begin();

		ModelManager::GetInstance()->Update();
		textureManager_->Update();

		Light::GetInstance()->Update();

		//camera_->Update();

	}

	void EngineSystem::PreDraw() {
		directXCommon_->PreDraw();
		//ModelManager::GetInstance()->PreDraw();
	}

	void EngineSystem::EndFrame() {

		directXCommon_->SetRenderTarget();
		imGuiManager_->EndFrame();
		directXCommon_->PostDraw();

	}

	int EngineSystem::ProcessMessage() {
		return winApp_->ProcessMessage() ? true : false;
	}

	std::wstring EngineSystem::ConvertString(const std::string& str) {
		if(str.empty()) {
			return std::wstring();
		}

		auto sizeNeeded = MultiByteToWideChar(
			CP_UTF8, 0, reinterpret_cast<const char*>(&str[0]), static_cast<int>(str.size()), NULL, 0);
		if(sizeNeeded == 0) {
			return std::wstring();
		}
		std::wstring result(sizeNeeded, 0);
		MultiByteToWideChar(
			CP_UTF8, 0, reinterpret_cast<const char*>(&str[0]), static_cast<int>(str.size()),
			&result[0], sizeNeeded);
		return result;
	}

	inline Camera* EngineSystem::GetCamera() {
		return camera_;
	}

	void EngineSystem::Reset() {
		indexTriangle_ = 0;
	}


	WinApp* sWinApp = nullptr;
	DirectXCommon* sDirectXCommon = nullptr;
	ImGuiManager* sImGuiManager = nullptr;
	TextureManager* sTextureManager_ = nullptr;
	InputManager* sInput = nullptr;

	std::unique_ptr<EngineSystem> sEngineSystem = nullptr;

} //// namespace




void Engine::Initialize(const std::string& title) {

	assert(!sWinApp);
	assert(!sDirectXCommon);
	assert(!sImGuiManager);
	assert(!sTextureManager_);


	const int width = kWindowSize.x;
	const int height = kWindowSize.y;
	auto&& titleString = ConvertString(title);
	sWinApp = WinApp::GetInstance();
	sWinApp->CreateGameWindow(
		titleString.c_str(),
		WS_OVERLAPPEDWINDOW & ~(WS_MAXIMIZEBOX | WS_THICKFRAME),
		width, height
	);

	///- DirectXの初期化
	sDirectXCommon = DirectXCommon::GetInstance();
	sDirectXCommon->Initialize(sWinApp);

	///- Inputの初期化
	sInput = InputManager::GetInstance();
	sInput->Initialize(sWinApp);

	///- ImGuiの初期化
	sImGuiManager = ImGuiManager::GetInstance();
	sImGuiManager->Initialize(sWinApp, sDirectXCommon);


	sEngineSystem = std::make_unique<EngineSystem>();
	sEngineSystem->Initialize();


	sTextureManager_ = TextureManager::GetInstance();
	sTextureManager_->Initialize();

	Light::GetInstance()->Init();

}

void Engine::Finalize() {
	sEngineSystem.reset();

	ModelManager::GetInstance()->Finalize();
	GameObjectManager::GetInstance()->Finalize();

	// ゲームウィンドウの破棄
	sWinApp->TerminateGameWindow();

	Light::GetInstance()->Finalize();

	sImGuiManager->Finalize();

	SpriteManager::GetInstance()->Finalize();
	sTextureManager_->Finalize();
	RenderTextureManager::GetInstance()->Finalize();

	PipelineStateObjectManager::GetInstance()->Finalize();

	sInput->Finalize();

	sDirectXCommon->Finalize();

}

void Engine::BeginFrame() {
	sEngineSystem->BeginFrame();
}

void Engine::PreDraw() {
	sEngineSystem->PreDraw();
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

std::wstring Engine::ConvertString(const std::string& string) {
	return sEngineSystem->ConvertString(string);
}

int Engine::ProcessMessage() {
	return sEngineSystem->ProcessMessage();
}

Camera* Engine::GetCamera() {
	return sEngineSystem->GetCamera();
}



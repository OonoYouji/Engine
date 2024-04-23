#include <Engine.h>

#include <Windows.h>
#include <assert.h>
#include <memory>
#include <array>

#include <Environment.h>
#include <WinApp.h>
#include <DirectXCommon.h>
#include <ImGuiManager.h>
#include <TextureManager.h>
#include <DirectionalLight.h>
#include <Input.h>
#include <Camera.h>

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

		Input* input_ = nullptr;
		//std::unique_ptr<Camera> camera_ = nullptr;
		Camera* camera_ = nullptr;

		uint32_t indexTriangle_ = 0;

		struct VertexPosColor {
			Vector3 pos;   // xyz座標
			Vector4 color; // RGBA
		};

	public:

		void Initialize();

		void BeginFrame();
		void EndFrame();
		int ProcessMessage();

		std::wstring ConvertString(const std::string& str);

		void TestDraw(const Vector4& v1, const Vector4& v2, const Vector4& v3, const Vec3f& scale, const Vec3f& rotate, Vec3f& translate);
		void TestDraw(const Matrix4x4& worldMatrix);

		void TestDraw();

		//void DrawTriangle(const Vec3f& v1, const Vec3f& v2, const Vec3f& v3, uint32_t color);

		inline Camera* GetCamera();

		void Reset();

	};


	EngineSystem::~EngineSystem() {
		SafeDelete(camera_);
	}

	void EngineSystem::Initialize() {
		camera_ = new Camera();
		winApp_ = WinApp::GetInstance();
		directXCommon_ = DirectXCommon::GetInstance();
		imGuiManager_ = ImGuiManager::GetInstance();
		textureManager_ = TextureManager::GetInstance();
		input_ = Input::GetInstance();
		//camera_ = std::make_unique<Camera>();
	}

	void EngineSystem::BeginFrame() {

		directXCommon_->PreDraw();
		imGuiManager_->BeginFrame();
		input_->Begin();

		Light::GetInstance()->Update();

		camera_->Update();

	}

	void EngineSystem::EndFrame() {

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

	void EngineSystem::TestDraw(const Vector4& v1, const Vector4& v2, const Vector4& v3, const Vec3f& scale, const Vec3f& rotate, Vec3f& translate) {
		//dxc_->TestDraw(v1, v2, v3, scale, rotate, translate);
	}

	void EngineSystem::TestDraw(const Matrix4x4& worldMatrix) {
		//directXCommon_->TestDraw(worldMatrix);
	}

	void EngineSystem::TestDraw() {
		directXCommon_->TestDraw();
	}


	inline Camera* EngineSystem::GetCamera() {
		//return camera_.get();
		return camera_;
	}

	void EngineSystem::Reset() {
		indexTriangle_ = 0;
	}


	WinApp* sWinApp = nullptr;
	DirectXCommon* sDirectXCommon = nullptr;
	ImGuiManager* sImGuiManager = nullptr;
	TextureManager* sTextureManager_ = nullptr;
	Input* sInput = nullptr;

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

	///- ImGuiの初期化
	sImGuiManager = ImGuiManager::GetInstance();
	sImGuiManager->Initialize(sWinApp, sDirectXCommon);

	///- Inputの初期化
	sInput = Input::GetInstance();
	sInput->Init();

	sEngineSystem = std::make_unique<EngineSystem>();
	sEngineSystem->Initialize();

	sTextureManager_ = TextureManager::GetInstance();
	sTextureManager_->Initialize();

	Light::GetInstance()->Init();

}

void Engine::Finalize() {
	sEngineSystem.reset();

	// ゲームウィンドウの破棄
	sWinApp->TerminateGameWindow();

	Light::GetInstance()->Finalize();

	sImGuiManager->Finalize();

	sTextureManager_->Finalize();

	sDirectXCommon->Finalize();

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

std::wstring Engine::ConvertString(const std::string& string) {
	return sEngineSystem->ConvertString(string);
}

int Engine::ProcessMessage() {
	return sEngineSystem->ProcessMessage();
}

void Engine::TestDraw(const Vector4& v1, const Vector4& v2, const Vector4& v3, const Vec3f& scale, const Vec3f& rotate, Vec3f& translate) {
	sEngineSystem->TestDraw(v1, v2, v3, scale, rotate, translate);
}

void Engine::TestDraw(const Matrix4x4& worldMatrix) {
	sEngineSystem->TestDraw(worldMatrix);
}

void Engine::TestDraw() {
	sEngineSystem->TestDraw();
}

Camera* Engine::GetCamera() {
	return sEngineSystem->GetCamera();
}



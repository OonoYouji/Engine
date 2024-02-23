#include <Engine.h>

#include <Windows.h>
#include <assert.h>
#include <memory>
#include <array>

#include <WinApp.h>
#include <DirectXCommon.h>
#include <DXCompile.h>
#include <ImGuiManager.h>
#include <Camera.h>

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
		DXCompile* dxc_ = nullptr;
		ImGuiManager* imGuiManager_ = nullptr;

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

		void TestDraw(const Vector4& v1, const Vector4& v2, const Vector4& v3);
		void TestDraw(const Vector4& v1, const Vector4& v2, const Vector4& v3, const Vec3f& scale, const Vec3f& rotate, Vec3f& translate);

		//void DrawTriangle(const Vec3f& v1, const Vec3f& v2, const Vec3f& v3, uint32_t color);

		inline Camera* GetCamera();

		void Reset();

	};


	EngineSystem::~EngineSystem() {
		SafeDelete(camera_);
	}

	void EngineSystem::Initialize() {
		winApp_ = WinApp::GetInstance();
		directXCommon_ = DirectXCommon::GetInstance();
		dxc_ = DXCompile::GetInstance();
		imGuiManager_ = ImGuiManager::GetInstance();
		//camera_ = std::make_unique<Camera>();
		camera_ = new Camera();
	}

	void EngineSystem::BeginFrame() {

		directXCommon_->PreDraw();
		imGuiManager_->BeginFrame();

	}

	void EngineSystem::EndFrame() {

		imGuiManager_->EndFrame();
		directXCommon_->PostDraw();

	}

	int EngineSystem::ProcessMessage() {
		return winApp_->ProcessMessage() ? true : false;
	}

	std::wstring EngineSystem::ConvertString(const std::string& str) {
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

	void EngineSystem::TestDraw(const Vector4& v1, const Vector4& v2, const Vector4& v3) {
		//dxc_->TestDraw(v1, v2, v3);
	}

	void EngineSystem::TestDraw(const Vector4& v1, const Vector4& v2, const Vector4& v3, const Vec3f& scale, const Vec3f& rotate, Vec3f& translate) {
		dxc_->TestDraw(v1, v2, v3, scale, rotate, translate);
	}

	//void EngineSystem::DrawTriangle(const Vec3f& v1, const Vec3f& v2, const Vec3f& v3, uint32_t color) {
	//	ID3D12GraphicsCommandList* commandList = directXCommon_->GetCommandList();

	//	// 頂点データ
	//	std::array vertices = {
	//		VertexPosColor{{v1.x, v1.y, v1.z}, {1, 1, 1, 1}},
	//		VertexPosColor{{v2.x, v2.y, v2.z}, {1, 1, 1, 1}},
	//		VertexPosColor{{v3.x, v3.y, v3.z}, {1, 1, 1, 1}},
	//	};

	//	Vector4 colorf = FloatColor(color);

	//	for (auto vertex : vertices) {
	//		vertex.color = colorf;
	//	}

	//	size_t indexVertex = indexTriangle_ * 3;

	//}

	inline Camera* EngineSystem::GetCamera() {
		//return camera_.get();
		return camera_;
	}

	void EngineSystem::Reset() {
		indexTriangle_ = 0;
	}


	WinApp* sWinApp = nullptr;
	DirectXCommon* sDirectXCommon = nullptr;
	DXCompile* sDXC = nullptr;
	ImGuiManager* sImGuiManager = nullptr;

	std::unique_ptr<EngineSystem> sEngineSystem = nullptr;

} //// namespace




void Engine::Initialize(const std::string& title, const Vec2& windowSize) {

	assert(!sWinApp);
	assert(!sDirectXCommon);
	assert(!sDXC);
	assert(!sImGuiManager);


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

	sDXC = DXCompile::GetInstance();
	sDXC->Initialize();

	sImGuiManager = ImGuiManager::GetInstance();
	sImGuiManager->Initialize(sWinApp, sDirectXCommon);

	sEngineSystem = std::make_unique<EngineSystem>();
	sEngineSystem->Initialize();

}

void Engine::Finalize() {
	sEngineSystem.reset();

	// ゲームウィンドウの破棄
	sWinApp->TerminateGameWindow();

	sImGuiManager->Finalize();

	sDXC->Finalize();
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

void Engine::TestDraw(const Vector4& v1, const Vector4& v2, const Vector4& v3) {
	sEngineSystem->TestDraw(v1, v2, v3);
}

void Engine::TestDraw(const Vector4& v1, const Vector4& v2, const Vector4& v3, const Vec3f& scale, const Vec3f& rotate, Vec3f& translate) {
	sEngineSystem->TestDraw(v1, v2, v3, scale, rotate, translate);
}

Camera* Engine::GetCamera() {
	return sEngineSystem->GetCamera();
}



#include "FrameTimer.h"

#include <ImGuiManager.h>

/// ===================================================
/// 初期化
/// ===================================================
FrameTimer* FrameTimer::GetInstance() {
	static FrameTimer instance;
	return &instance;
}


/// ===================================================
/// Frameの最初に行う
/// ===================================================
void FrameTimer::Begin() {
	frameStart_ = std::chrono::high_resolution_clock::now();
}


/// ===================================================
/// Frameの最後に行う
/// ===================================================
void FrameTimer::End() {
	auto end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<float, std::milli> duration = end - frameStart_;

	float frameTimeSecond = duration.count() / 100.0f;
	float fps = 1.0f / frameTimeSecond;

	ImGui::Begin("FrameTimer");
	ImGui::Text("frame time %f", frameTimeSecond);
	ImGui::Text("FPS %f", fps);
	ImGui::End();
}

#pragma once

#include <chrono>

class FrameTimer final {
	FrameTimer() = default;
	~FrameTimer() = default;
public:

	static FrameTimer* GetInstance();

	void Begin();
	void End();

private:

	std::chrono::high_resolution_clock::time_point frameStart_;

};
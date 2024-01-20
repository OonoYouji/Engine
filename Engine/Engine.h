#pragma once

#include <string>
#include <format>
#include <Vector2.h>

class Engine {
public:

	static void Initialize(const std::string& title, const Vec2& windowSize);
	static void Finalize();

public:

	static void ConsolePrint(const std::string& message);
	static void ConsolePrint(const std::wstring& message);

	static int ProcessMessage();

private:

	Engine(const Engine& other) = delete;
	Engine(Engine&& other) = delete;
	Engine& operator=(const Engine& other) = delete;

};

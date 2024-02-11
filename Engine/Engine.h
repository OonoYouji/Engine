#pragma once

#include <Windows.h>
#include <string>
#include <Vector2.h>
#include <SafeDelete.h>

class Engine {
public:

	static void Initialize(const std::string& title, const Vec2& windowSize);
	static void Finalize(); 

public:

	static void BeginFrame();
	static void EndFrame();


	static void ConsolePrint(const std::string& message);
	static void ConsolePrint(const std::wstring& message);
	static std::wstring ConvertString(const std::string& string);

	static int ProcessMessage();


	static void TestDraw();

private:

	Engine(const Engine& other) = delete;
	Engine(Engine&& other) = delete;
	Engine& operator=(const Engine& other) = delete;

};

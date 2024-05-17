#pragma once

#include <Windows.h>
#include <string>
#include <Vector4.h>
#include <Matrix4x4.h>

class Camera;

class Engine {
public:

	static void Initialize(const std::string& title);
	static void Finalize(); 

public:

	static void BeginFrame();
	static void EndFrame();


	static void ConsolePrint(const std::string& message);
	static void ConsolePrint(const std::wstring& message);
	static std::wstring ConvertString(const std::string& string);

	static int ProcessMessage();

	static Camera* GetCamera();

private:

	Engine(const Engine& other) = delete;
	Engine(Engine&& other) = delete;
	Engine& operator=(const Engine& other) = delete;

};

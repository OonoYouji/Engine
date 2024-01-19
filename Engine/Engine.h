#pragma once

#include <string>
#include <format>

class Engine {
public:

	static void Initialize(const std::string& title, int width, int height);
	static void Finalize();

public:

	static void ConsolePrint(const std::string& message);

	static int ProcessMessage();

private:

	Engine(const Engine& other) = delete;
	Engine(Engine&& other) = delete;
	Engine& operator=(const Engine& other) = delete;

};

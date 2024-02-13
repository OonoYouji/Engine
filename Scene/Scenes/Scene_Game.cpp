#include <Scene_Game.h>
#include <Engine.h>


Scene_Game::Scene_Game() { Init(); }
Scene_Game::~Scene_Game() { Finalize(); }



void Scene_Game::Init() {
}

void Scene_Game::Update() {
}

void Scene_Game::Draw() {


	Engine::TestDraw(
		{ -0.5f,-0.5f,0.0f,1.0f },
		{ 0.0f,0.5f,0.0f,1.0f },
		{ 0.5f,-0.5f,0.0f,1.0f }
	);

	Engine::TestDraw(
		{ 0.0f,-0.5f,0.0f,1.0f },
		{ -0.5f,0.5f,0.0f,1.0f },
		{ 0.5f,0.5f,0.0f,1.0f }
	);

}

void Scene_Game::Finalize() { }

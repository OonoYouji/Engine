#include <Scene_Game.h>
#include <Engine.h>


Scene_Game::Scene_Game() { Init(); }
Scene_Game::~Scene_Game() { Finalize(); }



void Scene_Game::Init() {

	scale_ = { 1.0f,1.0f,1.0f };
	rotate_ = { 0.0f,0.0f,0.0f };
	pos_ = { 0.0f,0.0f,0.0f };

}

void Scene_Game::Update() {

	rotate_.y += 0.03f;

}

void Scene_Game::Draw() {


	Engine::TestDraw(
		{ -0.5f,-0.5f,0.0f,1.0f },
		{ 0.0f,0.5f,0.0f,1.0f },
		{ 0.5f,-0.5f,0.0f,1.0f },
		scale_,
		rotate_,
		pos_
	);

	//Engine::TestDraw(
	//	{ 0.0f,-0.5f,0.0f,1.0f },
	//	{ -0.5f,0.5f,0.0f,1.0f },
	//	{ 0.5f,0.5f,0.0f,1.0f }
	//);

}

void Scene_Game::Finalize() { }

#include <Scene_Game.h>
#include <Engine.h>

#include <imgui.h>

Scene_Game::Scene_Game() { Init(); }
Scene_Game::~Scene_Game() { Finalize(); }



void Scene_Game::Init() {

	scale_ = { 1.0f,1.0f,1.0f };
	rotate_ = { 0.0f,0.0f,0.0f };
	pos_ = { 0.0f,0.0f,0.0f };

}

void Scene_Game::Update() {

	rotate_.y += 0.01f;
	/*ImGui::Begin("main");

	ImGui::DragFloat3("pos", &pos_.x, 0.05f);
	ImGui::DragFloat3("rotate", &rotate_.x, 0.005f);

	ImGui::End();*/

}

void Scene_Game::Draw() {

	ImGui::ShowDemoWindow();

	Engine::TestDraw(
		{ -0.5f,-0.5f,0.0f,1.0f },
		{ 0.0f,0.5f,0.0f,1.0f },
		{ 0.5f,-0.5f,0.0f,1.0f },
		scale_,
		rotate_,
		pos_
	);


	/*Engine::TestDraw(
		{ 0.0f,-0.5f,0.0f,1.0f },
		{ -0.5f,0.5f,0.0f,1.0f },
		{ 0.5f,0.5f,0.0f,1.0f },
		scale_,
		rotate_,
		pos_
	);*/


}

void Scene_Game::Finalize() { }

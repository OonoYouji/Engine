#include <Scene_Game.h>
#include <Engine.h>
#include "DirectXCommon.h"

#include <imgui.h>
#include "InputImage.h"
#include "DirectionalLight.h"


Scene_Game::Scene_Game() { Init(); }
Scene_Game::~Scene_Game() { Finalize(); }



void Scene_Game::Init() {
	Light::GetInstance()->Init();

	sphere_ = std::make_unique<Sphere>();
	sphere_->Init();

}

void Scene_Game::Update() {

	sphere_->DebugDraw();

}

void Scene_Game::Draw() {

	//ImGui::ShowDemoWindow();
	//Engine::TestDraw();


	sphere_->Draw();

}

void Scene_Game::Finalize() {

	sphere_.reset();

}

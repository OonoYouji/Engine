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

	model_ = std::make_unique<Model>();
	model_->Initialize("./Resources/Objects/Monkey", "monkey.obj");

	terrain_ = std::make_unique<Terrain>();
	terrain_->Init();

}

void Scene_Game::Update() {

	sphere_->DebugDraw();

	model_->DebugDraw("Model");

	terrain_->Update();

}

void Scene_Game::Draw() {

	//ImGui::ShowDemoWindow();
	//sphere_->Draw();
	model_->Draw();

	terrain_->Draw();

}

void Scene_Game::Finalize() {

	sphere_.reset();
	model_.reset();
	terrain_.reset();
}

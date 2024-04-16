#include <Scene_Game.h>
#include <Engine.h>
#include "DirectXCommon.h"

#include <imgui.h>

Scene_Game::Scene_Game() { Init(); }
Scene_Game::~Scene_Game() { Finalize(); }



void Scene_Game::Init() {

	terrain_ = std::make_unique<Terrain>();
	terrain_->Init();

}

void Scene_Game::Update() {

	terrain_->Update();

	
}

void Scene_Game::Draw() {

	ImGui::ShowDemoWindow();

	//Engine::TestDraw();

	terrain_->Draw();


}

void Scene_Game::Finalize() {

	terrain_.reset();

}

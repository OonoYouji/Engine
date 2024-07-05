#include <Scene_Game.h>
#include <Engine.h>
#include "DirectXCommon.h"

#include <imgui.h>
#include "DirectionalLight.h"

#include <Input.h>


Scene_Game::Scene_Game() { Init(); }
Scene_Game::~Scene_Game() { Finalize(); }



void Scene_Game::Init() {
	Light::GetInstance()->Init();

	gameObjectManager_ = GameObjectManager::GetInstance();

	(new Player())->Initialize();
	(new Object3d())->Initialize("ICO");

	LoadFile();

}

void Scene_Game::Update() {
	gameObjectManager_->Update();

	ImGui::ShowDemoWindow();


}

void Scene_Game::Draw() {
	gameObjectManager_->Draw();

}

void Scene_Game::Finalize() {


}

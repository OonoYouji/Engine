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

	model_.reset(new Model());
	model_->Initialize("./Resources/Objects/Axis", "axis.obj");

	wtf_.Initialize();

}

void Scene_Game::Update() {
	gameObjectManager_->Update();




}

void Scene_Game::Draw() {


	model_->Draw(wtf_);
}

void Scene_Game::Finalize() {

	model_.reset();

}

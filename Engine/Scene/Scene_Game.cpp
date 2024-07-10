#include <Scene_Game.h>
#include <Engine.h>
#include "DirectXCommon.h"

#include <imgui.h>
#include "DirectionalLight.h"

#include <Input.h>
#include <GameObjectFactory.h>


Scene_Game::Scene_Game() { Init(); }
Scene_Game::~Scene_Game() { Finalize(); }



void Scene_Game::Init() {
	Light::GetInstance()->Init();

	gameObjectManager_ = GameObjectManager::GetInstance();
	GameObjectFactory::GetInstance()->Initialize();

	sprite_.reset(new Sprite());
	sprite_->Initialize();

	LoadFile();

}

void Scene_Game::Update() {
	gameObjectManager_->Update();


}

void Scene_Game::Draw() {
	gameObjectManager_->Draw();

	sprite_->Draw();

}

void Scene_Game::Finalize() {
	sprite_.reset();

}

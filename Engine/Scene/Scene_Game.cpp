#include <Scene_Game.h>
#include <Engine.h>
#include "DirectXCommon.h"

#include <imgui.h>
#include "DirectionalLight.h"

#include <Input.h>
#include <GameObjectFactory.h>
#include <ModelManager.h>


Scene_Game::Scene_Game() { Init(); }
Scene_Game::~Scene_Game() { Finalize(); }



void Scene_Game::Init() {
	Light::GetInstance()->Init();

	gameObjectManager_ = GameObjectManager::GetInstance();
	GameObjectFactory::GetInstance()->Initialize();

	sprite_.reset(new Sprite());
	sprite_->Initialize();
	sprite_->SetScale({640.0f, 360.0f, 0.0f});

	LoadFile();

}

void Scene_Game::Update() {
	gameObjectManager_->Update();


}

void Scene_Game::Draw() {
	ModelManager* modelManager = ModelManager::GetInstance();
	modelManager->PreDraw();
	
	gameObjectManager_->Draw();

	modelManager->PostDraw();

	sprite_->PreDraw();
	sprite_->Draw();
	sprite_->PostDraw();

}

void Scene_Game::Finalize() {
	sprite_.reset();

}

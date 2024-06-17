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

	terrain_ = std::make_unique<Terrain>();
	terrain_->Init();

	brush_ = std::make_unique<Brush>();
	brush_->Init();

	player_ = std::make_unique<Player>();
	player_->Initialize();

	terrainCollider_ = std::make_unique<TerrainCollider>();
	terrainCollider_->SetPlayer(player_.get());
	terrainCollider_->SetTerrain(terrain_.get());
	terrainCollider_->Initialize();

	gameObjectManager_ = GameObjectManager::GetInstance();

}

void Scene_Game::Update() {
	gameObjectManager_->Update();

	brush_->Update();
	terrain_->Update();

	player_->Update();

	terrainCollider_->Update();

	player_->SetHeight(terrainCollider_->GetHeight());

}

void Scene_Game::Draw() {

	brush_->Draw();
	terrain_->Draw();

	player_->Draw();


}

void Scene_Game::Finalize() {

	terrain_.reset();
	brush_.reset();

	player_.reset();
	terrainCollider_.reset();

}

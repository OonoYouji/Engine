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

	terrain_ = std::make_unique<Terrain>();
	terrain_->Init();

	brush_ = std::make_unique<Brush>();
	brush_->Init();

	player_ = std::make_unique<Player>();
	player_->Initialize();

}

void Scene_Game::Update() {

	/*brush_->Update();
	terrain_->Update();

	player_->Update();*/

	//wtfs_[0].rotate.x += 1.0f / 64.0f;
	//wtfs_[1].rotate.y += 1.0f / 64.0f;

}

void Scene_Game::Draw() {

	/*brush_->Draw();
	terrain_->Draw();

	player_->Draw();*/


}

void Scene_Game::Finalize() {

	terrain_.reset();
	brush_.reset();

	player_.reset();

}

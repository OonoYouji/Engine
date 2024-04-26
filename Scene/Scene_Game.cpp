#include <Scene_Game.h>
#include <Engine.h>
#include "DirectXCommon.h"

#include <imgui.h>
#include "InputImage.h"


Scene_Game::Scene_Game() { Init(); }
Scene_Game::~Scene_Game() { Finalize(); }



void Scene_Game::Init() {

	terrain_ = std::make_unique<Terrain>();
	terrain_->Init();

	brush_ = std::make_unique<Brush>();
	brush_->Init();

	terrainOperator_ = std::make_unique<TerrainOperator>();
	terrainOperator_->Init(terrain_.get(), brush_.get());


	inputImage_ = InputImage::GetInstance();
	inputImage_->Initialize();

}

void Scene_Game::Update() {

	inputImage_->Update();


	terrain_->Update();
	brush_->Update();

	terrainOperator_->Update();


}

void Scene_Game::Draw() {

	//ImGui::ShowDemoWindow();

	//Engine::TestDraw();

	brush_->Draw();
	terrain_->Draw();

	//terrainOperator_->Draw();


}

void Scene_Game::Finalize() {

	terrain_.reset();
	brush_.reset();

	terrainOperator_.reset();


}

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

	sprite_ = std::make_unique<Sprite>();
	sprite_->Initialize();
	
	sphere_ = std::make_unique<Sphere>();
	sphere_->Init();

	model_ = std::make_unique<Model>();
	model_->Initialize("./Resources/Objects/Background", "Background.obj");
	WorldTransform wt;
	wt.Initialize();
	wt.translate.z = 5.0f;
	model_->SetWorldTransform(wt);
	model_->SetColor(Vec4f(0.1f, 0.1f, 0.1f, 1.0f));

	terrain_ = std::make_unique<Terrain>();
	terrain_->Init();

	brush_ = std::make_unique<Brush>();
	brush_->Init();
	
	triangleEffect_ = std::make_unique<TriangleEffect>();
	triangleEffect_->Initialize();


}

void Scene_Game::Update() {

	model_->DebugDraw("Model");
	//sprite_->DebugDraw("Sprite");

	//brush_->Update();
	//terrain_->Update();

	triangleEffect_->Update();


}

void Scene_Game::Draw() {

	//ImGui::ShowDemoWindow();
	//sphere_->Draw();
	//sprite_->Draw();
	model_->Draw();

	//brush_->Draw();
	//terrain_->Draw();

	triangleEffect_->Draw();


}

void Scene_Game::Finalize() {

	sphere_.reset();
	sprite_.reset();
	model_.reset();
	terrain_.reset();
	brush_.reset();
	triangleEffect_.reset();

}

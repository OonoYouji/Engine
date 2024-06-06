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

	
	models_.push_back(std::make_unique<Model>());
	models_.back()->Initialize("./Resources/Objects/Axis", "axis.obj");
	tags_.push_back("modelA");
	wtfs_.push_back(WorldTransform());
	wtfs_.back().Initialize();

	
	models_.push_back(std::make_unique<Model>());
	models_.back()->Initialize("./Resources/Objects/Axis", "axis.obj");
	tags_.push_back("modelB");
	wtfs_.push_back(WorldTransform());
	wtfs_.back().Initialize();

	
	meshCollider_.reset(new MeshCollider());
	meshCollider_->SetModel(models_.begin()->get());
	meshCollider_->SetWorldTransform(&wtfs_[0]);
	meshCollider_->Initialize();

	for(const auto& vertexData : models_.back()->GetVertexDatas()) {
		vertices_.push_back(Vec3f::Convert4To3(vertexData.position));
	}

}

void Scene_Game::Update() {

	ImGui::Text("IsCollision : %d", isCollision_);

	for(int i = 0; i < models_.size(); i++) {
		models_[i]->DebugDraw(tags_[i]);
		wtfs_[i].ImGuiDebug(tags_[i]);
		wtfs_[i].UpdateWorldMatrix();
	}

	/*brush_->Update();
	terrain_->Update();

	player_->Update();*/

	meshCollider_->Update();
	isCollision_ = meshCollider_->IsCollision(vertices_, wtfs_.back());

}

void Scene_Game::Draw() {

	/*brush_->Draw();
	terrain_->Draw();

	player_->Draw();*/

	for(int i = 0; i < models_.size(); i++) {
		models_[i]->Draw(wtfs_[i]);
	}

}

void Scene_Game::Finalize() {

	terrain_.reset();
	brush_.reset();

	player_.reset();

	models_.clear();

	meshCollider_.reset();
}

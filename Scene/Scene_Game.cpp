#include <Scene_Game.h>
#include <Engine.h>
#include "DirectXCommon.h"

#include <imgui.h>

Scene_Game::Scene_Game() { Init(); }
Scene_Game::~Scene_Game() { Finalize(); }



void Scene_Game::Init() {

	pos_.resize(2);

	sprite_.resize(2);
	for(auto& sprite : sprite_) {
		sprite = std::make_unique<Sprite>();
		sprite->Init();
	}

}

void Scene_Game::Update() {

	
	sprite_[0]->ImGui("spriteA");
	sprite_[1]->ImGui("spriteB");
}

void Scene_Game::Draw() {

	ImGui::ShowDemoWindow();

	Engine::TestDraw();
	//DirectXCommon::GetInstance()->DrawSprite();
	sprite_[0]->Draw();
	sprite_[1]->Draw();

}

void Scene_Game::Finalize() {
	for(auto& sprite : sprite_) {
		sprite.reset();
	}
}

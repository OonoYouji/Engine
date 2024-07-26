#include <Scene_Game.h>

#include <Engine.h>
#include "DirectXCommon.h"

#include <GameObjectFactory.h>
#include <ModelManager.h>
#include <SpriteManager.h>
#include <LineDrawer.h>
#include <ImGuiManager.h>

#include <Object2d.h>


Scene_Game::Scene_Game() { Init(); }
Scene_Game::~Scene_Game() {
	LineDrawer::GetInstance()->Finalize();
	ModelManager::GetInstance()->Finalize();
}



void Scene_Game::Init() {
	Light::GetInstance()->Init();
	InitializeRenderTex(this);

	gameObjectManager_ = GameObjectManager::GetInstance();
	GameObjectFactory::GetInstance()->Initialize();
	LineDrawer::GetInstance()->Initialize();
	ModelManager::GetInstance()->Initialize();

	Object2d* sprite = new Object2d();
	sprite->Initialize();
	sprite->SetSprite("uvChecker");
	sprite->SetScale({ 640.0f,360.0f, 1.0f });

	(new Player)->Initialize();

	LoadFile();

}

void Scene_Game::Update() {
	gameObjectManager_->Update();


}



/// ===================================================
/// 背景の描画
/// ===================================================
void Scene_Game::BackDraw() {
	ModelManager* modelManager = ModelManager::GetInstance();
	SpriteManager* spriteManager = SpriteManager::GetInstance();

	spriteManager->PreDraw();
	modelManager->PreDraw();

	gameObjectManager_->BackSpriteDraw();

	spriteManager->PostDraw();
	modelManager->PostDraw();
}


/// ===================================================
/// 3dオブジェクトの描画
/// ===================================================
void Scene_Game::ObjectDraw() {
	ModelManager* modelManager = ModelManager::GetInstance();
	LineDrawer* lineDrawer = LineDrawer::GetInstance();

	modelManager->PreDraw();
	lineDrawer->PreDraw();

	gameObjectManager_->Object3dDraw();

	modelManager->PostDraw();
	lineDrawer->PostDraw();

}



/// ===================================================
/// 前景の描画
/// ===================================================
void Scene_Game::FrontDraw() {
	ModelManager* modelManager = ModelManager::GetInstance();
	SpriteManager* spriteManager = SpriteManager::GetInstance();

	spriteManager->PreDraw();
	modelManager->PreDraw();

	gameObjectManager_->FrontSpriteDraw();

	spriteManager->PostDraw();
	modelManager->PostDraw();

}

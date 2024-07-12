#include <Scene_Game.h>

#include <Engine.h>
#include "DirectXCommon.h"

#include <GameObjectFactory.h>
#include <ModelManager.h>
#include <SpriteManager.h>
#include <LineDrawer.h>

#include <Object2d.h>


Scene_Game::Scene_Game() { Init(); }
Scene_Game::~Scene_Game() { Finalize(); }



void Scene_Game::Init() {
	Light::GetInstance()->Init();

	gameObjectManager_ = GameObjectManager::GetInstance();
	GameObjectFactory::GetInstance()->Initialize();
	LineDrawer::GetInstance()->Initialize();
	ModelManager::GetInstance()->Initialize();

	LoadFile();

}

void Scene_Game::Update() {
	gameObjectManager_->Update();


}

void Scene_Game::Draw() {
	ModelManager* modelManager = ModelManager::GetInstance();
	SpriteManager* spriteManager = SpriteManager::GetInstance();
	LineDrawer* lineDrawer = LineDrawer::GetInstance();

	/// ---------------------------------------------------
	/// 背景
	/// ---------------------------------------------------

	spriteManager->PreDraw();

	gameObjectManager_->BackSpriteDraw();

	spriteManager->PostDraw();


	/// ---------------------------------------------------
	/// 3dオブジェクト : Line
	/// ---------------------------------------------------

	modelManager->PreDraw();
	lineDrawer->PreDraw();

	gameObjectManager_->Object3dDraw();

	modelManager->PostDraw();
	lineDrawer->PostDraw();


	/// ---------------------------------------------------
	/// 前景
	/// ---------------------------------------------------

	DirectXCommon::GetInstance()->SetRenderTarget();
	spriteManager->PreDraw();

	gameObjectManager_->FrontSpriteDraw();
	//modelManager->DrawModelScreen();

	spriteManager->PostDraw();

}

void Scene_Game::Finalize() {
	LineDrawer::GetInstance()->Finalize();
	ModelManager::GetInstance()->Finalize();

}

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
	/// 3dオブジェクト
	/// ---------------------------------------------------

	modelManager->PreDraw();
	lineDrawer->PreDraw();

	gameObjectManager_->Object3dDraw();

	lineDrawer->Draw({ 0.0f,0.0f,0.0f }, { 0.0f, 0.0f, 1.0f }, { 1.0f, 0.0f, 0.0f, 1.0f });
	lineDrawer->Draw({ 0.0f,0.0f,0.0f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 1.0f, 0.0f, 1.0f });
	lineDrawer->Draw({ 0.0f,0.0f,0.0f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 1.0f, 1.0f });

	modelManager->PostDraw();
	lineDrawer->PostDraw();


	/// ---------------------------------------------------
	/// 前景
	/// ---------------------------------------------------

	spriteManager->PreDraw();

	gameObjectManager_->FrontSpriteDraw();

	spriteManager->PostDraw();

}

void Scene_Game::Finalize() {
	LineDrawer::GetInstance()->Finalize();

}

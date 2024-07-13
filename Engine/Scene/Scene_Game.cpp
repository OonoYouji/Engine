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
Scene_Game::~Scene_Game() { Finalize(); }



void Scene_Game::Init() {
	Light::GetInstance()->Init();

	gameObjectManager_ = GameObjectManager::GetInstance();
	GameObjectFactory::GetInstance()->Initialize();
	LineDrawer::GetInstance()->Initialize();
	ModelManager::GetInstance()->Initialize();

	Object3d* object = new Object3d();
	object->Initialize("Background");
	object->SetType(GameObject::Type::FrontSprite);

	LoadFile();

}

void Scene_Game::Update() {
	gameObjectManager_->Update();


}

void Scene_Game::Draw() {
	ModelManager* modelManager = ModelManager::GetInstance();
	SpriteManager* spriteManager = SpriteManager::GetInstance();
	LineDrawer* lineDrawer = LineDrawer::GetInstance();
	DirectXCommon* dxCommon_ = DirectXCommon::GetInstance();

	/// ---------------------------------------------------
	/// 背景
	/// ---------------------------------------------------

	dxCommon_->SetRenderTarget();
	spriteManager->PreDraw();
	modelManager->PreDraw();

	gameObjectManager_->BackSpriteDraw();

	spriteManager->PostDraw();
	modelManager->PostDraw();

	dxCommon_->ClearDepthBuffer();

	/// ---------------------------------------------------
	/// 3dオブジェクト : Line
	/// ---------------------------------------------------

	modelManager->RTVClear();
	modelManager->PreDraw();
	lineDrawer->PreDraw();

	gameObjectManager_->Object3dDraw();

	modelManager->PostDraw();
	modelManager->CopySRV();
	lineDrawer->PostDraw();

	dxCommon_->ClearDepthBuffer();

	/// ---------------------------------------------------
	/// 前景
	/// ---------------------------------------------------

	dxCommon_->SetRenderTarget();
	spriteManager->PreDraw();
	modelManager->PreDraw();

	gameObjectManager_->FrontSpriteDraw();
	

	spriteManager->PostDraw();
	modelManager->PostDraw();

	

}

void Scene_Game::Finalize() {
	LineDrawer::GetInstance()->Finalize();
	ModelManager::GetInstance()->Finalize();

}

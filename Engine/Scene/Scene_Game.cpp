#include <Scene_Game.h>

#include <Engine.h>
#include "DirectXCommon.h"

#include <GameObjectFactory.h>
#include <ModelManager.h>
#include <SpriteManager.h>

#include <Object2d.h>


Scene_Game::Scene_Game() { Init(); }
Scene_Game::~Scene_Game() { Finalize(); }



void Scene_Game::Init() {
	Light::GetInstance()->Init();

	gameObjectManager_ = GameObjectManager::GetInstance();
	GameObjectFactory::GetInstance()->Initialize();

	/*Object2d* front = new Object2d();
	front->Initialize();
	front->SetType(GameObject::Type::FrontSprite);

	Object2d* back = new Object2d();
	back->Initialize();
	back->SetType(GameObject::Type::BackSprite);*/

	

	LoadFile();

}

void Scene_Game::Update() {
	gameObjectManager_->Update();


}

void Scene_Game::Draw() {
	ModelManager* modelManager = ModelManager::GetInstance();
	SpriteManager* spriteManager = SpriteManager::GetInstance();


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
	
	gameObjectManager_->Object3dDraw();

	modelManager->PostDraw();


	/// ---------------------------------------------------
	/// 前景
	/// ---------------------------------------------------
	
	spriteManager->PreDraw();

	gameObjectManager_->FrontSpriteDraw();

	spriteManager->PostDraw();

}

void Scene_Game::Finalize() {

}

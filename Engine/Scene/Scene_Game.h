#pragma once

#include <IScene.h>
#include <memory>
#include <vector>
#include <list>

#include <Audio.h>
#include <Sprite.h>
#include <Model.h>
#include <GameObjectManager.h>
#include <Object3d.h>


#include <Terrain.h>
#include <Brush.h>
#include <Player.h>


/// <summary>
/// ゲームシーン
/// </summary>
class Scene_Game final
	: public IScene {
public:

	Scene_Game();
	~Scene_Game();

	void Init() override;
	void Update() override;

	void BackDraw() override;
	void ObjectDraw() override;
	void FrontDraw() override;

private:
	GameObjectManager* gameObjectManager_ = nullptr;


};
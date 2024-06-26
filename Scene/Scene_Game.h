#pragma once

#include <IScene.h>
#include <memory>
#include <vector>
#include <list>

#include <Vector3.h>

#include <Audio.h>
#include <Sprite.h>
#include <Model.h>
#include <GameObject.h>

#include <Terrain.h>
#include <Brush.h>
#include <Player.h>

#include <TerrainCollider.h>

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
	void Draw() override;
	void Finalize() override;

private:
	GameObjectManager* gameObjectManager_ = nullptr;

	std::unique_ptr<Model> model_;
	WorldTransform wtf_;

};
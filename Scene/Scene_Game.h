#pragma once

#include <IScene.h>
#include <memory>
#include <vector>
#include <list>

#include <Vector3.h>

#include <Sprite.h>
#include <Model.h>

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

	std::unique_ptr<Terrain> terrain_;
	std::unique_ptr<Brush> brush_;

	std::unique_ptr<Player> player_;

	std::unique_ptr<TerrainCollider> terrainCollider_;


};
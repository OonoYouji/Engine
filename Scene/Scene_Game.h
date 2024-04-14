#pragma once

#include <IScene.h>
#include <memory>
#include <vector>
#include <Vector3.h>
#include "2D/Sprite.h"

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

	std::vector<Vec2f> pos_;
	std::vector<std::unique_ptr<Sprite>> sprite_;

};
#pragma once

#include <IScene.h>
#include <memory>
#include <vector>
#include <Vector3.h>
#include "2D/Sprite.h"
#include "3D/Sphere.h"

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

	std::unique_ptr<Sphere> sphere_;

	std::unique_ptr<Sprite> sprite_;

};
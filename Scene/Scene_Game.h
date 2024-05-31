#pragma once

#include <IScene.h>
#include <memory>
#include <vector>
#include <Vector3.h>
#include "2D/Sprite.h"
#include "Terrain.h"

#include "Brush.h"
#include "Sphere.h"
#include "TerrainOperator.h"
#include "PerlinNoise.h"
#include "InputImage.h"
#include <Model.h>
#include <TriangleEffect.h>


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

	std::unique_ptr<Sprite> sprite_;
	std::unique_ptr<Sphere> sphere_;

	std::unique_ptr<Model> model_;
	WorldTransform modelWorldTransform_;

	std::unique_ptr<Terrain> terrain_;
	std::unique_ptr<Brush> brush_;
	std::unique_ptr<TriangleEffect> triangleEffect_;

};
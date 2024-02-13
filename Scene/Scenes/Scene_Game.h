#pragma once

#include <IScene.h>

#include <Vector3.h>

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

	Vec3f scale_;
	Vec3f rotate_;
	Vec3f pos_;

};
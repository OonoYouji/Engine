#pragma once

#include <GameObject.h>

class Sprite;

class Object2d : public GameObject {
public:

	Object2d();
	~Object2d();

	void Initialize() override;
	void Update() override;
	void Draw() override;

private:

	Sprite* sprite_ = nullptr;

};
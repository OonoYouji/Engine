#pragma once

#include <string>

#include <GameObject.h>

class Model;


class Object3d : public GameObject {
public:

	Object3d();
	~Object3d();

	void Initialize(const std::string& key);

	void Update() override;
	void Draw() override;

private:
	static int instanceCount_;

	int id_;
	Model* model_;

};
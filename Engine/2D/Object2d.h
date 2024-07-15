#pragma once

#include <GameObject.h>
#include <UvTransform.h>

class Sprite;


/// ===================================================
/// Spriteを使った2dのゲームオブジェクト
/// ===================================================
class Object2d : public GameObject {
public:

	Object2d();
	~Object2d();

	void Initialize() override;
	void Update() override;
	void Draw() override;

	void SetSprite(const std::string& textureName);

	Sprite* GetSprite() const { return sprite_; }

private:
	static int instanceId_;
	int id_ = 0;

	Sprite* sprite_ = nullptr;
	std::string textureName_;
	int convertisonType_;

	UvTransform uvTransform_;
};
#include "Object2d.h"

#include <SpriteManager.h>
#include <Sprite.h>

#include <CreateName.h>

int Object2d::instanceId_ = 0;


Object2d::Object2d() {
	SetName(CreateName(this));
	SetTag(CreateName(this));
	SetType(Type::FrontSprite);
}
Object2d::~Object2d() {}



/// ===================================================
/// 初期化
/// ===================================================
void Object2d::Initialize() {

	textureName_ = "white1x1";

	CreateTransformGroup();

	Group& uvTransform = CreateGroup("uvTransform");
	uvTransform.SetPtr("rotate", &uvTransform_.rotate);
	uvTransform.SetPtr("scale", &uvTransform_.scale);
	uvTransform.SetPtr("translate", &uvTransform_.tranalate);

	Group& textureKey = CreateGroup("tex");
	textureKey.SetPtr("key", &textureName_);
	textureKey.SetPtr("Type", &convertisonType_);
	SetType(Type(convertisonType_));

	id_ = instanceId_++;
	SetName(textureName_ + std::to_string(id_));

	sprite_ = SpriteManager::GetInstance()->GetSpritePtr(textureName_);

}


/// ===================================================
/// 更新
/// ===================================================
void Object2d::Update() {
	UpdateMatrix();
	uvTransform_.UpdateMatrix();
}


/// ===================================================
/// 描画
/// ===================================================
void Object2d::Draw() {
	sprite_->Draw(worldTransform_, uvTransform_.matTransform);
}

void Object2d::SetSprite(const std::string& textureName) {
	textureName_ = textureName;
	sprite_ = SpriteManager::GetInstance()->GetSpritePtr(textureName_);
}

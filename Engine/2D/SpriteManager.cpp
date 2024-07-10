#include "SpriteManager.h"

#include <Sprite.h>

SpriteManager* SpriteManager::GetInstance() {
	static SpriteManager instance;
	return &instance;
}

Sprite* SpriteManager::GetSpritePtr(const std::string& texturekey) {

	return nullptr;
}

void SpriteManager::PreDraw() {
	for(auto& sprite : sprites_) {
		sprite->PreDraw();
	}
}

void SpriteManager::PostDraw() {
	for(auto& sprite : sprites_) {
		sprite->PostDraw();
	}
}

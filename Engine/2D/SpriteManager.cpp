#include "SpriteManager.h"

#include <Sprite.h>
#include <DirectXCommon.h>


/// ===================================================
/// インスタンス確保
/// ===================================================
SpriteManager* SpriteManager::GetInstance() {
	static SpriteManager instance;
	return &instance;
}


/// ===================================================
/// スプライトへのポインタを取得
/// ===================================================
Sprite* SpriteManager::GetSpritePtr(const std::string& textureName) {
	if(sprites_.find(textureName) == sprites_.end()) {
		sprites_[textureName].reset(new Sprite());
		sprites_[textureName]->Initialize(textureName);
	}

	return sprites_[textureName].get();
}


/// ===================================================
/// 描画前処理
/// ===================================================
void SpriteManager::PreDraw() {
	for(auto& sprite : sprites_) {
		sprite.second->PreDraw();
	}

}


/// ===================================================
/// 描画後処理
/// ===================================================
void SpriteManager::PostDraw() {
	for(auto& sprite : sprites_) {
		sprite.second->PostDraw();
	}

	DirectXCommon::GetInstance()->ClearDepthBuffer();

}

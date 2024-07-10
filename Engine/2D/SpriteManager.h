#pragma once

#include <list>
#include <string>
#include <memory>

class Sprite;

/// <summary>
/// Spriteの管理クラス
/// </summary>
class SpriteManager final {
	SpriteManager() = default;
	~SpriteManager() = default;
public:

	static SpriteManager* GetInstance();


	Sprite* GetSpritePtr(const std::string& texturekey);

	void PreDraw();

	void PostDraw();

private:

	std::list<std::unique_ptr<Sprite>> sprites_;

};
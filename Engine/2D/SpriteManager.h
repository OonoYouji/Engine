#pragma once

#include <map>
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

	/// ===================================================
	/// public : methods
	/// ===================================================

	/// <summary>
	/// インスタンス確保
	/// </summary>
	static SpriteManager* GetInstance();

	/// <summary>
	/// 終了処理
	/// </summary>
	void Finalize();

	/// <summary>
	/// スプライトのポインタを取得
	/// </summary>
	Sprite* GetSpritePtr(const std::string& textureName);

	/// <summary>
	/// 描画前処理
	/// </summary>
	void PreDraw();

	/// <summary>
	/// 描画後処理
	/// </summary>
	void PostDraw();

private:

	std::map<std::string, std::unique_ptr<Sprite>> sprites_;

};
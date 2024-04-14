#pragma once

#include <wrl/client.h>
#include <d3d12.h>
#include <vector>
#include <string>
#include <cmath>

///- ComPtrの省略用
using namespace Microsoft::WRL;


/// <summary>
/// Textureを管理
/// </summary>
class TextureManager final {
public:

	/// <summary>
	/// テクスチャ1枚あたりのデータ
	/// </summary>
	struct Texture {
		ComPtr<ID3D12Resource> resource;
	};


	/// <summary>
	/// インスタンス確保用関数
	/// </summary>
	static TextureManager* GetInstance();


private: ///- メンバ変数

	std::vector<Texture> textures_;

private:

	TextureManager() = default;
	~TextureManager() = default;

	TextureManager& operator= (const TextureManager&) = delete;
	TextureManager(const TextureManager&) = delete;
	TextureManager(TextureManager&&) = delete;

};
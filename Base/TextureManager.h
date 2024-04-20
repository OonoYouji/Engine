#pragma once

#include <wrl/client.h>
#include <d3d12.h>
#include <DirectXTex.h>

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
		D3D12_CPU_DESCRIPTOR_HANDLE srvHandleCPU_;
		D3D12_GPU_DESCRIPTOR_HANDLE srvHandleGPU_;
	};


	/// <summary>
	/// インスタンス確保用関数
	/// </summary>
	static TextureManager* GetInstance();


	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();


	/// <summary>
	/// 終了処理
	/// </summary>
	void Finalize();


private: ///- メンバ変数

	std::vector<Texture> textures_;


	ComPtr<ID3D12Resource> textureResource_;



	/// <summary>
	/// 
	/// </summary>
	/// <param name="filePath"></param>
	void Load(const std::string& filePath);


	/// <summary>
	/// Textureデータを読む
	/// </summary>
	/// <param name="filePath"> テクスチャのファイルパス </param>
	/// <returns> mipMap付きのデータ </returns>
	DirectX::ScratchImage LoadTexture(const std::string& filePath);


	/// <summary>
	/// DirextX12のTextureResourceを作る
	/// </summary>
	ComPtr<ID3D12Resource> CreataTextureResouece(ID3D12Device* device, const DirectX::TexMetadata& metadata);


	/// <summary>
	/// TextureResourceにデータを転送する
	/// </summary>
	void UploadTextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages);

private:

	TextureManager() = default;
	~TextureManager() = default;

	TextureManager& operator= (const TextureManager&) = delete;
	TextureManager(const TextureManager&) = delete;
	TextureManager(TextureManager&&) = delete;

};
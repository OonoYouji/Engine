#pragma once

#include <wrl/client.h>
#include <d3d12.h>
#include <DirectXTex.h>

#include <vector>
#include <string>
#include <cmath>
#include <map>


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
		D3D12_CPU_DESCRIPTOR_HANDLE handleCPU;
		D3D12_GPU_DESCRIPTOR_HANDLE handleGPU;
		ComPtr<ID3D12Resource> intermediateResource;
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


	/// <summary>
	/// 
	/// </summary>
	/// <param name="filePath"></param>
	void Load(const std::string& textureName, const std::string& filePath);

	/// <summary>
	/// uav用Textureデータを読み込む
	/// </summary>
	/// <param name="textureName"></param>
	/// <param name="filePath"></param>
	void LoadUav(const std::string& textureName, const std::string& filePath);

	/// <summary>
	/// textureのセット
	/// </summary>
	/// <param name="index"></param>
	void SetGraphicsRootDescriptorTable(UINT rootParameterIndex, const std::string& textureName);

	/// <summary>
	/// textureのセット
	/// </summary>
	/// <param name="index"></param>
	void SetGraphicsRootDescriptorTableUAV(UINT rootParameterIndex, const std::string& textureName);


	const Texture& GetSrvTexture(const std::string& name);
	const Texture& GetUavTexture(const std::string& name);

	ID3D12Resource* GetUavTextureResource(const std::string& name);

private: ///- メンバ変数

	std::map<std::string, Texture> textures_;
	std::map<std::string, Texture> uavTextures_;


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
	ComPtr<ID3D12Resource> CreataTextureResoueceUAV(ID3D12Device* device, const DirectX::TexMetadata& metadata);


	/// <summary>
	/// TextureResourceにデータを転送する
	/// </summary>
	ComPtr<ID3D12Resource> UploadTextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages);
	ComPtr<ID3D12Resource> UploadTextureDataUav(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages);

	void CreateBarrier(ID3D12GraphicsCommandList* commandList, ID3D12Resource* pTexture, D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after);

private:

	TextureManager() = default;
	~TextureManager() = default;

	TextureManager& operator= (const TextureManager&) = delete;
	TextureManager(const TextureManager&) = delete;
	TextureManager(TextureManager&&) = delete;

};
#include "TextureManager.h"

#include "DirectXCommon.h"
#include <DxDescriptors.h>
#include <DxCommand.h>
#include "Engine.h"

#pragma comment(lib, "DirectXTex.lib")


/// <summary>
/// インスタンス確保
/// </summary>
TextureManager* TextureManager::GetInstance() {
	static TextureManager instance;
	return &instance;
}


/// <summary>
/// 初期化
/// </summary>
void TextureManager::Initialize() {

	const std::string baseFilePath = "./Resources/Images/";

	Load("uvChecker", baseFilePath + "uvChecker.png");
	Load("monsterBall", baseFilePath + "monsterBall.png");
	Load("dragon", baseFilePath + "dragon.png");
	Load("sprite", baseFilePath + "sprite.png");
	Load("yama", baseFilePath + "yama.png");
	Load("goku", baseFilePath + "goku.png");
	Load("clear1", baseFilePath + "clear1.png");
	Load("tileMap", baseFilePath + "tileMap.png");
	Load("GrayTexture", baseFilePath + "GrayTexture.png");

	LoadUav("uvChecker", baseFilePath + "uvChecker.png");
	LoadUav("monsterBall", baseFilePath + "monsterBall.png");
	LoadUav("dragon", baseFilePath + "dragon.png");
	LoadUav("sprite", baseFilePath + "sprite.png");
	LoadUav("yama", baseFilePath + "yama.png");
	LoadUav("goku", baseFilePath + "goku.png");
	LoadUav("clear1", baseFilePath + "clear1.png");
	LoadUav("tileMap", baseFilePath + "tileMap.png");
	LoadUav("GrayTexture", baseFilePath + "GrayTexture.png");



}


/// <summary>
/// 終了処理
/// </summary>
void TextureManager::Finalize() {

	textures_.clear();

}


/// <summary>
/// 新しいテクスチャを読み込み
/// </summary>
/// <param name="filePath"></param>
void TextureManager::Load(const std::string& textureName, const std::string& filePath) {

	if(textures_.find(textureName) != textures_.end()) {
		return;
	}

	Texture newTexture{};
	DirectX::ScratchImage mipImages = LoadTexture(filePath);
	const DirectX::TexMetadata& metadata = mipImages.GetMetadata();
	newTexture.resource = CreataTextureResouece(DirectXCommon::GetInstance()->GetDevice(), metadata);
	UploadTextureData(newTexture.resource.Get(), mipImages);

	///- metadataを基にSRVの設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = metadata.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = UINT(metadata.mipLevels);

	///- srvHandleの取得
	DxDescriptors* descriptiors = DxDescriptors::GetInstance();
	newTexture.handleCPU = descriptiors->GetCPUDescriptorHandle(descriptiors->GetSRVHeap(), descriptiors->GetSRVSize(), static_cast<uint32_t>((textures_.size() + uavTextures_.size()) + 1));
	newTexture.handleGPU = descriptiors->GetGPUDescriptorHandle(descriptiors->GetSRVHeap(), descriptiors->GetSRVSize(), static_cast<uint32_t>((textures_.size() + uavTextures_.size()) + 1));

	///- srvの生成
	DirectXCommon::GetInstance()->GetDevice()->CreateShaderResourceView(newTexture.resource.Get(), &srvDesc, newTexture.handleCPU);

	textures_[textureName] = newTexture;

}


void TextureManager::LoadUav(const std::string& textureName, const std::string& filePath) {

	if(uavTextures_.find(textureName) != uavTextures_.end()) {
		return;
	}

	Texture newTexture{};
	DirectX::ScratchImage mipImages = LoadTexture(filePath);
	const DirectX::TexMetadata& metadata = mipImages.GetMetadata();
	newTexture.resource = CreataTextureResoueceUAV(DirectXCommon::GetInstance()->GetDevice(), metadata);
	UploadTextureData(newTexture.resource.Get(), mipImages);

	///- metadataを基にSRVの設定
	D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc{};
	uavDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;			//- テクスチャのフォーマットを設定
	uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;	//- テクスチャの次元を設定
	uavDesc.Texture2D.MipSlice = 0;							//- テクスチャのミップレベルを設定

	///- uavhandleの取得
	DxDescriptors* descriptiors = DxDescriptors::GetInstance();
	newTexture.handleCPU = descriptiors->GetCPUDescriptorHandle(descriptiors->GetSRVHeap(), descriptiors->GetSRVSize(), static_cast<uint32_t>((textures_.size() + uavTextures_.size()) + 1));
	newTexture.handleGPU = descriptiors->GetGPUDescriptorHandle(descriptiors->GetSRVHeap(), descriptiors->GetSRVSize(), static_cast<uint32_t>((textures_.size() + uavTextures_.size()) + 1));

	DirectXCommon::GetInstance()->GetDevice()->CreateUnorderedAccessView(newTexture.resource.Get(), nullptr, &uavDesc, newTexture.handleCPU);

	uavTextures_[textureName] = newTexture;
}


/// <summary>
/// textureのセット
/// </summary>
/// <param name="index"></param>
void TextureManager::SetGraphicsRootDescriptorTable(UINT rootParameterIndex, const std::string& textureName) {

	/// texturesの範囲外参照しないように注意
	DxCommand::GetInstance()->GetList()->SetGraphicsRootDescriptorTable(rootParameterIndex, textures_[textureName].handleGPU);
}

void TextureManager::SetGraphicsRootDescriptorTableUAV(UINT rootParameterIndex, const std::string& textureName) {

	/// texturesの範囲外参照しないように注意
	DxCommand::GetInstance()->GetList()->SetGraphicsRootDescriptorTable(rootParameterIndex, uavTextures_[textureName].handleGPU);
}



/// <summary>
/// Textureデータの読み込み
/// </summary>
DirectX::ScratchImage TextureManager::LoadTexture(const std::string& filePath) {
	///- テクスチャファイルを読んでプログラムで扱えるようにする
	DirectX::ScratchImage image{};
	std::wstring filePathW = Engine::ConvertString(filePath);
	HRESULT result = DirectX::LoadFromWICFile(filePathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
	assert(SUCCEEDED(result));

	///- mipMapの作成
	DirectX::ScratchImage mipImages{};
	result = DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_SRGB, 0, mipImages);
	assert(SUCCEEDED(result));

	return mipImages;
}



/// <summary>
/// TextureResourceAの作成
/// </summary>
ComPtr<ID3D12Resource> TextureManager::CreataTextureResouece(ID3D12Device* device, const DirectX::TexMetadata& metadata) {


	/// --------------------------------------
	/// ↓ metadataを基にResourceの設定
	/// --------------------------------------
	D3D12_RESOURCE_DESC desc{};
	desc.Width = UINT(metadata.width);		//- textureの幅
	desc.Height = UINT(metadata.height);	//- textureの高さ
	desc.MipLevels = UINT16(metadata.mipLevels);		//- mipmapの数
	desc.DepthOrArraySize = UINT16(metadata.arraySize);	//- 奥行き or 配列Textureの配列数
	desc.Format = metadata.format;	//- TextureのFormat
	desc.SampleDesc.Count = 1;	//- サンプリングカウント; 1固定
	desc.Dimension = D3D12_RESOURCE_DIMENSION(metadata.dimension);	//- Textureの次元数
	desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

	/// --------------------------------------
	/// ↓ 利用するHeapの設定
	/// --------------------------------------

	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_CUSTOM;	//- 細かい設定を行う
	heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK; //- WriteBackポリシーでCPUアクセス可能
	heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;	//- プロセッサの近くに配置


	/// --------------------------------------
	/// ↓ Resourceを生成
	/// --------------------------------------

	ComPtr<ID3D12Resource> resource;
	HRESULT result = device->CreateCommittedResource(
		&heapProperties,		//- Heapの設定
		D3D12_HEAP_FLAG_NONE,	//- Heapの特殊な設定
		&desc,					//- Resourceの設定
		D3D12_RESOURCE_STATE_GENERIC_READ,	//- 初回のResourceState; Textureは基本読むだけ
		nullptr,				//- Clear最適値; 使わないのでnullptr
		IID_PPV_ARGS(&resource)
	);
	assert(SUCCEEDED(result));

	return resource;
}

ComPtr<ID3D12Resource> TextureManager::CreataTextureResoueceUAV(ID3D12Device* device, const DirectX::TexMetadata& metadata) {


	/// --------------------------------------
	/// ↓ metadataを基にResourceの設定
	/// --------------------------------------
	D3D12_RESOURCE_DESC desc{};
	desc.Width = UINT(metadata.width);		//- textureの幅
	desc.Height = UINT(metadata.height);	//- textureの高さ
	desc.MipLevels = UINT16(metadata.mipLevels);		//- mipmapの数
	desc.DepthOrArraySize = UINT16(metadata.arraySize);	//- 奥行き or 配列Textureの配列数
	desc.Format = metadata.format;	//- TextureのFormat
	desc.SampleDesc.Count = 1;	//- サンプリングカウント; 1固定
	desc.Dimension = D3D12_RESOURCE_DIMENSION(metadata.dimension);	//- Textureの次元数
	desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

	/// --------------------------------------
	/// ↓ 利用するHeapの設定
	/// --------------------------------------

	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_CUSTOM;	//- 細かい設定を行う
	heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK; //- WriteBackポリシーでCPUアクセス可能
	heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;	//- プロセッサの近くに配置


	/// --------------------------------------
	/// ↓ Resourceを生成
	/// --------------------------------------

	ComPtr<ID3D12Resource> resource;
	HRESULT result = device->CreateCommittedResource(
		&heapProperties,		//- Heapの設定
		D3D12_HEAP_FLAG_NONE,	//- Heapの特殊な設定
		&desc,					//- Resourceの設定
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS,	//- 初回のResourceState; Textureは基本読むだけ
		nullptr,				//- Clear最適値; 使わないのでnullptr
		IID_PPV_ARGS(&resource)
	);
	assert(SUCCEEDED(result));

	return resource;
}


/// <summary>
/// TextureResourceにデータを転送する
/// </summary>
void TextureManager::UploadTextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages) {

	///- metadataを取得
	const DirectX::TexMetadata& metadata = mipImages.GetMetadata();

	///- 全MipMapについて
	for(size_t mipLevel = 0; mipLevel < metadata.mipLevels; ++mipLevel) {

		///- MipMapLevelを指定して各Imageを取得
		const DirectX::Image* img = mipImages.GetImage(mipLevel, 0, 0);
		///- Textureに転送
		HRESULT result = texture->WriteToSubresource(
			UINT(mipLevel),
			nullptr,				//- 全領域へコピー
			img->pixels,			//- 元データへアクセス
			UINT(img->rowPitch),	//- 1ラインサイズ
			UINT(img->slicePitch)	//- 1枚サイズ
		);

		assert(SUCCEEDED(result));
	}

}




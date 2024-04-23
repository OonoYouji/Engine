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
	//Load("white1x1", baseFilePath + "white1x1.png");



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

	///- SRVを作成するDescriptorHeapの場所を決める
	//ID3D12DescriptorHeap* srvHeap = DxDescriptors::GetInstance()->GetSRVHeap();
	//newTexture.srvHandleCPU = srvHeap->GetCPUDescriptorHandleForHeapStart();
	//newTexture.srvHandleGPU = srvHeap->GetGPUDescriptorHandleForHeapStart();
	///- 先頭はImGuiが使っているのでその次を使う

	///- srvHandleの取得
	DxDescriptors* descriptiors = DxDescriptors::GetInstance();
	newTexture.srvHandleCPU = descriptiors->GetCPUDescriptorHandle(descriptiors->GetSRVHeap(), descriptiors->GetSRVSize(), static_cast<uint32_t>(textures_.size() + 1));
	newTexture.srvHandleGPU = descriptiors->GetGPUDescriptorHandle(descriptiors->GetSRVHeap(), descriptiors->GetSRVSize(), static_cast<uint32_t>(textures_.size() + 1));


	///- srvの生成
	DirectXCommon::GetInstance()->GetDevice()->CreateShaderResourceView(newTexture.resource.Get(), &srvDesc, newTexture.srvHandleCPU);

	textures_[textureName] = newTexture;

}


/// <summary>
/// textureのセット
/// </summary>
/// <param name="index"></param>
void TextureManager::SetGraphicsRootDescriptorTable(const std::string& textureName) {

	/// texturesの範囲外参照しないように注意
	DxCommand::GetInstance()->GetList()->SetGraphicsRootDescriptorTable(2, textures_[textureName].srvHandleGPU);
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



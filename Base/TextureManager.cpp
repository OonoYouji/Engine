#include "TextureManager.h"

#include <d3dx12.h>

#include <iostream>
#include <filesystem>

#include <DirectXCommon.h>
#include <DxDescriptors.h>
#include <DxCommand.h>
#include <Engine.h>

#include <ImGuiManager.h>

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

	const std::string baseFilePath = kDirectoryPath_;

	Load("white1x1", baseFilePath + "white1x1.png");
	Load("uvChecker", baseFilePath + "uvChecker.png");
	Load("monsterBall", baseFilePath + "monsterBall.png");
	Load("dragon", baseFilePath + "dragon.png");
	Load("sprite", baseFilePath + "sprite.png");
	Load("yama", baseFilePath + "yama.png");
	Load("goku", baseFilePath + "goku.png");
	Load("clear1", baseFilePath + "clear1.png");
	Load("tileMap", baseFilePath + "tileMap.png");
	Load("GrayTexture", baseFilePath + "GrayTexture.png");
	Load("CheckerBoard", baseFilePath + "checkerBoard.png");

	LoadUav("uvChecker", baseFilePath + "uvChecker.png");
	LoadUav("monsterBall", baseFilePath + "monsterBall.png");
	LoadUav("dragon", baseFilePath + "dragon.png");
	LoadUav("sprite", baseFilePath + "sprite.png");
	LoadUav("yama", baseFilePath + "yama.png");
	LoadUav("goku", baseFilePath + "goku.png");
	LoadUav("clear1", baseFilePath + "clear1.png");
	LoadUav("tileMap", baseFilePath + "tileMap.png");
	LoadUav("GrayTexture", baseFilePath + "GrayTexture.png");
	LoadUav("128x128Texture", baseFilePath + "128x128Texture.png");



}


/// <summary>
/// 終了処理
/// </summary>
void TextureManager::Finalize() {

	textures_.clear();
	uavTextures_.clear();
}

void TextureManager::Update() {

	ImGuiDebug();

}

void TextureManager::ImGuiDebug() {
#ifdef _DEBUG

	ImGui::Begin("Texture Loaded Checker");

	///- ディレクトリパスないのファイルをすべて文字列に書き込む
	std::list<std::string> filePaths;
	for(const auto& entry : std::filesystem::directory_iterator(kDirectoryPath_)) {
		if(entry.is_regular_file()) {
			std::string path = entry.path().string();
			filePaths.push_back(path);
		}
	}
	///- アルファベット順にソートする
	filePaths.sort();

	///- combo用のlistの更新
	comboList_.clear();
	int index = 0;
	for(auto& filePath : filePaths) {
		comboList_[index] = filePath;
		index++;
	}

	///- listの文字列をconbo用に変換
	std::vector<const char*> paths;
	for(const auto& pair : comboList_) {
		paths.push_back(pair.second.c_str());
	}


	///- ImGui::Comboにすべてのファイルを表示する
	static int currentNumber = 0;
	ImGui::Combo("Path", &currentNumber, paths.data(), static_cast<int>(paths.size()));
	ImGui::Separator();


	///- 選択されたFileの情報を表示する
	auto it = comboList_.find(currentNumber);
	if(it != comboList_.end()) {

		///- map<>用のkeyを計算
		std::string key = it->second.substr((kDirectoryPath_).length());
		size_t strPos = key.find(".png");
		key = key.substr(0, strPos);

		///- 読み込み済みかどうか出力
		if(textures_.find(key) != textures_.end()) {
			ImGui::Text("Loaded");
			ImGui::Separator();

			///- 読み込まれた情報を色々出す
			///////////////////////////////////////////////////////////////////

			///- Unload
			if(ImGui::Button("UNLOAD")) {


			}

		} else {
			ImGui::Text("Not Loaded");
			ImGui::Separator();

			///- Load
			if(ImGui::Button("LOAD")) {


			}

		}

	}




	ImGui::End();

#endif // _DEBUG
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
	newTexture.intermediateResource = UploadTextureData(newTexture.resource.Get(), mipImages);


	DxCommand* command = DxCommand::GetInstance();
	command->Close();
	DirectXCommon::GetInstance()->WaitExecution();
	command->ResetCommandList();
	newTexture.intermediateResource.Reset();

	///- metadataを基にSRVの設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = metadata.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = UINT(metadata.mipLevels);

	///- srvHandleの取得
	DxDescriptors* descriptiors = DxDescriptors::GetInstance();
	newTexture.handleCPU = descriptiors->GetCPUDescriptorHandle();
	newTexture.handleGPU = descriptiors->GetGPUDescriptorHandle();
	descriptiors->AddSrvUsedCount();

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
	newTexture.intermediateResource = UploadTextureDataUav(newTexture.resource.Get(), mipImages);

	DxCommand* command = DxCommand::GetInstance();
	command->Close();
	DirectXCommon::GetInstance()->WaitExecution();
	command->ResetCommandList();
	newTexture.intermediateResource.Reset();

	///- metadataを基にSRVの設定
	D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc{};
	uavDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;			//- テクスチャのフォーマットを設定
	uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;	//- テクスチャの次元を設定
	uavDesc.Texture2D.MipSlice = 0;							//- テクスチャのミップレベルを設定

	///- uavhandleの取得
	DxDescriptors* descriptiors = DxDescriptors::GetInstance();
	newTexture.handleCPU = descriptiors->GetCPUDescriptorHandle();
	newTexture.handleGPU = descriptiors->GetGPUDescriptorHandle();
	descriptiors->AddSrvUsedCount();

	DirectXCommon::GetInstance()->GetDevice()->CreateUnorderedAccessView(newTexture.resource.Get(), nullptr, &uavDesc, newTexture.handleCPU);

	uavTextures_[textureName] = newTexture;
}


/// <summary>
/// textureのセット
/// </summary>
/// <param name="index"></param>
void TextureManager::SetGraphicsRootDescriptorTable(UINT rootParameterIndex, const std::string& textureName) {
	DxDescriptors::GetInstance()->SetCommandListSrvHeap(DxCommand::GetInstance()->GetList());
	/// texturesの範囲外参照しないように注意
	DxCommand::GetInstance()->GetList()->SetGraphicsRootDescriptorTable(rootParameterIndex, textures_.at(textureName).handleGPU);
}

void TextureManager::SetGraphicsRootDescriptorTableUAV(UINT rootParameterIndex, const std::string& textureName) {
	DxDescriptors::GetInstance()->SetCommandListSrvHeap(DxCommand::GetInstance()->GetList());
	/// texturesの範囲外参照しないように注意
	DxCommand::GetInstance()->GetList()->SetGraphicsRootDescriptorTable(rootParameterIndex, uavTextures_[textureName].handleGPU);
}

const TextureManager::Texture& TextureManager::GetSrvTexture(const std::string& name) {
	return textures_.at(name);
}

const TextureManager::Texture& TextureManager::GetUavTexture(const std::string& name) {
	return uavTextures_.at(name);
}

ID3D12Resource* TextureManager::GetUavTextureResource(const std::string& name) {
	return uavTextures_.at(name).resource.Get();
}


void TextureManager::CreateBarrier(ID3D12GraphicsCommandList* commandList, ID3D12Resource* pTexture, D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after) {
	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = pTexture;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Transition.StateBefore = before;
	barrier.Transition.StateAfter = after;
	commandList->ResourceBarrier(1, &barrier);

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
/// TextureResourceの作成
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
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;	//- 細かい設定を行う


	/// --------------------------------------
	/// ↓ Resourceを生成
	/// --------------------------------------

	ComPtr<ID3D12Resource> resource;
	HRESULT result = device->CreateCommittedResource(
		&heapProperties,		//- Heapの設定
		D3D12_HEAP_FLAG_NONE,	//- Heapの特殊な設定
		&desc,					//- Resourceの設定
		D3D12_RESOURCE_STATE_COPY_DEST,	//- 初回のResourceState; Textureは基本読むだけ
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
		D3D12_RESOURCE_STATE_COPY_DEST,	//- 初回のResourceState; Textureは基本読むだけ
		nullptr,				//- Clear最適値; 使わないのでnullptr
		IID_PPV_ARGS(&resource)
	);
	assert(SUCCEEDED(result));

	return resource;
}


/// <summary>
/// TextureResourceにデータを転送する
/// </summary>
[[nodiscard]]
ComPtr<ID3D12Resource> TextureManager::UploadTextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages) {

	ID3D12Device* device = DirectXCommon::GetInstance()->GetDevice();
	ID3D12GraphicsCommandList* commandList = DxCommand::GetInstance()->GetList();

	std::vector<D3D12_SUBRESOURCE_DATA> subresources;
	DirectX::PrepareUpload(device, mipImages.GetImages(), mipImages.GetImageCount(), mipImages.GetMetadata(), subresources);
	uint64_t intermediateSize = GetRequiredIntermediateSize(texture, 0, static_cast<UINT>(subresources.size()));
	ComPtr<ID3D12Resource> intermediateResource = DirectXCommon::GetInstance()->CreateBufferResource(intermediateSize);

	UpdateSubresources(commandList, texture, intermediateResource.Get(), 0, 0, static_cast<UINT>(subresources.size()), subresources.data());
	CreateBarrier(commandList, texture, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ);

	return intermediateResource;

}

[[nodiscard]]
ComPtr<ID3D12Resource> TextureManager::UploadTextureDataUav(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages) {

	ID3D12Device* device = DirectXCommon::GetInstance()->GetDevice();
	ID3D12GraphicsCommandList* commandList = DxCommand::GetInstance()->GetList();

	std::vector<D3D12_SUBRESOURCE_DATA> subresources;
	DirectX::PrepareUpload(device, mipImages.GetImages(), mipImages.GetImageCount(), mipImages.GetMetadata(), subresources);
	uint64_t intermediateSize = GetRequiredIntermediateSize(texture, 0, static_cast<UINT>(subresources.size()));
	ComPtr<ID3D12Resource> intermediateResource = DirectXCommon::GetInstance()->CreateBufferResource(intermediateSize);

	UpdateSubresources(commandList, texture, intermediateResource.Get(), 0, 0, static_cast<UINT>(subresources.size()), subresources.data());
	CreateBarrier(commandList, texture, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

	return intermediateResource;
}




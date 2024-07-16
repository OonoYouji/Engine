#include <IScene.h>

#pragma region Include
#include <json.hpp>


#include <string>
#include <filesystem>
#include <fstream>

#include <WinApp.h>

#include <GameObjectManager.h>
#include <GameObjectFactory.h>
#include <CreateName.h>
#include <RenderTexture.h>

#include <PipelineStateObjectManager.h>
#include <DxCommand.h>
#include <DxDescriptors.h>
#include <DirectXCommon.h>

#include <ImGuiManager.h>
#include <SpriteManager.h>
#include <ModelManager.h>

#include <Sprite.h>
#include <Object2d.h>
#pragma endregion

using json = nlohmann::json;

/// ===================================================
/// 無記名名前空間
/// ===================================================
namespace {

	const std::string kDirectoryPath = "./Resources/External/";
	

} ///- namespace



/// ===================================================
/// ファイルの保存
/// ===================================================
void IScene::SaveFile() {

	std::string thisName = CreateName(this);

	json root;
	root[thisName] = json::object();

	GameObjectManager* gom = GameObjectManager::GetInstance();
	std::list<GameObject*> objects = gom->GetGameObjcetList();

	for(auto& object : objects) {

		std::string objectName = object->GetName();
		root[thisName][objectName] = json::object();
		root[thisName][objectName]["className"] = object->GetTag();
		root[thisName][objectName]["file"] = kDirectoryPath + thisName + "/Objects/" + object->GetName() + ".json";

	}

	///- ディレクトリがなければ作成する
	const std::string path = kDirectoryPath + thisName + "/";
	std::filesystem::path dir(path);
	if(!std::filesystem::exists(dir)) {
		std::filesystem::create_directory(dir);
	}

	///- File Open
	std::string filePath = path + thisName + ".json";
	std::ofstream ofs;
	ofs.open(filePath);

	if(ofs.fail()) {
		//std::string message = "Failed open data file for write";
		//MessageBoxA(nullptr, message.c_str(), "Externam Param Manager", 0);
		assert(false);
		return;
	}

	///- ファイルにjson文字列を書き込む
	ofs << std::setw(4) << root << std::endl;
	ofs.close();

}



/// ===================================================
/// ファイル読み込み
/// ===================================================
void IScene::LoadFile() {

	///- .jsonファイルを読み込んでインスタンスを生成する

	GameObjectManager* gom = GameObjectManager::GetInstance();

	///- ファイルを開く
	std::string thisName = CreateName(this);
	std::string filePath = kDirectoryPath + thisName + "/" + thisName + ".json";
	std::ifstream ifs;
	ifs.open(filePath);

	///- ファイルが開くかチェック
	if(!ifs.is_open()) {
		return;
	}

	///- json文字列からjsonのデータ構造に展開
	json root;
	ifs >> root;
	ifs.close();

	///- "Scene"ないのオブジェクトを読む
	if(root.contains(thisName)) {

		for(auto& element : root[thisName].items()) {

			std::string key = element.key();
			std::string className = element.value()["className"];
			std::string file = element.value()["file"];

			///- インスタンスがなければ作る
			if(!gom->Find(key)) {

				GameObject* object = GameObjectFactory::GetInstance()->CreateGameObject(className);

				object->CreateObejct(key);
				object->LoadFile(key, file);
				object->Initialize();


			}

		}

	}


}



void IScene::BeginRenderTarget(Target target) {
	renderTexs_[target]->CreateBarrier(D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
	renderTexs_[target]->SetRenderTarget();
	renderTexs_[target]->Clear();
}



void IScene::EndRenderTarget(Target target) {
	renderTexs_[target]->CreateBarrier(D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
}



void IScene::ImGuiDraw() {
#ifdef _DEBUG

	

	BeginRenderTarget(IScene::kImGui);

	SpriteManager::GetInstance()->PreDraw();
	screen_->Draw();
	SpriteManager::GetInstance()->PostDraw();
	
	EndRenderTarget(IScene::kImGui);

	for(auto& tex : renderTexs_) {
		tex->ImGuiImage();
	}


#endif // _DEBUG
}

void IScene::CopyScreen() {
	renderTexs_[kScreen]->CreateBarrier(D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	ID3D12GraphicsCommandList* commandList = DxCommand::GetInstance()->GetList();


	PipelineStateObjectManager::GetInstance()->SetComputeCommandList("TextureCompound", commandList);

	DxDescriptors::GetInstance()->SetCommandListSrvHeap(commandList);

	commandList->SetComputeRootDescriptorTable(0, renderTexs_[kBack]->GetTexture().handleGPU);
	commandList->SetComputeRootDescriptorTable(1, renderTexs_[k3dObject]->GetTexture().handleGPU);
	commandList->SetComputeRootDescriptorTable(2, renderTexs_[kFront]->GetTexture().handleGPU);
	commandList->SetComputeRootDescriptorTable(3, renderTexs_[kScreen]->GetTexture().handleGPU);

	commandList->Dispatch((1280 + 15) / 16, (720 + 15) / 16, 1);

	DxCommand::GetInstance()->Close();
	DirectXCommon::GetInstance()->WaitExecution();
	DxCommand::GetInstance()->ResetCommandList();


	renderTexs_[kScreen]->CreateBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_GENERIC_READ);


}



void IScene::InitializeRenderTex(IScene* thisScene) {

	screen_ = new Object2d();
	screen_->Initialize();
	screen_->SetType(GameObject::None);
	screen_->SetScale({
		static_cast<float>(WinApp::kWindowWidth_),
		static_cast<float>(WinApp::kWindowHeigth_),
		1.0f });

	for(auto& tex : renderTexs_) {
		tex = new RenderTexture();
		tex->SetSceneLink(thisScene);
	}

	renderTexs_[kScreen]->InitializeUAV(WinApp::kWindowWidth_, WinApp::kWindowHeigth_, { 0.1f,0.25f,0.5f,1.0f });
	renderTexs_[kScreen]->SetName(CreateName(thisScene));

	renderTexs_[kFront]->Initialize(WinApp::kWindowWidth_, WinApp::kWindowHeigth_, { 0.1f,0.25f,0.5f,0.0f });
	renderTexs_[kFront]->SetName(CreateName(thisScene) + "_Front");

	renderTexs_[k3dObject]->Initialize(WinApp::kWindowWidth_, WinApp::kWindowHeigth_, { 0.1f,0.25f,0.5f,0.0f });
	renderTexs_[k3dObject]->SetName(CreateName(thisScene) + "_3dObject");

	renderTexs_[kBack]->Initialize(WinApp::kWindowWidth_, WinApp::kWindowHeigth_, { 0.1f,0.25f,0.5f,0.0f });
	renderTexs_[kBack]->SetName(CreateName(thisScene) + "_Back");

	renderTexs_[kImGui]->Initialize(WinApp::kWindowWidth_, WinApp::kWindowHeigth_, { 0.1f,0.25f,0.5f,1.0f });
	renderTexs_[kImGui]->SetName(CreateName(thisScene) + "_ImGui");


	std::string name = CreateName(this) + "_Scene";
	TextureManager::GetInstance()->SetNewTexture(name, renderTexs_[kScreen]->GetTexture());
	screen_->SetSprite(name);

}

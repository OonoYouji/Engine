#include <ModelManager.h>

#pragma region Include
#include <filesystem>
#include <iostream>

#include <ImGuiManager.h>

#include <Model.h>
#include <RenderTexture.h>
#pragma endregion



/// ===================================================
/// インスタンス確保
/// ===================================================
ModelManager* ModelManager::GetInstance() {
	static ModelManager instance;
	return &instance;
}


void ModelManager::Initialize() {
	renderTex_.reset(new RenderTexture());
	renderTex_->InitializeOffScreen(1280, 720);
	screen_ = new Object2d();
	screen_->Initialize();
	screen_->SetType(GameObject::Type::FrontSprite);
	screen_->SetScale({ 640.0f, 360.0f, 1.0f });
}



/// ===================================================
/// 終了処理
/// ===================================================
void ModelManager::Finalize() {
	models_.clear();
	renderTex_.reset();
}



/// ===================================================
/// 描画前処理
/// ===================================================
void ModelManager::PreDraw() {

	renderTex_->CreateBarrier(D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
	renderTex_->SetRenderTarget();
	renderTex_->Clear({1.0f, 0.0f, 0.0f, 1.0f});

	for(auto& model : models_) {
		model.second->PreDraw();
	}
}



/// ===================================================
/// 描画後処理
/// ===================================================
void ModelManager::PostDraw() {

	for(auto& model : models_) {
		model.second->PostDraw();
	}



	renderTex_->CopyBuffer();
	TextureManager::GetInstance()->SetNewTexture("modelScreen", renderTex_->GetTexture());
	screen_->SetSprite("modelScreen");

	renderTex_->CreateBarrier(D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);

}

/// ===================================================
/// 更新処理
/// ===================================================
void ModelManager::Update() {

	ImGuiDebug();

}


/// ===================================================
/// モデルへのポインタを返す
/// ===================================================
Model* ModelManager::GetModelPtr(const std::string& key) {
	///- 読み込み済みじゃなければ生成する
	if(models_.find(key) == models_.end()) {
		Create(key + ".obj");
	}

	///- ポインタを返す
	return models_.at(key).get();
}




/// ===================================================
/// モデルを作成してポインタを返す
/// ===================================================
Model* ModelManager::Create(const std::string& fileName) {
	size_t pos = fileName.find(".obj");
	std::string key = fileName.substr(0, pos);

	///- すでに読み込み済みであればそのモデルのポインタを返す
	if(models_.find(key) != models_.end()) {
		return models_.at(key).get();
	}

	///- 読み込んでからそのモデルへのポインタを返す
	CreateModel(directoryPath_ + key, fileName, key);
	return models_.at(key).get();
}



/// ===================================================
/// ImGuiでデバッグ表示
/// ===================================================
void ModelManager::ImGuiDebug() {
#ifdef _DEBUG

	/// ---------------------------------------------------
	/// ファイル探索をしてファイルの文字列をlistに格納する
	/// ---------------------------------------------------
	std::list<std::string> filePaths;
	for(const auto& entry : std::filesystem::directory_iterator(directoryPath_)) {
		if(entry.is_directory()) {
			std::string path = entry.path().string();
			filePaths.push_back(path);
		}
	}

	///- アルファベット順にソートする
	filePaths.sort();

	///- ペアの更新
	pairs_.clear();
	int index = 0;
	for(auto& filePath : filePaths) {
		pairs_[index] = filePath;
		index++;
	}



	/// ---------------------------------------------------
	/// ImGuiに色々表示
	/// ---------------------------------------------------
	ImGui::Begin("Model Loaded Checker");

	///- pairのstringを格納してComboに使用する
	std::vector<const char*> itemNames;
	for(const auto& pair : pairs_) {
		itemNames.push_back(pair.second.c_str());
	}

	///- Comboでプルダウンメニューの表示
	static int currentNumber = 0;
	ImGui::Combo("Path", &currentNumber, itemNames.data(), static_cast<int>(itemNames.size()));
	ImGui::Separator();

	///- 選択されたModelの探索
	auto it = pairs_.find(currentNumber);
	if(it != pairs_.end()) {
		
		///- map<>用のkeyを計算
		std::string key = it->second.substr((directoryPath_).length());


		/// ---------------------------------------------------
		/// 読み込み済みかどうか表示 : 読み込まれたモデルの情報を表示
		/// ---------------------------------------------------
		if(models_.find(key) != models_.end()) {
			ImGui::Text("Loaded");
			ImGui::Separator();

			///- 読み込まれた情報を色々出す
			///////////////////////////////////////////////////////////////////

			///- Unload
			if(ImGui::Button("UNLOAD")) {
				models_.erase(key);
			}

		} else {
			ImGui::Text("Not Loaded");
			ImGui::Separator();

			///- Load
			if(ImGui::Button("LOAD")) {
				Create(key + ".obj");
			}

		}

	}

	ImGui::End();

#endif // _DEBUG
}



/// ===================================================
/// モデルの作成
/// ===================================================
void ModelManager::CreateModel(const std::string& directoryPath, const std::string& fileName, const std::string& key) {

	models_[key] = std::make_unique<Model>();
	models_.at(key)->Initialize(directoryPath, fileName);

}

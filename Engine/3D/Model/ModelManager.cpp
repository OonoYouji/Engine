#include <ModelManager.h>

#include <filesystem>
#include <iostream>

#include <ImGuiManager.h>

#include <Model.h>

/// ================================================================================================== ///
/// ↓ ModelManager Methods
/// ================================================================================================== ///


ModelManager* ModelManager::GetInstance() {
	static ModelManager instance;
	return &instance;
}

void ModelManager::Finalize() {
	models_.clear();
}

void ModelManager::PreDraw() {
	for(auto& model : models_) {
		model.second->Reset();
	}
}

void ModelManager::Update() {
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

	ImGuiDebug();

}

Model* ModelManager::GetModelPtr(const std::string& key) {
	if(models_.find(key) == models_.end()) {
		Create(key + ".obj");
	}

	return models_.at(key).get();
}

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

void ModelManager::ImGuiDebug() {
#ifdef _DEBUG

	ImGui::Begin("Model Loaded Checker");

	static int currentNumber = 0;

	std::vector<const char*> itemNames;
	for(const auto& pair : pairs_) {
		itemNames.push_back(pair.second.c_str());
	}

	ImGui::Combo("Path", &currentNumber, itemNames.data(), static_cast<int>(itemNames.size()));
	ImGui::Separator();

	///- 選択されたModelの探索
	auto it = pairs_.find(currentNumber);
	if(it != pairs_.end()) {

		///- map<>用のkeyを計算
		std::string key = it->second.substr((directoryPath_).length());

		///- 読み込み済みかどうか出力
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

void ModelManager::CreateModel(const std::string& directoryPath, const std::string& fileName, const std::string& key) {

	models_[key] = std::make_unique<Model>();
	models_.at(key)->Initialize(directoryPath, fileName);

}

#include <ExternalParamManager.h>

#include <fstream>

#include <ImGuiManager.h>


const std::string ExternalParamManager::kDirectoryPath_ = "./Resources/External/";


////////////////////////////////////////////////////////////////////////////
///
/// group   member methods
///
////////////////////////////////////////////////////////////////////////////


/// ===================================================
/// 各Itemのデバッグ表示
/// ===================================================
void ExternalParamManager::Group::ImGuiDebug() {
	for(auto& item : items) {

		Item_first& first = item.second.first;
		Item_second& second = item.second.second;

		///- int
		if(std::holds_alternative<int>(second)) {
			int* ptr = std::get_if<int>(&second);
			ImGui::DragInt(item.first.c_str(), ptr, 1);
			///- 値を変えたらptrにも適用する
			if(ImGui::IsItemEdited()) {
				int* ptr = std::get<int*>(first);
				if(ptr) {
					*ptr = std::get<int>(second);
				}
			}
			continue;
		}

		///- float
		if(std::holds_alternative<float>(second)) {
			float* ptr = std::get_if<float>(&second);
			ImGui::DragFloat(item.first.c_str(), ptr, 0.05f);
			///- 値を変えたらptrにも適用する
			if(ImGui::IsItemEdited()) {
				float* ptr = std::get<float*>(first);
				if(ptr) {
					*ptr = std::get<float>(second);
				}
			}
			continue;
		}

		///- vector3
		if(std::holds_alternative<Vector3>(second)) {
			Vector3* ptr = std::get_if<Vector3>(&second);
			ImGui::DragFloat3(item.first.c_str(), &ptr->x, 0.05f);
			///- 値を変えたらptrにも適用する
			if(ImGui::IsItemEdited()) {
				Vector3* ptr = std::get<Vector3*>(first);
				if(ptr) {
					*ptr = std::get<Vector3>(second);
				}
			}
			continue;
		}

	}
}



////////////////////////////////////////////////////////////////////////////
///
/// category   member methods
///
////////////////////////////////////////////////////////////////////////////



/// ===================================================
/// グループのセット
/// ===================================================
Epm::Group& Epm::Category::CraeteGroup(const std::string& key) {
	for(auto& group : groups) {
		if(group.first == key) {
			return group.second;
		}
	}
	groups.push_back(pair(key, Group()));
	return groups.back().second;
}


/// ===================================================
/// グループのゲット
/// ===================================================
Epm::Group& Epm::Category::GetGroup(const std::string& key) {
	Group* result = nullptr;
	for(auto& group : groups) {
		if(group.first == key) {
			result = &group.second;
		}
	}
	assert(result); //- nullptrチェック
	return *result;
}


/// ===================================================
/// ファイルに書き出し
/// ===================================================
void ExternalParamManager::Category::SaveFile(json& root, const std::string& categoryName) {
	//json r = root[categoryName];

	///- rに値を保存していく
	for(auto& group : groups) {
		root[categoryName][group.first] = json::object();

		for(auto& itemIt : group.second.items) {

			const std::string& name = itemIt.first;
			const Item_second& item = itemIt.second.second;

			///- int
			if(std::holds_alternative<int>(item)) {
				root[categoryName][group.first][name] = std::get<int>(item);
				continue;
			}

			///- float
			if(std::holds_alternative<float>(item)) {
				root[categoryName][group.first][name] = std::get<float>(item);
				continue;
			}

			///- Vector3
			if(std::holds_alternative<Vector3>(item)) {
				Vec3f value = std::get<Vector3>(item);
				root[categoryName][group.first][name] = json::array({ value.x, value.y, value.z });
				continue;
			}

		}

	}

	///- ディレクトリがなければ作成する
	std::filesystem::path dir(Epm::kDirectoryPath_);
	if(!std::filesystem::exists(dir)) {
		std::filesystem::create_directory(dir);
	}

	///- File Open
	std::string filePath = kDirectoryPath_ + categoryName + ".json";
	std::ofstream ofs;
	ofs.open(filePath);

	if(ofs.fail()) {
		std::string message = "Failed open data file for write";
		MessageBoxA(nullptr, message.c_str(), "Externam Param Manager", 0);
		assert(false);
		return;
	}

	///- ファイルにjson文字列を書き込む
	ofs << std::setw(4) << root << std::endl;
	ofs.close();

}


/// ===================================================
/// 各Groupのデバッグ表示
/// ===================================================
void Epm::Category::ImGuiDebug() {
	for(auto& group : groups) {
		if(ImGui::TreeNodeEx(group.first.c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
			group.second.ImGuiDebug();
			ImGui::TreePop();
		}
		ImGui::Separator();
	}
}



////////////////////////////////////////////////////////////////////////////
///
/// external param manager   member methods
///
////////////////////////////////////////////////////////////////////////////



/// ===================================================
/// インスタンス確保
/// ===================================================
ExternalParamManager* ExternalParamManager::GetInstance() {
	static ExternalParamManager instance;
	return &instance;
}


/// ===================================================
/// インスタンス確保
/// ===================================================
void ExternalParamManager::Initialize() {
	LoadFiles();
}


/// ===================================================
/// カテゴリーの生成
/// ===================================================
Epm::Category& ExternalParamManager::CreateCategory(const std::string& key) {
	return datas_[key];
}


/// ===================================================
/// カテゴリーのゲット
/// ===================================================
Epm::Category& ExternalParamManager::GetCategory(const std::string& key) {
	return datas_.at(key);
}


/// ===================================================
/// カテゴリーのゲット
/// ===================================================
void ExternalParamManager::SaveFile(const std::string& categoryName) {

	///- categoryがあるか確認
	auto it = datas_.find(categoryName);
	if(it == datas_.end()) { return; }

	json root;
	root = json::object();

	///- カテゴリーの登録
	root[categoryName] = json::object();
	it->second.SaveFile(root, categoryName);


}


/// ===================================================
/// ファイルの読み込み
/// ===================================================
void ExternalParamManager::LoadFile(const std::string& categoryName) {

	///- ファイルを開く
	std::string filePath = kDirectoryPath_ + categoryName + ".json";
	std::ifstream ifs;
	ifs.open(filePath);

	///- ファイルが開けなければ
	if(!ifs.is_open()) {
		std::string message = "File could nor be opened";
		MessageBoxA(nullptr, message.c_str(), "External Param Manager", 0);
		assert(false);
		return;
	}

	///- json文字列からjsonのデータ構造に展開
	json root;
	ifs >> root;
	ifs.close();

	///- カテゴリー探索
	json::iterator itCategory = root.find(categoryName);
	Category* category = &CreateCategory(categoryName);

	///- 未登録チェック
	assert(itCategory != root.end());

	///- 各グループ
	for(json::iterator itGroup = itCategory->begin(); itGroup != itCategory->end(); ++itGroup) {

		///- グループ名の取得
		const std::string& groupName = itGroup.key();
		Group* group = &category->CraeteGroup(groupName);

		///- 各アイテム
		for(json::iterator itItem = itGroup->begin(); itItem != itGroup->end(); ++itItem) {

			///- アイテム名の取得
			const std::string& itemName = itItem.key();

			///- int
			if(itItem->is_number_integer()) {
				int value = itItem->get<int>();
				group->SetValue(itemName, value);
				continue;
			}

			///- float
			if(itItem->is_number_float()) {
				double value = itItem->get<double>();
				group->SetValue(itemName, static_cast<float>(value));
				continue;
			}

			///- Vector3
			if(itItem->is_array() && itItem->size() == 3) {
				Vec3f value = { itItem->at(0), itItem->at(1), itItem->at(2) };
				group->SetValue(itemName, value);
				continue;
			}

		}

	}


}


/// ===================================================
/// ファイルの読み込み
/// ===================================================
void ExternalParamManager::LoadFiles() {

	///- 読み込み先がなければ return
	std::filesystem::path dir(kDirectoryPath_);
	if(!std::filesystem::exists(dir)) {
		return;
	}

	std::filesystem::directory_iterator dir_it(kDirectoryPath_);
	for(const auto& entry : dir_it) {

		///- ファイルパスを取得
		const std::filesystem::path& filePath = entry.path();

		///- ファイル拡張子を取得
		std::string extersion = filePath.extension().string();
		///- .json以外はスキップ
		if(extersion.compare(".json") != 0) {
			continue;
		}

		LoadFile(filePath.stem().string());

	}


}



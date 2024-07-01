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

		///- int
		if(std::holds_alternative<int*>(item.second)) {
			int** ptr = std::get_if<int*>(&item.second);
			ImGui::DragInt(item.first.c_str(), *ptr, 1);
			continue;
		}

		///- float
		if(std::holds_alternative<float*>(item.second)) {
			float** ptr = std::get_if<float*>(&item.second);
			ImGui::DragFloat(item.first.c_str(), *ptr, 0.05f);
			continue;
		}

		///- vector3
		if(std::holds_alternative<Vector3*>(item.second)) {
			Vector3** ptr = std::get_if<Vector3*>(&item.second);
			ImGui::DragFloat3(item.first.c_str(), &(**ptr).x, 0.05f);
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
			const Item& item = itemIt.second;

			///- int
			if(std::holds_alternative<int*>(item)) {
				root[categoryName][group.first][name] = *std::get<int*>(item);
				continue;
			}

			///- float
			if(std::holds_alternative<float*>(item)) {
				root[categoryName][group.first][name] = *std::get<float*>(item);
				continue;
			}

			///- Vector3
			if(std::holds_alternative<Vector3*>(item)) {
				Vec3f value = *std::get<Vector3*>(item);
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



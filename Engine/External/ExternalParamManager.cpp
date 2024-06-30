#include <ExternalParamManager.h>

#include <ImGuiManager.h>


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
void Epm::Category::SetGroup(const std::string& key) {
	groups[key];
}


/// ===================================================
/// グループのゲット
/// ===================================================
Epm::Group& Epm::Category::GetGroup(const std::string& key) {
	return groups.at(key);
}


/// ===================================================
/// 各Groupのデバッグ表示
/// ===================================================
void Epm::Category::ImGuiDebug() {
	for(auto& group : groups) {
		if(!ImGui::TreeNodeEx(group.first.c_str())) {
			continue;
		}

		group.second.ImGuiDebug();

		ImGui::TreePop();
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



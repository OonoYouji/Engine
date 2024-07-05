#include <ExternalParamManager.h>

#include <fstream>

#include <ImGuiManager.h>





/// /////////////////////////////////////////////////////////////////////
/// 
/// struct Group methods
/// 
/// /////////////////////////////////////////////////////////////////////


template<typename T>
void Epm::Group::SetValue(const std::string& key, const T& value) {
	///- 探索
	auto it = items.find(key);

	///- なかったら
	if(it == items.end()) {
		items[key];
		items.at(key).variable.first = static_cast<T*>(nullptr);
	}

	items.at(key).variable.second = value;

}

template<typename T>
void Epm::Group::SetPtr(const std::string& key, T* ptr) {
	///- 探索
	auto it = items.find(key);

	///- なかったら
	if(it == items.end()) {
		items[key];
		items.at(key).variable.second = *ptr;
	}

	items.at(key).variable.first = ptr;

}


/// ---------------------------------------------------
/// テンプレート関数の明示的な具体化
/// ---------------------------------------------------
template void Epm::Group::SetValue<int>(const std::string& key, const int& value);
template void Epm::Group::SetValue<float>(const std::string& key, const float& value);
template void Epm::Group::SetValue<Vec3f>(const std::string& key, const Vec3f& value);
template void Epm::Group::SetValue<bool>(const std::string& key, const bool& value);
template void Epm::Group::SetValue<std::string>(const std::string& key, const std::string& value);

template void Epm::Group::SetPtr<int>(const std::string& key, int* value);
template void Epm::Group::SetPtr<float>(const std::string& key, float* value);
template void Epm::Group::SetPtr<Vec3f>(const std::string& key, Vec3f* value);
template void Epm::Group::SetPtr<bool>(const std::string& key, bool* value);
template void Epm::Group::SetPtr<std::string>(const std::string& key, std::string* value);


/// /////////////////////////////////////////////////////////////////////
/// 
/// struct Object methods
/// 
/// /////////////////////////////////////////////////////////////////////


Epm::Group& Epm::Object::CreateGroup(const std::string& groupName) {
	///- 探索
	for(auto& group : groups) {
		if(group.first == groupName) {
			return group.second;
		}
	}
	groups.push_back(std::make_pair(groupName, Group()));
	return groups.back().second;
}

void Epm::Object::ImGuiDebug() {

	/*ImGui::SetNextItemOpen(true, ImGuiCond_Always);
	if(!ImGui::TreeNodeEx(name.c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
		return;
	}*/

	for(auto& group : groups) {
		if(ImGui::TreeNodeEx(group.first.c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {

			for(auto& item : group.second.items) {

				Item::Ptr& item_ptr = item.second.variable.first;
				Item::Value& item_value = item.second.variable.second;

				///- int
				if(std::holds_alternative<int>(item_value)) {
					int* v = std::get_if<int>(&item_value);
					ImGui::DragInt(item.first.c_str(), v, 1);
					if(ImGui::IsItemEdited()) {
						int* ptr = std::get<int*>(item_ptr);
						if(ptr) {
							*ptr = std::get<int>(item_value);
						}
					}
					continue;
				}

				///- float
				if(std::holds_alternative<float>(item_value)) {
					float* ptr = std::get_if<float>(&item_value);
					ImGui::DragFloat(item.first.c_str(), ptr, 0.01f);
					if(ImGui::IsItemEdited()) {
						float* ptr = std::get<float*>(item_ptr);
						if(ptr) {
							*ptr = std::get<float>(item_value);
						}
					}
					continue;
				}

				///- vector3
				if(std::holds_alternative<Vec3f>(item_value)) {
					Vec3f* ptr = std::get_if<Vec3f>(&item_value);
					ImGui::DragFloat3(item.first.c_str(), &(*ptr).x, 0.01f);
					if(ImGui::IsItemEdited()) {
						Vec3f* ptr = std::get<Vec3f*>(item_ptr);
						if(ptr) {
							*ptr = std::get<Vec3f>(item_value);
						}
					}
					continue;
				}

				///- bool
				if(std::holds_alternative<bool>(item_value)) {
					bool* ptr = std::get_if<bool>(&item_value);
					ImGui::Checkbox(item.first.c_str(), ptr);
					if(ImGui::IsItemEdited()) {
						bool* ptr = std::get<bool*>(item_ptr);
						if(ptr) {
							*ptr = std::get<bool>(item_value);
						}
					}
					continue;
				}

				///- string
				if(std::holds_alternative<std::string>(item_value)) {

					continue;
				}
			}

			ImGui::TreePop();
		}
	}


	//ImGui::TreePop();


}



/// /////////////////////////////////////////////////////////////////////
/// 
/// struct ObjectType methods
/// 
/// /////////////////////////////////////////////////////////////////////


Epm::Object* Epm::ObjectType::CreateObject(const std::string& objectName) {
	///- 探索
	auto it = objects.find(objectName);

	///- なかったら
	if(it == objects.end()) {
		objects[objectName];
		objects[objectName].name = objectName;
	}

	return &objects.at(objectName);
}


/// /////////////////////////////////////////////////////////////////////
/// 
/// class epm methods
/// 
/// /////////////////////////////////////////////////////////////////////


Epm* Epm::GetInstance() {
	static Epm instance;
	return &instance;
}

Epm::ObjectType* Epm::CreateObjectType(const std::string& key) {
	return &datas_[key];
}

void Epm::SaveFiles() {
	///- すべてのファイルを保存

	for(auto& objectType : datas_) {
		SaveFile(objectType.first);
	}

}

void Epm::SaveFile(const std::string& objectTypeName) {
	///- .jsonファイル形式で保存

	///- 探索
	auto itr = datas_.find(objectTypeName);

	///- ないとき
	if(itr == datas_.end()) {
		return;
	}

	json root;
	root[objectTypeName] = json::object();

	///- インスタンスごと
	for(auto& object : datas_.at(objectTypeName).objects) {

		root[objectTypeName][object.first] = json::object();

		///- グループごと
		for(auto& group : object.second.groups) {

			root[objectTypeName][object.first][group.first] = json::object();

			///- 変数ごと
			for(auto& item : group.second.items) {

				///- int
				if(std::holds_alternative<int>(item.second.variable.second)) {

					root[objectTypeName][object.first][group.first][item.first] = 
						std::get<int>(item.second.variable.second);
					continue;
				}

				///- float
				if(std::holds_alternative<float>(item.second.variable.second)) {

					root[objectTypeName][object.first][group.first][item.first] = 
						std::get<float>(item.second.variable.second);
					continue;
				}

				///- vector3
				if(std::holds_alternative<Vec3f>(item.second.variable.second)) {

					Vec3f value = std::get<Vector3>(item.second.variable.second);
					root[objectTypeName][object.first][group.first][item.first] =
						json::array({ value.x, value.y, value.z });
					continue;
				}

				///- bool
				if(std::holds_alternative<bool>(item.second.variable.second)) {

					root[objectTypeName][object.first][group.first][item.first] =
						std::get<bool>(item.second.variable.second);
					continue;
				}

				///- string
				if(std::holds_alternative<std::string>(item.second.variable.second)) {

					root[objectTypeName][object.first][group.first][item.first] = 
						std::get<std::string>(item.second.variable.second);
					continue;
				}


			}

		}

	}


	///- ディレクトリがなければ作成する
	std::filesystem::path dir(Epm::kDirectoryPath_);
	if(!std::filesystem::exists(dir)) {
		std::filesystem::create_directory(dir);
	}

	///- File Open
	std::string filePath = kDirectoryPath_ + objectTypeName + ".json";
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

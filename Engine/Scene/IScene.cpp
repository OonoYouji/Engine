#include <IScene.h>

#pragma region Include
#include <json.hpp>


#include <string>
#include <filesystem>
#include <fstream>

#include <GameObjectManager.h>
#include <CreateName.h>
#pragma endregion


namespace {

	const std::string kDirectoryPath = "./Resources/External/";

} ///- namespace

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

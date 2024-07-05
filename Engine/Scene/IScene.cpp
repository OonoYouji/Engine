#include <IScene.h>

#pragma region Include
#include <json.hpp>


#include <string>
#include <filesystem>
#include <fstream>

#include <GameObjectManager.h>
#include <CreateName.h>
#pragma endregion

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


			}

		}

	}


}

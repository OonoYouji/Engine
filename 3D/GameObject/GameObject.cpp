#include <GameObject.h>

#include <ImGuiManager.h>


///- クラスの名前をstringに変換するsample code
//#include <iostream>
//#include <typeinfo>
//#include <string>
//std::string className = typeid(this).name();


void GameObject::SetTag(const std::string& tag) {
	tag_ = tag;
}



void GameObject::ImGuiDebug() {
#ifdef _DEBUG
	ImGui::Begin(tag_.c_str());



	ImGui::End();
#endif // _DEBUG
}




///// --------------------------------------------------------------------------
///// ↓ GameObjectManager class methods
///// --------------------------------------------------------------------------


GameObjectManager* GameObjectManager::GetInstance() {
	static GameObjectManager instance;
	return &instance;
}

void GameObjectManager::Update() {
	ImGuiDebug();
}

void GameObjectManager::AddGameObject(GameObject* gameObject) {
	pGameObjects_.push_back(gameObject);
}

void GameObjectManager::ClearList() {
	pGameObjects_.clear();
}

void GameObjectManager::ImGuiDebug() {
	for(auto& gameObject : pGameObjects_) {
		gameObject->ImGuiDebug();
	}
}

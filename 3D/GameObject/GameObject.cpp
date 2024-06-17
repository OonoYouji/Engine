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



const std::string& GameObject::GetTag() const {
	return tag_;
}

void GameObject::ImGuiDebug() {
	if(!ImGui::TreeNodeEx(tag_.c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
		return;
	}

	worldTransform_.ImGuiTreeNodeDebug();


	ImGui::TreePop();
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
	ImGui::Begin("Hierarchy");
	for(auto& gameObject : pGameObjects_) {
		if(ImGui::Selectable(gameObject->GetTag().c_str(), selectObject_ == gameObject)) {
			selectObject_ = gameObject;
		}
	}
	ImGui::End();
	
	ImGui::Begin("Inspector");

	if(selectObject_) {
		selectObject_->ImGuiDebug();
	}

	ImGui::End();
}

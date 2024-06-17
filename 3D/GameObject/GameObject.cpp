#include <GameObject.h>

#include <ImGuiManager.h>


///- クラスの名前をstringに変換するsample code
#include <iostream>
#include <typeinfo>
#include <string>


GameObject::GameObject() {
	GameObjectManager::GetInstance()->AddGameObject(this);

	std::string name = typeid(*this).name();
	name = name.substr(std::string("class ").length());
	SetTag(name);

}

void GameObject::SetTag(const std::string& tag) {
	tag_ = tag;
}



const std::string& GameObject::GetTag() const {
	return tag_;
}

void GameObject::ImGuiDebug() {
	ImGui::SetNextItemOpen(true, ImGuiCond_Always);
	if(!ImGui::TreeNodeEx(tag_.c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
		return;
	}

	worldTransform_.ImGuiTreeNodeDebug();


	ImGui::TreePop();
}

const WorldTransform& GameObject::GetWorldTransform() const {
	return worldTransform_;
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
#ifdef _DEBUG


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


#endif // _DEBUG
}

#include <GameObject.h>

#include <ImGuiManager.h>

#include <string>

#include <CreateName.h>


GameObject::GameObject() {
	GameObjectManager::GetInstance()->AddGameObject(this);
	SetTag(CreateName(this));

}

void GameObject::SetTag(const std::string& tag) {
	tag_ = tag;
}



const std::string& GameObject::GetTag() const {
	return tag_;
}

void GameObject::SetName(const std::string& name) {
	name_ = name;
}

const std::string& GameObject::GetName() const {
	return name_;
}

void GameObject::ImGuiDebug() {

	worldTransform_.ImGuiTreeNodeDebug();

}

const WorldTransform& GameObject::GetWorldTransform() const {
	return worldTransform_;
}


void GameObject::SetPosition(const Vec3f& position) { worldTransform_.translate = position; }
void GameObject::SetPositionX(float x) { worldTransform_.translate.x = x; }
void GameObject::SetPositionY(float y) { worldTransform_.translate.y = y; }
void GameObject::SetPositionZ(float z) { worldTransform_.translate.z = z; }

void GameObject::UpdateMatrix() {
	worldTransform_.UpdateWorldMatrix();
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
		ImGui::SetNextItemOpen(true, ImGuiCond_Always);
		if(ImGui::TreeNodeEx(selectObject_->GetTag().c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {


			selectObject_->ImGuiDebug();
			ImGui::TreePop();
		}
	}

	ImGui::End();


#endif // _DEBUG
}

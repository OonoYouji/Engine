#include <GameObject.h>

#include <string>

#include <CreateName.h>

#include <ImGuiManager.h>
#include <GameObjectManager.h>


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

void GameObject::SetParent(const GameObject* parent) {
	parent_ = parent;
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
	worldTransform_.UpdateMatTransform();
	if(parent_) {
		worldTransform_.matTransform *= parent_->GetWorldTransform().matTransform;
	}
}



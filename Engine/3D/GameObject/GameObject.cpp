#include <GameObject.h>

#include <string>

#include <CreateName.h>

#include <ImGuiManager.h>
#include <GameObjectManager.h>


GameObject::GameObject() {
	GameObjectManager::GetInstance()->AddGameObject(this);
	SetName(CreateName(this));

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

void GameObject::SetParent(GameObject* parent) {
	parent_ = parent;
}

GameObject* GameObject::GetParent() const {
	return parent_;
}

bool GameObject::AddChild(GameObject* child) {
	auto it = std::find(childs_.begin(), childs_.end(), child);
	if(it != childs_.end()) {
		return false; //- 失敗
	}
	childs_.push_back(child);
	return true; //- 成功
}

bool GameObject::SubChild(GameObject* child) {
	auto it = std::find(childs_.begin(), childs_.end(), child);
	if(it != childs_.end()) {
		childs_.erase(it);
		return true; //- 成功
	}
	return false; //- 失敗
}

const std::list<GameObject*>& GameObject::GetChilds() const {
	return childs_;
}

void GameObject::ImGuiDebug() {

	//worldTransform_.ImGuiTreeNodeDebug();
	object_->ImGuiDebug();
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

void GameObject::CreateObejct() {
	CreateObejct(GetName());
}

void GameObject::CreateObejct(const std::string& objectName) {
	object_ = Epm::GetInstance()->CreateObject(objectName);
}

void GameObject::CreateTransformGroup() {
	Epm::Group& group = object_->CreateGroup("Transform");
	group.SetPtr("scale", &worldTransform_.scale);
	group.SetPtr("rotate", &worldTransform_.rotate);
	group.SetPtr("translate", &worldTransform_.translate);
}




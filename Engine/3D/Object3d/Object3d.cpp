#include <Object3d.h>

#include <CreateName.h>

#include <Model.h>
#include <ModelManager.h>

#include <ExternalParamManager.h>


int Object3d::nextId_ = 0;

Object3d::Object3d() {
	std::string name = CreateName(this);
	SetName(name);
	SetTag(name);
	id_ = nextId_++;
}

Object3d::~Object3d() {}

void Object3d::Initialize(const std::string& key) {
	CreateObjectType(this);

	modelKey_ = key;
	model_ = ModelManager::GetInstance()->GetModelPtr(modelKey_);
	worldTransform_.Initialize();

	SetName(key + std::to_string(id_));
	GameObject::Initialize();

	/*Epm::Group& group = category_->CraeteGroup("model");
	group.SetPtr("key", &modelKey_);*/

}

void Object3d::Update() {
	UpdateMatrix();
}

void Object3d::Draw() {
	model_->Draw(worldTransform_);
}

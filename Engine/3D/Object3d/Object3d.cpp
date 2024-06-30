#include <Object3d.h>

#include <CreateName.h>

#include <Model.h>
#include <ModelManager.h>

#include <ExternalParamManager.h>


int Object3d::instanceCount_ = 0;

Object3d::Object3d() {
	SetName(CreateName(this));
}

Object3d::~Object3d() {}

void Object3d::Initialize(const std::string& key) {
	id_ = instanceCount_;
	instanceCount_++;

	model_ = ModelManager::GetInstance()->GetModelPtr(key);
	worldTransform_.Initialize();

	SetName(key + std::to_string(id_));

}

void Object3d::Update() {
	UpdateMatrix();
}

void Object3d::Draw() {
	model_->Draw(worldTransform_);
}

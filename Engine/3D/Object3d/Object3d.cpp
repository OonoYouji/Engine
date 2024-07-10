#include <Object3d.h>

#include <CreateName.h>

#include <Model.h>
#include <ModelManager.h>

//#include <ExternalParamManager.h>


int Object3d::nextId_ = 0;

Object3d::Object3d() {
	std::string name = CreateName(this);
	SetName(name);
	SetTag(name);
	id_ = nextId_++;
}

Object3d::~Object3d() {}

void Object3d::Initialize(const std::string& key) {
	Initialize();

	modelKey_ = key;
	SetName(modelKey_ + std::to_string(id_));
	model_ = ModelManager::GetInstance()->GetModelPtr(modelKey_);

}

void Object3d::Initialize() {

	worldTransform_.Initialize();
	uvTransform_.Initilize();

	GameObject::CreateTransformGroup();

	Group& model = CreateGroup("Model");
	model.SetPtr("key", &modelKey_);

	Group& uvTransform = CreateGroup("UvTransform");
	uvTransform.SetPtr("scale", &uvTransform_.scale);
	uvTransform.SetPtr("rotate", &uvTransform_.rotate);
	uvTransform.SetPtr("tranalate", &uvTransform_.tranalate);

	if(!modelKey_.empty()) {
		SetName(modelKey_ + std::to_string(id_));
		model_ = ModelManager::GetInstance()->GetModelPtr(modelKey_);
	}


}

void Object3d::Update() {
	UpdateMatrix();
	uvTransform_.UpdateMatrix();
}

void Object3d::Draw() {
	if(model_) {
		model_->Draw(worldTransform_, uvTransform_.matTransform);
	}
}

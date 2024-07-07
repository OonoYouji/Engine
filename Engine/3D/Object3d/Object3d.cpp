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

	GameObject::CreateTransformGroup();

	Group& group = CreateGroup("Model");
	group.SetPtr("key", &modelKey_);

	if(!modelKey_.empty()) {
		SetName(modelKey_ + std::to_string(id_));
		model_ = ModelManager::GetInstance()->GetModelPtr(modelKey_);
	}


}

void Object3d::Update() {
	UpdateMatrix();
}

void Object3d::Draw() {
	if(model_) {
		model_->Draw(worldTransform_);
	}
}

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
	SetType(Type::Object3d);
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

	material_.color = { 1,1,1,1 };
	material_.enableLighting = 1;
	material_.uvTransform = uvTransform_.matTransform;


	Group& material = CreateGroup("Material");
	material.SetPtr("Lighting", &material_.enableLighting);

}

void Object3d::Update() {
	UpdateMatrix();
	uvTransform_.UpdateMatrix();

	aabb_.min = {};
	aabb_.max = {};
	aabb_.translation = GetPos();
	for(auto& vertex : model_->GetVertexDatas()) {
		Vec3f position = Vec3f::Convert4To3(vertex.position);
		aabb_.ExpandToFit(Mat4::TransformNormal(position, GetWorldTransform().matTransform));
	}

	aabbs_.clear();
	int index = 0;
	for(auto& vertex : model_->GetVertexDatas()) {
		if(index % 3 == 0) {
			aabbs_.push_back(AABB());
		}

		auto& back = aabbs_.back();
		Vec3f position = Vec3f::Convert4To3(vertex.position);
		back.ExpandToFit(Mat4::TransformNormal(position, GetWorldTransform().matTransform));
		back.translation = GetPos();
		
		index++;

	}

	
	material_.uvTransform = uvTransform_.matTransform;


}

void Object3d::Draw() {
	if(model_) {
		model_->Draw(worldTransform_, material_);
	}

	//for(auto& aabb : aabbs_) {
	//	aabb.Draw({1.0f, 0.0f, 0.0f, 0.0f});
	//}

	//aabb_.Draw();

}

#include <DebugCamera.h>

#include <ImGuiManager.h>
#include <Input.h>

#include <CreateName.h>

DebugCamera::DebugCamera() {
	SetName(CreateName(this));
	SetTag(CreateName(this));
	Initialize();
}
DebugCamera::~DebugCamera() {}


void DebugCamera::Initialize() {

	if(!object_) {
		CreateObejct();
	}


	worldTransform_.Initialize();
	viewProjection_.UpdateProjection();
	viewProjection_.fovY = 0.45f;
	viewProjection_.farZ = 1000.0f;
	GameObject::CreateTransformGroup();
	//Epm::Group* group = &category_->CraeteGroup("Viewport");
	//group->SetPtr("fovY", &viewProjection_.fovY);
	//group->SetPtr("farZ", &viewProjection_.farZ);
}



void DebugCamera::Update() {

	velocity_ = { 0.0f,0.0f,0.0f };

	if(Input::PressMouse(2)) {
		velocity_ += (Vec3f(Input::MouseVelocity(), 0.0f) / 256.0f) * -1.0f;
		velocity_.y *= -1.0f;
		velocity_.z = 0.0f;
	}

	velocity_.z = Input::MouseWheel() / 256.0f;
	velocity_.x *= 1.0f + Vec3f::Length(worldTransform_.translate) / 32.0f;
	velocity_.y *= 1.0f + Vec3f::Length(worldTransform_.translate) / 32.0f;

	if(Input::PressMouse(1)) {
		worldTransform_.rotate.x += Input::MouseVelocity().y / 256.0f;
		worldTransform_.rotate.y += Input::MouseVelocity().x / 256.0f;
	}

	velocity_ = Mat4::Transform(velocity_, Mat4::MakeRotate(worldTransform_.rotate));
	worldTransform_.translate += velocity_;

	worldTransform_.UpdateMatTransform();
	viewProjection_.matView = Mat4::MakeInverse(worldTransform_.matTransform);
	viewProjection_.UpdateProjection();

}

void DebugCamera::Draw() {
	//model_->Draw();
}



void DebugCamera::ImGuiDebug() {
	worldTransform_.ImGuiTreeNodeDebug();
	ImGui::Separator();

	if(ImGui::TreeNodeEx("Parameters", ImGuiTreeNodeFlags_DefaultOpen)) {

		///- 視野角
		ImGui::DragFloat("FovY", &viewProjection_.fovY, 0.025f, 0.1f, 3.0f);
		if(ImGui::IsItemEdited()) { viewProjection_.UpdateProjection(); }

		///- 奥行き
		ImGui::DragFloat("FarZ", &viewProjection_.farZ, 5.0f, 1.0f, std::pow(2.0f, 32.0f));
		if(ImGui::IsItemEdited()) { viewProjection_.UpdateProjection(); }

		ImGui::TreePop();
	}
}

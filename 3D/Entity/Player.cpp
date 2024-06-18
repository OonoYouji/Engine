#include <Player.h>

#include <Input.h>
#include <ImGuiManager.h>

#include <iostream>
#include <typeinfo>
#include <string>

Player::Player() {
	std::string name = typeid(*this).name();
	name = name.substr(std::string("class ").length());
	SetTag(name);
}
Player::~Player() {}


void Player::Initialize() {

	model_ = std::make_unique<Model>();
	model_->Initialize("./Resources/Objects/SampleObject", "sampleObject.obj");

	worldTransform_.Initialize();

}

void Player::Update() {

	Move();


	if(isKinematic_) {
		worldTransform_.translate.y += kGravity_;
	}

	//if(GetDiffHeight() < kWallHeight_) {
	if(worldTransform_.GetWorldPosition().y < terrainHeight_) {
		worldTransform_.translate.y = terrainHeight_;
	}
	//} else {
		///- 壁と判定された : 押し戻し処理を行う

	//}

	UpdateMatrix();

}

void Player::Draw() {

	model_->Draw(worldTransform_);

}

void Player::SetHeight(float height) {
	worldTransform_.translate.y = height;
}

float Player::GetDiffHeight() {
	return terrainHeight_ - preTerrainHeight_;
}

void Player::Move() {
	velocity_.x = static_cast<float>(Input::PressKey(DIK_D) - Input::PressKey(DIK_A));
	velocity_.z = static_cast<float>(Input::PressKey(DIK_W) - Input::PressKey(DIK_S));

	velocity_ = Vec3f::Normalize(velocity_);
	velocity_ *= kSpeed_;

	worldTransform_.translate += velocity_;
}

void Player::ImGuiDebug() {

	worldTransform_.ImGuiTreeNodeDebug();

	ImGui::Checkbox("IsKinematic", &isKinematic_);


}


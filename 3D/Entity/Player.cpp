#include <Player.h>

#include <Input.h>
#include <ImGuiManager.h>

#include <iostream>
#include <typeinfo>
#include <string>

Player::Player() {
	SetTag(typeid(this).name());
}
Player::~Player() {}


void Player::Initialize() {

	model_ = std::make_unique<Model>();
	model_->Initialize("./Resources/Objects/SampleObject", "sampleObject.obj");

	worldTransform_.Initialize();

}

void Player::Update() {

	ImGuiDebug();
	
	velocity_.x = static_cast<float>(Input::PressKey(DIK_D) - Input::PressKey(DIK_A));
	velocity_.z = static_cast<float>(Input::PressKey(DIK_W) - Input::PressKey(DIK_S));

	velocity_ = Vec3f::Normalize(velocity_);
	velocity_ *= kSpeed_;

	worldTransform_.translate += velocity_;

	worldTransform_.UpdateWorldMatrix();
}

void Player::Draw() {

	model_->Draw(worldTransform_);

}

void Player::SetHeight(float height) {
	worldTransform_.translate.y = height;
}

void Player::Move() {

}


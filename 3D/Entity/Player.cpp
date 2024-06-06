#include <Player.h>



Player::Player() {}
Player::~Player() {}


void Player::Initialize() {

	model_ = std::make_unique<Model>();
	model_->Initialize("./Resources/Objects/Monkey", "monkey.obj");

	worldTransform_.Initialize();

}

void Player::Update() {

}

void Player::Draw() {

	model_->Draw(worldTransform_);

}

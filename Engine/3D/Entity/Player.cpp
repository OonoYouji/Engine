#include <Player.h>

#include <Input.h>
#include <ImGuiManager.h>

#include <CreateName.h>
#include <ModelManager.h>

Player::Player() {
	SetName(CreateName(this));
	SetTag(CreateName(this));
}
Player::~Player() {}


void Player::Initialize() {

	/*if(!object_) {
		CreateObejct();
	}*/


	model_ = ModelManager::GetInstance()->GetModelPtr("SampleObject");

	worldTransform_.Initialize();
	SetType(Type::Object3d);

	GameObject::CreateTransformGroup();
}

void Player::Update() {

	Move();


	if(isKinematic_) {
		worldTransform_.translate.y += kGravity_;
	}

	//if(GetDiffHeight() < kWallHeight_) {
	/*if(worldTransform_.GetWorldPosition().y < terrainHeight_) {
		worldTransform_.translate.y = terrainHeight_;
	}*/
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
	Vec2f input = Input::GamePad_L_Stick();
	velocity_.x = input.x;
	velocity_.z = input.y;

	velocity_ = Vec3f::Normalize(velocity_);
	velocity_ *= kSpeed_;

	worldTransform_.translate += velocity_;
}

void Player::ImGuiDebug() {

	worldTransform_.ImGuiTreeNodeDebug();

	ImGui::Checkbox("IsKinematic", &isKinematic_);


}


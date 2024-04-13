#include <Camera.h>

#include <Environment.h>



Camera::Camera() { Init(); }
Camera::~Camera() { Finalize(); }

void Camera::Init() {

	scale_ = { 1.0f,1.0f,1.0f };
	rotate_ = { 0.0f,0.0f,0.0f };
	worldPos_ = { 0.0f,0.0f,-5.0f };

	worldMatrix_ = Matrix4x4::MakeAffine(scale_, rotate_, worldPos_);
	viewMatrix_ = Matrix4x4::MakeInverse(worldMatrix_);

	projectionMatrix_ = Matrix4x4::MakePerspectiveFovMatrix(
		0.45f, static_cast<float>(kWindowSize.x) / static_cast<float>(kWindowSize.y),
		0.1f, 100.0f);

	vpMatrix_ = viewMatrix_ * projectionMatrix_;

}

void Camera::Update() {


}

void Camera::Draw() {


}

void Camera::Finalize() {


}

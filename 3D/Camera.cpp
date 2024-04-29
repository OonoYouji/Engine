#include <Camera.h>

#include <cmath>

#include <Environment.h>
#include "ImGuiManager.h"


Camera::Camera() { Init(); }
Camera::~Camera() { Finalize(); }

void Camera::Init() {

	scale_ = { 1.0f,1.0f,1.0f };
	//rotate_ = { 0.26f,0.0f,0.0f };
	rotate_ = { 0.5f, 0.0f, 0.0f };
	worldPos_ = { 0.0f,50.0f, -100.0f };

	fovY_ = 0.45f;
	farZ_ = 1000.0f;

	worldMatrix_ = Matrix4x4::MakeAffine(scale_, rotate_, worldPos_);
	viewMatrix_ = Matrix4x4::MakeInverse(worldMatrix_);

	MakeProjectionMatrix();

	vpMatrix_ = viewMatrix_ * projectionMatrix_;

}

void Camera::Update() {
#ifdef _DEBUG

	ImGui::Begin("Camera");

	/// -------------------------------------------
	/// ↓ 拡縮・回転・平行移動
	/// -------------------------------------------
	if(ImGui::TreeNodeEx("Transform", true)) {

		ImGui::DragFloat3("scale", &scale_.x, 0.25f);
		ImGui::DragFloat3("rotate", &rotate_.x, 1.0f / 64.0f);
		ImGui::DragFloat3("translate", &worldPos_.x, 0.25f);

		ImGui::TreePop();
	}


	ImGui::Separator();

	/// -------------------------------------------
	/// ↓ 各種パラメータ
	/// -------------------------------------------
	if(ImGui::TreeNodeEx("Parameters", true)) {

		///- 視野角
		ImGui::DragFloat("FovY", &fovY_, 0.025f, 0.1f, 3.0f);
		if(ImGui::IsItemEdited()) { MakeProjectionMatrix(); }

		///- 奥行き
		ImGui::DragFloat("FarZ", &farZ_, 5.0f, 1.0f, std::pow(2.0f, 32.0f));
		if(ImGui::IsItemEdited()) { MakeProjectionMatrix(); }

		ImGui::TreePop();
	}

	ImGui::End();

#endif // _DEBUG

	worldMatrix_ = Matrix4x4::MakeAffine(scale_, rotate_, worldPos_);
	viewMatrix_ = Matrix4x4::MakeInverse(worldMatrix_);
	vpMatrix_ = viewMatrix_ * projectionMatrix_;


}

void Camera::Draw() {


}

void Camera::Finalize() {


}

void Camera::MakeProjectionMatrix() {
	projectionMatrix_ = Matrix4x4::MakePerspectiveFovMatrix(
		fovY_, static_cast<float>(kWindowSize.x) / static_cast<float>(kWindowSize.y),
		0.1f, farZ_);
}

#include <Camera.h>

#include <cmath>

#include <Environment.h>
#include <ImGuiManager.h>
#include <Input.h>
#include <DebugCamera.h>


Camera::Camera() { Init(); }
Camera::~Camera() { Finalize(); }

void Camera::Init() {

	worldTransform_.Initialize();
	//worldTransform_.rotate = { 0.5f, 0.0f, 0.0f };
	//worldTransform_.translate = { 0.0f,3.25f, -5.5f };
	worldTransform_.rotate = { 1.5f, 0.0f, 0.0f };
	worldTransform_.translate = { 0.0f,0.0f, -5.5f };

	viewProjection_.fovY = 0.45f;
	viewProjection_.farZ = 1000.0f;

	viewProjection_.matView = Matrix4x4::MakeInverse(worldTransform_.worldMatrix);
	viewProjection_.UpdateProjection();

	vpMatrix_ = viewProjection_.matView * viewProjection_.matProjection;

#ifdef _DEBUG
	debugCamera_ = std::make_unique<DebugCamera>();
	debugCamera_->Initalize();
	isDebugCameraActive_ = false;
#endif // _DEBUG
}

void Camera::Update() {

	DebugDraw();

	worldTransform_.UpdateWorldMatrix();
	viewProjection_.matView = Matrix4x4::MakeInverse(worldTransform_.worldMatrix);
	viewProjection_.UpdateProjection();
	vpMatrix_ = viewProjection_.matView * viewProjection_.matProjection;

#ifdef _DEBUG
	if(isDebugCameraActive_) {
		debugCamera_->Update();
		viewProjection_.matProjection = debugCamera_->GetViewProjection().matProjection;
		viewProjection_.matView = debugCamera_->GetViewProjection().matView;
		vpMatrix_ = viewProjection_.matView * viewProjection_.matProjection;
	}
#endif // _DEBUG
}

void Camera::DebugDraw() {
#ifdef _DEBUG

	ImGui::Begin("Camera");

	/// -------------------------------------------
	/// ↓ 拡縮・回転・平行移動
	/// -------------------------------------------
	worldTransform_.ImGuiTreeNodeDebug();

	ImGui::Separator();

	/// -------------------------------------------
	/// ↓ 各種パラメータ
	/// -------------------------------------------
	if(ImGui::TreeNodeEx("Parameters", ImGuiTreeNodeFlags_DefaultOpen)) {

		///- 視野角
		ImGui::DragFloat("FovY", &viewProjection_.fovY, 0.025f, 0.1f, 3.0f);
		if(ImGui::IsItemEdited()) { viewProjection_.UpdateProjection(); }

		///- 奥行き
		ImGui::DragFloat("FarZ", &viewProjection_.farZ, 5.0f, 1.0f, std::pow(2.0f, 32.0f));
		if(ImGui::IsItemEdited()) { viewProjection_.UpdateProjection(); }

		ImGui::TreePop();
	}

	ImGui::Separator();

	/// -------------------------------------------
	/// ↓ DebugCamera
	/// -------------------------------------------
	if(ImGui::TreeNodeEx("DebugCamera", ImGuiTreeNodeFlags_DefaultOpen)) {

		ImGui::Checkbox("IsActive", &isDebugCameraActive_);
		debugCamera_->DebugDraw();
		if(!isDebugCameraActive_) {
			debugCamera_->Draw();
		}

		ImGui::TreePop();
	}

	ImGui::End();

#endif // _DEBUG
}

void Camera::Finalize() {


}


void ViewProjection::UpdateProjection() {
	matProjection = Matrix4x4::MakePerspectiveFovMatrix(
		fovY, static_cast<float>(kWindowSize.x) / static_cast<float>(kWindowSize.y),
		0.1f, farZ);
}

ViewProjection& ViewProjection::operator=(const ViewProjection& other) {
	this->farZ = other.farZ;
	this->fovY = other.fovY;
	this->matProjection = other.matProjection;
	this->matView = other.matView;
	return *this;
}

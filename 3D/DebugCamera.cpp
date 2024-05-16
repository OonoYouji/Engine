#include <DebugCamera.h>

#include <ImGuiManager.h>


DebugCamera::DebugCamera() {}
DebugCamera::~DebugCamera() {}



void DebugCamera::Initalize() {
	worldTransform_.Initialize();
	viewProjection_.UpdateProjection();
	viewProjection_.fovY = 0.45f;
	viewProjection_.farZ = 1000.0f;
}



void DebugCamera::Update() {
	worldTransform_.UpdateWorldMatrix();
	viewProjection_.matView = Mat4::MakeInverse(worldTransform_.worldMatrix);
	viewProjection_.UpdateProjection();
}



void DebugCamera::DebugDraw() {
#ifdef _DEBUG
	worldTransform_.ImGuiTreeNodeDebug();
	ImGui::Separator();
	
#endif // _DEBUG
}

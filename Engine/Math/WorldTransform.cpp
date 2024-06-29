#include <WorldTransform.h>

#include <ImGuiManager.h>



/// -------------------------
///	初期化関数
/// -------------------------
void WorldTransform::Initialize() {

	scale = { 1.0f,1.0f,1.0f };
	rotate = { 0.0f,0.0f,0.0f };
	translate = { 0.0f,0.0f,0.0f };
	UpdateMatTransform();
}



/// -------------------------
/// world行列の生成
/// -------------------------
void WorldTransform::UpdateMatTransform() {
	matTransform = Mat4::MakeAffine(scale, rotate, translate);
}

void WorldTransform::ImGuiTreeNodeDebug() {
	if(ImGui::TreeNodeEx("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {

		ImGui::DragFloat3("Scale", &scale.x, 0.05f);
		ImGui::DragFloat3("Rotate", &rotate.x, 0.025f);
		ImGui::DragFloat3("Translate", &translate.x, 0.05f);

		ImGui::TreePop();
	}
}

Vec3f WorldTransform::GetWorldPosition() {
	return Mat4::Transform({ 0.0f,0.0f,0.0f }, matTransform);
}

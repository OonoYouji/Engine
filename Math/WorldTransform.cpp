#include <WorldTransform.h>

#include <ImGuiManager.h>



/// -------------------------
///	初期化関数
/// -------------------------
void WorldTransform::Initialize() {

	scale = { 1.0f,1.0f,1.0f };
	rotate = { 0.0f,0.0f,0.0f };
	translate = { 0.0f,0.0f,0.0f };
	UpdateWorldMatrix();
}



/// -------------------------
/// world行列の生成
/// -------------------------
void WorldTransform::UpdateWorldMatrix() {
	worldMatrix = Mat4::MakeAffine(scale, rotate, translate);
}

void WorldTransform::ImGuiTreeNodeDebug() {
	if(ImGui::TreeNodeEx("WorldTransform", ImGuiTreeNodeFlags_DefaultOpen)) {

		ImGui::DragFloat3("Scale", &scale.x, 0.05f);
		ImGui::DragFloat3("Rotate", &rotate.x, 0.025f);
		ImGui::DragFloat3("Translate", &translate.x, 0.05f);

		ImGui::TreePop();
	}
}


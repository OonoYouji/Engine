#include "DirectionalLight.h"

#include <DirectXCommon.h>
#include <ImGuiManager.h>

Light::Light() {}
Light::~Light() {}



Light* Light::GetInstance() {
	static Light instance;
	return &instance;
}



void Light::Init() {

	resource_.Attach(DirectXCommon::GetInstance()->CreateBufferResource(sizeof(DirectionalLight)));

	resource_->Map(0, nullptr, reinterpret_cast<void**>(&directinalLight_));

	directinalLight_->color = { 1.0f,1.0f,1.0f,1.0f };
	directinalLight_->direction = Vec3f::Normalize({ 1.0f,-1.0f,1.0f });
	directinalLight_->intensity = 1.0f;

	nextDir_ = { 0.0f,0.0f,0.0f };

}

void Light::Update() {
#ifdef _DEBUG
	ImGui::Begin("Light");

	ImGui::DragFloat3("nextDirection", &nextDir_.x, 0.01f);
	if(ImGui::Button("SetDirection")) {
		directinalLight_->direction = Vec3f::Normalize(nextDir_);
	}

	ImGui::End();
#endif // _DEBUG


}

void Light::Finalize() {
	resource_.Reset();
}



void Light::SetConstantBuffer(ID3D12GraphicsCommandList* commandList) {
	commandList->SetGraphicsRootConstantBufferView(5, resource_->GetGPUVirtualAddress());
}

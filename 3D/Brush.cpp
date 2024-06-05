#include "Brush.h"

#include <cmath>
#include <numbers>

#include <DxCommand.h>
#include <TextureManager.h>
#include "Environment.h"
#include "Engine.h"
#include "ImGuiManager.h"
#include "Camera.h"
#include "Input.h"
#include "DirectionalLight.h"
#include <PipelineStateObjectManager.h>

using namespace std::numbers;


Brush::Brush() {}
Brush::~Brush() {
	wvpResource_.Reset();
	materialResource_.Reset();
	vertexResource_.Reset();
	mousePointResource_.Reset();
}



void Brush::Init() {
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	///- 頂点数の決定
	vertexData_.resize(kTriangleVertexCount_ * kVertexCount_);


	///- 頂点数分確保
	vertexResource_ = dxCommon->CreateBufferResource(sizeof(VertexData) * vertexData_.size());

	///- vertexBufferViewの作成
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	vertexBufferView_.SizeInBytes = UINT(sizeof(VertexData) * vertexData_.size());
	vertexBufferView_.StrideInBytes = sizeof(VertexData);

	///- 頂点データの書き込み
	pData_ = vertexData_.data();
	vertexResource_->Map(0, nullptr, &pMappedData_);
	memcpy(pMappedData_, pData_, vertexData_.size() * sizeof(VertexData));
	vertexResource_->Unmap(0, nullptr);

	///- マテリアルリソースの生成; 情報の書き込み
	materialResource_ = dxCommon->CreateBufferResource(sizeof(Material));
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
	materialData_->color = { 1.0f,1.0f,1.0f,1.0f };
	materialData_->enableLighting = false;

	///- 行列リソースの生成; 書き込み
	wvpResource_ = dxCommon->CreateBufferResource(sizeof(TransformMatrix));
	wvpResource_->Map(0, nullptr, reinterpret_cast<void**>(&matrixData_));
	matrixData_->World = Matrix4x4::MakeIdentity(); //- とりあえずの単位行列
	matrixData_->WVP = Matrix4x4::MakeIdentity(); //- とりあえずの単位行列

	///- マウスの座標
	mousePointResource_ = dxCommon->CreateBufferResource(sizeof(MousePoint));
	mousePointResource_->Map(0, nullptr, reinterpret_cast<void**>(&mousePointData_));
	mousePointData_->position = Vec2f{ 0.0f,0.0f };
	mousePointData_->size = 10.0f;
	mousePointData_->worldPos = Vec3f{ 0.0f,0.0f,0.0f };
	mousePointData_->isUp = false;
	mousePointData_->isDown = false;
	mousePointData_->rayDir = Vec3f{ 0.0f,0.0f,0.0f };
	mousePointData_->isActive = false;
	mousePointData_->calcState = static_cast<int>(CalcState::Add);
	mousePointData_->power = 0.5f;

	///- 頂点データの計算
	int index = 0;
	for(int i = 0; i < kVertexCount_; i++) {

		///- 頂点のθ
		float thetaA = (float(i) / float(kVertexCount_ / 2)) * pi_v<float>;
		float thetaB = (float(i + 1) / float(kVertexCount_ / 2)) * pi_v<float>;

		/// A
		vertexData_[index + 0].position = { 0.0f,0.0f,0.0f,1.0f };
		vertexData_[index + 0].texcoord = { 0.0f,0.0f };

		/// B
		vertexData_[index + 1].position = {
			std::cos(thetaB),
			0.0f,
			std::sin(thetaB),
			1.0f
		};
		vertexData_[index + 1].texcoord = { 0.0f,1.0f };

		/// C
		vertexData_[index + 2].position = {
			std::cos(thetaA),
			0.0f,
			std::sin(thetaA),
			1.0f
		};
		vertexData_[index + 2].texcoord = { 1.0f,0.0f };



		index += kTriangleVertexCount_;
	}


	worldTransform_.Initialize();

	distanceTestObject_ = 1.0f;
	circleRadius_ = 0.0f;

}



void Brush::Update() {
	mousePointData_->position = Input::GetMousePosition();
	mousePos_ = { mousePointData_->position.x, mousePointData_->position.y, 0.0f };
	mousePointData_->isUp = Input::PressMouse(0);
	mousePointData_->isDown = Input::PressMouse(1);

	///- マウスのスクリーン座標をワールド座標に変換
	ConvertMousePosition();

	///- ImGuiデバッグ
#ifdef _DEBUG
	ImGui::Begin("Brush");


	ImGui::DragFloat3("WorldPosition", &mousePointData_->worldPos.x, 0.0f);
	ImGui::DragFloat2("ScreenPosition", &mousePointData_->position.x, 0.0f);

	ImGui::Separator();

	if(ImGui::TreeNodeEx("Parameter", true)) {

		ImGui::DragFloat("circleRadius", &mousePointData_->size, 0.01f);

		ImGui::Spacing();

		static bool isActive = false;
		ImGui::Checkbox("IsActive", &isActive);
		if(ImGui::IsItemEdited()) {
			mousePointData_->isActive = isActive;
		}

		ImGui::Spacing();

		///- 上下量の決定
		ImGui::DragFloat("Power", &mousePointData_->power, 0.005f, 0.0f, 1.0f);

		ImGui::Spacing();

		///- 編集のタイプ
		ImGui::DragInt("CalcState", &mousePointData_->calcState, 0);
		if(ImGui::Button("Add", ImVec2(64.0f, 24.0f))) {
			mousePointData_->calcState = static_cast<int>(CalcState::Add);
		}

		ImGui::SameLine();

		if(ImGui::Button("Mul", ImVec2(64.0f, 24.0f))) {
			mousePointData_->calcState = static_cast<int>(CalcState::Mul);
		}




		ImGui::TreePop();
	}

	ImGui::Separator();



	ImGui::End();
#endif // _DEBUG


	///- 拡縮行列は円の半径
	worldTransform_.scale = {
		circleRadius_,
		circleRadius_,
		circleRadius_
	};


}



void Brush::Draw() {
	ID3D12GraphicsCommandList* commandList = DxCommand::GetInstance()->GetList();
	/////- 深度値のリセット; 必ず上側に表示されるようになる
	//DirectXCommon::GetInstance()->ClearDepthBuffer();

	//commandList->IASetVertexBuffers(0, 1, &vertexBufferView_);

	/////- データの書き込み

	/////- 頂点情報
	//memcpy(pMappedData_, pData_, vertexData_.size() * sizeof(VertexData));

	/////- 色情報
	////materialData_->color = { 0.0f,0.0f,0.0f,1.0f };
	/////- 行列情報
	//worldTransform_.MakeWorldMatrix();
	//matrixData_->World = worldTransform_.worldMatrix;
	//matrixData_->WVP = worldTransform_.worldMatrix * Engine::GetCamera()->GetVpMatrix();



	/////- 各種設定
	//commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//commandList->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
	//commandList->SetGraphicsRootConstantBufferView(1, wvpResource_->GetGPUVirtualAddress());
	PipelineStateObjectManager::GetInstance()->SetCommandList(1, commandList);
	commandList->SetGraphicsRootConstantBufferView(6, mousePointResource_->GetGPUVirtualAddress());
	////commandList->SetGraphicsRootDescriptorTable(2, DirectXCommon::GetInstance()->GetTextureSrvHandleGPU());
	//TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(2, "uvChecker");
	//TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(3, "yama");
	//Light::GetInstance()->SetConstantBuffer(commandList);

	/////- 描画 (DrawCall)
	//commandList->DrawInstanced(UINT(vertexData_.size()), 1, 0, 0);

}



void Brush::ConvertMousePosition() {


	Matrix4x4 matVPV = Engine::GetCamera()->GetViewMatrix() * Engine::GetCamera()->GetProjectionMatrix() * DirectXCommon::GetInstance()->GetViewportMatrix();
	Matrix4x4 matInverseVPV = Matrix4x4::MakeInverse(matVPV);

	///- スクリーン座標
	posNear_ = { mousePos_.x, mousePos_.y, 0.0f };
	posFar_ = { mousePos_.x, mousePos_.y, 1.0f };

	///- スクリーンからワールド座標に
	posNear_ = Matrix4x4::Transform(posNear_, matInverseVPV);
	posFar_ = Matrix4x4::Transform(posFar_, matInverseVPV);

	///- マウスレイのベクトル
	mouseRayDirection_ = Vec3f::Normalize(posFar_ - posNear_);
	mousePointData_->rayDir = mouseRayDirection_;

	///- カメラから設定オブジェクトの距離
	worldTransform_.translate = posNear_ + (mouseRayDirection_ * distanceTestObject_);
	mousePointData_->worldPos = posNear_;

}

void Brush::SetColot(const Vec4f& color) {

	materialData_->color = color;

}


#include "Brush.h"

#include <cmath>
#include <numbers>

#include "Environment.h"
#include "Engine.h"
#include "ImGuiManager.h"
#include "Camera.h"
#include "Input.h"



using namespace std::numbers;


Brush::Brush() {}
Brush::~Brush() {
	wvpResource_.Reset();
	materialResource_.Reset();
	vertexResource_.Reset();
}



void Brush::Init() {
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	///- 頂点数の決定
	vertexData_.resize(kTriangleVertexCount_ * kVertexCount_);


	///- 頂点数分確保
	vertexResource_.Attach(dxCommon->CreateBufferResource(sizeof(VertexData) * vertexData_.size()));

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
	materialResource_.Attach(dxCommon->CreateBufferResource(sizeof(Vector4)));
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
	*materialData_ = { 1.0f,1.0f,1.0f,1.0f };

	///- 行列リソースの生成; 書き込み
	wvpResource_.Attach(dxCommon->CreateBufferResource(sizeof(Matrix4x4)));
	wvpResource_->Map(0, nullptr, reinterpret_cast<void**>(&wvpData_));
	*wvpData_ = Matrix4x4::MakeIdentity(); //- とりあえずの単位行列


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


	worldTransform_.Init();

	distanceTestObject_ = 1.0f;
	circleRadius_ = 0.25f;

}



void Brush::Update() {
	Input* input = Input::GetInstance();
	mousePos_ = { input->GetMousePos().x, input->GetMousePos().y, 0.0f };

	///- マウスのスクリーン座標をワールド座標に変換
	ConvertMousePosition();

	///- ImGuiデバッグ
#ifdef _DEBUG
	ImGui::Begin("Brush");

	ImGui::DragFloat4("worldMousePos", &worldTransform_.translate.x, 0.0f);
	ImGui::DragFloat("cameraDistance", &distanceTestObject_, 0.25f);
	ImGui::DragFloat("circleRadius", &circleRadius_, 0.01f);

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
	ID3D12GraphicsCommandList* commandList = DirectXCommon::GetInstance()->GetCommandList();

	commandList->IASetVertexBuffers(0, 1, &vertexBufferView_);

	///- データの書き込み

	///- 頂点情報
	memcpy(pMappedData_, pData_, vertexData_.size() * sizeof(VertexData));

	///- 色情報
	*materialData_ = { 0.0f,0.0f,0.0f,1.0f };
	///- 行列情報
	worldTransform_.MakeWorldMatrix();
	*wvpData_ = worldTransform_.worldMatrix * Engine::GetCamera()->GetVpMatrix();


	///- 各種設定
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	commandList->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
	commandList->SetGraphicsRootConstantBufferView(1, wvpResource_->GetGPUVirtualAddress());
	commandList->SetGraphicsRootDescriptorTable(2, DirectXCommon::GetInstance()->GetTextureSrvHandleGPU());

	///- 描画 (DrawCall)
	commandList->DrawInstanced(UINT(vertexData_.size()), 1, 0, 0);

}



void Brush::ConvertMousePosition() {


	Matrix4x4 matVPV = Engine::GetCamera()->GetViewMatrix() * Engine::GetCamera()->GetProjectionMatrix() * DirectXCommon::GetInstance()->GetViewportMatrix();
	Matrix4x4 matInverseVPV = Matrix4x4::MakeInverse(matVPV);

	///- スクリーン座標
	Vec3f posNear = { mousePos_.x, mousePos_.y, 0.0f };
	Vec3f posFar = { mousePos_.x, mousePos_.y, 1.0f };

	///- スクリーンからワールド座標に
	posNear = Matrix4x4::Transform(posNear, matInverseVPV);
	posFar = Matrix4x4::Transform(posFar, matInverseVPV);

	///- マウスレイのベクトル
	Vec3f mouseDirection = Vec3f::Normalize(posFar - posNear);

	///- カメラから設定オブジェクトの距離
	worldTransform_.translate = posNear + (mouseDirection * distanceTestObject_);

}


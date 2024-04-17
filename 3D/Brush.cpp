#include "Brush.h"

#include "Environment.h"
#include "Engine.h"
#include "ImGuiManager.h"
#include "Camera.h"
#include "Input.h"
#include "DirectXCommon.h"



Brush::Brush() {}
Brush::~Brush() {

}



void Brush::Init() {
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	///- 頂点数分確保
	vertexResource_.Attach(dxCommon->CreateBufferResource(sizeof(VertexData) * 3));

	///- vertexBufferViewの作成
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	vertexBufferView_.SizeInBytes = sizeof(VertexData) * 3;
	vertexBufferView_.StrideInBytes = sizeof(VertexData);

	///- 頂点データの書き込み
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));

	///- マテリアルリソースの生成; 情報の書き込み
	materialResource_.Attach(dxCommon->CreateBufferResource(sizeof(Vector4)));
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
	*materialData_ = { 1.0f,1.0f,1.0f,1.0f };


	///- 行列リソースの生成; 書き込み
	wvpResource_.Attach(dxCommon->CreateBufferResource(sizeof(Matrix4x4)));
	wvpResource_->Map(0, nullptr, reinterpret_cast<void**>(&wvpData_));
	*wvpData_ = Matrix4x4::MakeIdentity(); //- とりあえずの単位行列



	///- 頂点データの計算
	///- 上
	vertexData_[0].position = { 0.0f,0.0f,0.1f,1.0f };
	vertexData_[0].texcoord = { 0.0f,0.5f };
	///- 右下
	vertexData_[1].position = { 0.1f,0.0f,-0.1f,1.0f };
	vertexData_[1].texcoord = { 1.0f,1.0f };
	///- 左下
	vertexData_[2].position = { -0.1f,0.0f,-0.1f,1.0f };
	vertexData_[2].texcoord = { 0.0f,1.0f };


	worldTransform_.Init();

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

	ImGui::End();
#endif // _DEBUG





}



void Brush::Draw() {
	ID3D12GraphicsCommandList* commandList = DirectXCommon::GetInstance()->GetCommandList();

	commandList->IASetVertexBuffers(0, 1, &vertexBufferView_);

	///- データの書き込み

	///- 頂点情報

	///- 色情報
	*materialData_ = { 1.0f,0.0f,0.0f,1.0f };
	///- 行列情報
	worldTransform_.MakeWorldMatrix();
	*wvpData_ = worldTransform_.worldMatrix * Engine::GetCamera()->GetVpMatrix();


	///- 各種設定
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	commandList->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
	commandList->SetGraphicsRootConstantBufferView(1, wvpResource_->GetGPUVirtualAddress());
	commandList->SetGraphicsRootDescriptorTable(2, DirectXCommon::GetInstance()->GetTextureSrvHandleGPU());

	///- 描画 (DrawCall)
	commandList->DrawInstanced(3, 1, 0, 0);

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
	const float kDistanceTestObject = 1.0f;
	worldTransform_.translate = posNear + (mouseDirection * kDistanceTestObject);

	/*///- 正規化デバイス座標系
	Vec3f normalizeDevicePos = Matrix4x4::Transform(mousePos_, Matrix4x4::MakeInverse(DirectXCommon::GetInstance()->GetViewportMatrix()));
	///- プロジェクション座標
	Vec3f projectionPos = normalizeDevicePos * 1.0f;
	///- ビュー座標系
	Vec3f viewPos = Matrix4x4::Transform(projectionPos, Matrix4x4::MakeInverse(Engine::GetCamera()->GetProjectionMatrix()));
	///- ワールド座標系
	worldTransform_.translate = Matrix4x4::Transform(viewPos, Engine::GetCamera()->GetWorldMatrix());*/

}


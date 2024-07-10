#include "Sprite.h"

#include "DirectXCommon.h"
#include "Environment.h"
#include "ImGuiManager.h"
#include "DxCommand.h"
#include <TextureManager.h>
#include <DirectionalLight.h>
#include <PipelineStateObjectManager.h>

Sprite::Sprite() {}
Sprite::~Sprite() {
	materialResource_.Reset();
	transformationMatrixResource_.Reset();
	vertexResource_.Reset();
}


/// ===================================================
/// 初期化
/// ===================================================
void Sprite::Initialize() {
	dxCommon_ = DirectXCommon::GetInstance();


	position = { 0.0f,0.0f };
	color = { 1.0f,1.0f,1.0f,1.0f };


	///- vertex index resource
	CreateVertexResource();
	CreateIndexResource();

	///- cbv resource
	CreateTransformaitonResource();
	CreateMaterialResource();



}



/// ===================================================
/// 描画
/// ===================================================
void Sprite::Draw() {
	ID3D12GraphicsCommandList* commandList = DxCommand::GetInstance()->GetList();

	///- 使用するpipelineの設定
	PipelineStateObjectManager::GetInstance()->SetCommandList("Object3d", commandList);

	///- cbvのstructのデータを設定
	///- material
	materialData_->color = color;
	materialData_->uvTransform = Mat4::MakeAffine(uvScale_, uvRotate_, uvTranslate_);
	///- transformation
	transformationMatrixData_->World = worldTransform_.matTransform;
	Matrix4x4 viewMatrixSprite = Matrix4x4::MakeIdentity();
	Matrix4x4 projectionMatrixSprite = Matrix4x4::MakeOrthographicMatrix(0.0f, 0.0f, float(kWindowSize.x), float(kWindowSize.y), 0.0f, 100.0f);
	Matrix4x4 wvpMatrixSprite = worldTransform_.matTransform * (viewMatrixSprite * projectionMatrixSprite);
	transformationMatrixData_->WVP = wvpMatrixSprite;

	///- 頂点データに中心点のプラスして書き込む
	for(uint32_t index = 0; index < 4; index++) {
		Vec4f pos = { position.x, position.y, 0.0f, 0.0f };
		vertexData_[index].position = localVertex_[index] + pos;
	}

	///- commandListに設定
	commandList->IASetVertexBuffers(0, 1, &vertexBufferView_);
	commandList->IASetIndexBuffer(&indexBufferView_);

	commandList->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
	commandList->SetGraphicsRootConstantBufferView(1, transformationMatrixResource_->GetGPUVirtualAddress());

	TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(2, "uvChecker");
	Light::GetInstance()->SetConstantBuffer(3, commandList);

	///- DrawCall
	commandList->DrawIndexedInstanced(6, 1, 0, 0, 0);

}



/// ===================================================
/// 頂点リソースの生成
/// ===================================================
void Sprite::CreateVertexResource() {
	/// ---------------------------------
	/// ↓ 頂点リソースの初期化
	/// ---------------------------------
	vertexResource_ = dxCommon_->CreateBufferResource(sizeof(VertexData) * 4);

	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress(); //- リソースの先頭アドレス
	vertexBufferView_.SizeInBytes = sizeof(VertexData) * 4;						//- 使用する頂点のデータ
	vertexBufferView_.StrideInBytes = sizeof(VertexData);						//- 使用する頂点の1つ分のデータ

	vertexData_ = nullptr;
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));

	///- 頂点情報
	vertexData_[0].position = { 0.0f,360.0f,0.0f,1.0f };
	vertexData_[0].texcoord = { 0.0f,1.0f };
	vertexData_[1].position = { 0.0f,0.0f,0.0f,1.0f };
	vertexData_[1].texcoord = { 0.0f,0.0f };
	vertexData_[2].position = { 640.0f,360.0f,0.0f,1.0f };
	vertexData_[2].texcoord = { 1.0f,1.0f };
	vertexData_[3].position = { 640.0f,0.0f,0.0f,1.0f };
	vertexData_[3].texcoord = { 1.0f,0.0f };

	///- 頂点座標を保存
	localVertex_.resize(4);
	for(uint32_t index = 0; index < 4; index++) {
		localVertex_[index] = vertexData_[index].position;
	}

}


/// ===================================================
/// インデックスリソースの生成
/// ===================================================
void Sprite::CreateIndexResource() {
	
	/// ---------------------------------
	/// ↓ Indexリソースの初期化
	/// ---------------------------------
	indexResource_ = dxCommon_->CreateBufferResource(sizeof(uint32_t) * 6);

	indexBufferView_.BufferLocation = indexResource_->GetGPUVirtualAddress();	//- リソースの先頭アドレス
	indexBufferView_.SizeInBytes = sizeof(uint32_t) * 6;						//- 使用するバイト数
	indexBufferView_.Format = DXGI_FORMAT_R32_UINT;								//- indexの型 : int or uintなど

	///- indexResourceに書き込み
	indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&indexData_));

	indexData_[0] = 0;
	indexData_[1] = 1;
	indexData_[2] = 2;

	indexData_[3] = 1;
	indexData_[4] = 3;
	indexData_[5] = 2;

}


/// ===================================================
/// マテリアルリソースの生成
/// ===================================================
void Sprite::CreateMaterialResource() {

	///- resourceの生成
	materialResource_ = dxCommon_->CreateBufferResource(sizeof(Material));

	///- データのmap
	materialData_ = nullptr;
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));

	///- データの初期化
	materialData_->color = { 1.0f,1.0f,1.0f,1.0f };
	materialData_->enableLighting = false;
	materialData_->uvTransform = Mat4::MakeIdentity();

}


/// ===================================================
/// 行列リソースの生成
/// ===================================================
void Sprite::CreateTransformaitonResource() {

	///- resourceの生成
	transformationMatrixResource_ = dxCommon_->CreateBufferResource(sizeof(TransformMatrix));

	///- データのmap
	transformationMatrixData_ = nullptr;
	transformationMatrixResource_->Map(0, nullptr, reinterpret_cast<void**>(&transformationMatrixData_));

	///- データの初期化
	transformationMatrixData_->World = Matrix4x4::MakeIdentity();
	transformationMatrixData_->WVP = Matrix4x4::MakeIdentity();

	worldTransform_.Initialize();

	Matrix4x4 viewMatrixSprite = Matrix4x4::MakeIdentity();
	Matrix4x4 projectionMatrixSprite = Matrix4x4::MakeOrthographicMatrix(0.0f, 0.0f, float(kWindowSize.x), float(kWindowSize.y), 0.0f, 100.0f);
	Matrix4x4 wvpMatrixSprite = worldTransform_.matTransform * (viewMatrixSprite * projectionMatrixSprite);
	transformationMatrixData_->WVP = wvpMatrixSprite;

}

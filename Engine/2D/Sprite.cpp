#include "Sprite.h"

#include "DirectXCommon.h"
#include "Environment.h"
#include "ImGuiManager.h"
#include "DxCommand.h"
#include <TextureManager.h>
#include <DirectionalLight.h>
#include <PipelineStateObjectManager.h>
#include <DxDescriptors.h>

Sprite::Sprite() {}
Sprite::~Sprite() {
	materialResource_.Reset();
	worldMatrixResource_.Reset();
	vertexResource_.Reset();
}


/// ===================================================
/// 初期化
/// ===================================================
void Sprite::Initialize() {
	dxCommon_ = DirectXCommon::GetInstance();
	dxDescriptors_ = DxDescriptors::GetInstance();


	position = { 0.0f,0.0f };
	color = { 1.0f,1.0f,1.0f,1.0f };
	uvScale_ = { 1.0f, 1.0f, 1.0f };
	uvRotate_ = { 0.0f, 0.0f, 0.0f };
	uvTranslate_ = { 0.0f, 0.0f, 0.0f };


	///- vertex index resource
	CreateVertexResource();
	CreateIndexResource();

	///- cbv resource
	CreateWorldMatrixResource();
	CreateMaterialResource();
	CreateViewProjectionResource();


}



/// ===================================================
/// 描画
/// ===================================================
void Sprite::Draw(const WorldTransform& worldTransform, const Mat4& uvTransfrom) {
	
	///- インスタンスを超えて描画できない
	if(instanceCount_ > kMaxInstanceCount_) {
		assert(false);
	}

	///- material
	materialData_[instanceCount_].color = color;
	materialData_[instanceCount_].uvTransform = uvTransfrom;
	///- transformation
	*worldMatrixData_ = worldTransform.matTransform;

	///- 頂点データに中心点のプラスして書き込む
	for(uint32_t index = 0; index < 4; index++) {
		Vec4f pos = { position.x, position.y, 0.0f, 0.0f };
		vertexData_[index].position = localVertex_[index] + pos;
	}
	
	instanceCount_++;
	
}



/// ===================================================
/// 描画前処理
/// ===================================================
void Sprite::PreDraw() {
	instanceCount_ = 0;

}


/// ===================================================
/// 描画後処理
/// ===================================================
void Sprite::PostDraw() {
	ID3D12GraphicsCommandList* commandList = DxCommand::GetInstance()->GetList();

	///- 使用するpipelineの設定
	PipelineStateObjectManager::GetInstance()->SetCommandList("Sprite", commandList);

	///- commandListに設定
	commandList->IASetVertexBuffers(0, 1, &vertexBufferView_);
	commandList->IASetIndexBuffer(&indexBufferView_);

	commandList->SetGraphicsRootDescriptorTable(0, worldMatrixGpuHandle_);
	commandList->SetGraphicsRootConstantBufferView(1, viewProjectionResource_->GetGPUVirtualAddress());
	commandList->SetGraphicsRootDescriptorTable(3, materialGpuHandle_);

	TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(2, "uvChecker");

	///- DrawCall
	commandList->DrawIndexedInstanced(6, instanceCount_, 0, 0, 0);



}

void Sprite::SetScale(const Vec3f& scale) {
	worldTransform_.scale = scale;
	worldTransform_.UpdateMatTransform();
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
	vertexData_[0].position = { 0.0f,1.0f,0.0f,1.0f };
	vertexData_[0].texcoord = { 0.0f,1.0f };
	vertexData_[1].position = { 0.0f,0.0f,0.0f,1.0f };
	vertexData_[1].texcoord = { 0.0f,0.0f };
	vertexData_[2].position = { 1.0f,1.0f,0.0f,1.0f };
	vertexData_[2].texcoord = { 1.0f,1.0f };
	vertexData_[3].position = { 1.0f,0.0f,0.0f,1.0f };
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
	materialResource_ = dxCommon_->CreateBufferResource(sizeof(Material) * kMaxInstanceCount_);

	///- データのmap
	materialData_ = nullptr;
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));

	///- データの初期化
	for(int index = 0u; index < kMaxInstanceCount_; ++index) {
		materialData_[index].color = { 1.0f,1.0f,1.0f,1.0f };
		materialData_[index].enableLighting = false;
		materialData_[index].uvTransform = Mat4::MakeIdentity();
	}

	///- srvの作成
	D3D12_SHADER_RESOURCE_VIEW_DESC materialDesc{};
	materialDesc.Format = DXGI_FORMAT_UNKNOWN;
	materialDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	materialDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	materialDesc.Buffer.FirstElement = 0;
	materialDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	materialDesc.Buffer.NumElements = kMaxInstanceCount_;
	materialDesc.Buffer.StructureByteStride = sizeof(Material);

	D3D12_CPU_DESCRIPTOR_HANDLE materialCpuHandle = dxDescriptors_->GetCPUDescriptorHandle();
	materialGpuHandle_ = dxDescriptors_->GetGPUDescriptorHandle();
	dxDescriptors_->AddSrvUsedCount();
	dxCommon_->GetDevice()->CreateShaderResourceView(materialResource_.Get(), &materialDesc, materialCpuHandle);


}


/// ===================================================
/// 行列リソースの生成
/// ===================================================
void Sprite::CreateWorldMatrixResource() {

	///- resourceの生成
	worldMatrixResource_ = dxCommon_->CreateBufferResource(sizeof(Matrix4x4) * kMaxInstanceCount_);

	///- データのmap
	worldMatrixData_ = nullptr;
	worldMatrixResource_->Map(0, nullptr, reinterpret_cast<void**>(&worldMatrixData_));

	///- データの初期化
	for(int index = 0u; index < kMaxInstanceCount_; ++index) {
		worldMatrixData_[index] = Matrix4x4::MakeIdentity();
	}

	worldTransform_.Initialize();

	Matrix4x4 viewMatrixSprite = Matrix4x4::MakeIdentity();
	Matrix4x4 projectionMatrixSprite = Matrix4x4::MakeOrthographicMatrix(0.0f, 0.0f, float(kWindowSize.x), float(kWindowSize.y), 0.0f, 100.0f);
	Matrix4x4 wvpMatrixSprite = worldTransform_.matTransform * (viewMatrixSprite * projectionMatrixSprite);

	D3D12_SHADER_RESOURCE_VIEW_DESC worldMatrixDesc{};
	worldMatrixDesc.Format = DXGI_FORMAT_UNKNOWN;
	worldMatrixDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	worldMatrixDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	worldMatrixDesc.Buffer.FirstElement = 0;
	worldMatrixDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	worldMatrixDesc.Buffer.NumElements = kMaxInstanceCount_;
	worldMatrixDesc.Buffer.StructureByteStride = sizeof(Matrix4x4);

	D3D12_CPU_DESCRIPTOR_HANDLE worldMatrixCpuHandle = dxDescriptors_->GetCPUDescriptorHandle();
	worldMatrixGpuHandle_ = dxDescriptors_->GetGPUDescriptorHandle();
	dxDescriptors_->AddSrvUsedCount();
	dxCommon_->GetDevice()->CreateShaderResourceView(worldMatrixResource_.Get(), &worldMatrixDesc, worldMatrixCpuHandle);

}



void Sprite::CreateViewProjectionResource() {
	
	viewProjectionResource_ = dxCommon_->CreateBufferResource(sizeof(Matrix4x4));

	viewProjectionData_ = nullptr;
	viewProjectionResource_->Map(0, nullptr, reinterpret_cast<void**>(&viewProjectionData_));

	Matrix4x4 viewMatrix = Matrix4x4::MakeIdentity();
	Matrix4x4 projectionMatrix = Matrix4x4::MakeOrthographicMatrix(0.0f, 0.0f, float(kWindowSize.x), float(kWindowSize.y), 0.0f, 100.0f);
	*viewProjectionData_ = viewMatrix * projectionMatrix;

}

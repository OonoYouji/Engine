#include "Terrain.h"

#include <array>

#include <DxCommand.h>
#include <DxDescriptors.h>
#include <TextureManager.h>
#include "ImGuiManager.h"
#include "Engine.h"
#include "Input.h"


Terrain::Terrain() {}
Terrain::~Terrain() {
	///- 解放処理
	wvpResource_.Reset();
	materialResource_.Reset();
	vertexResource_.Reset();
}



void Terrain::Init() {
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	///- 頂点数の調整
	int indexCount = (kSubdivision_ + 1) * (kSubdivision_ + 1);
	indexCount *= 6;	//- 三角形1個分の頂点数をかける
	indexData_.resize(indexCount);

	int vertexCount = (kSubdivision_ + 1) * (kSubdivision_ + 1);
	//vertexCount *= 4;
	vertexData_.resize(vertexCount);


	/// -------------------------------------------------
	/// ↓ 頂点計算
	/// ------------------------------------------------- 


	uint32_t indices[kSubdivision_ + 1][kSubdivision_ + 1]{};
	int number = 0;
	for(uint32_t row = 0; row < kSubdivision_ + 1; row++) {
		for(uint32_t col = 0; col < kSubdivision_ + 1; col++) {
			indices[row][col] = number;
			++number;
		}
	}




	/// -----------------------
	/// ↓ 頂点インデックス
	/// -----------------------

	for(uint32_t row = 0; row < kSubdivision_; row++) {
		for(uint32_t col = 0; col < kSubdivision_; col++) {
			uint32_t startIndex = (row * kSubdivision_ + col) * 6;

			indexData_[startIndex + 0] = indices[row + 0][col + 0];
			indexData_[startIndex + 1] = indices[row + 1][col + 0];
			indexData_[startIndex + 2] = indices[row + 0][col + 1];
			indexData_[startIndex + 3] = indices[row + 1][col + 1];
			indexData_[startIndex + 4] = indices[row + 0][col + 1];
			indexData_[startIndex + 5] = indices[row + 1][col + 0];

		}
	}



	/// -----------------------
	/// ↓ 頂点データ
	/// -----------------------

	uint32_t index = 0;
	for(int32_t row = 0; row <= kSubdivision_; row++) {
		for(int32_t col = 0; col <= kSubdivision_; col++) {
			

			/// -----------------------
			/// ↓ Position
			/// -----------------------

			vertexData_[index + 0].position = {
				float(col), 0.0f,
				float(row), 1.0f
			};


			/// -----------------------
			/// ↓ Texcoord
			/// -----------------------


			vertexData_[index + 0].texcoord = {
				float(col + 0) / float(kSubdivision_ + 1),
				1.0f - (float(row + 0) / float(kSubdivision_ + 1))
			};


			///- 地形の中心が原点になるように移動
			vertexData_[index].position -= Vec4f{
				static_cast<float>(kSubdivision_ + 1) / 2.0f,
				0.0f,
				static_cast<float>(kSubdivision_ + 1) / 2.0f,
				0.0f
			};


			++index;

		}
	}




	/// ------------------------------------------------- 
	/// ↓ リソースの生成
	/// ------------------------------------------------- 


	/// -------------------------
	/// ↓ VertexResource
	/// -------------------------

	///- 頂点数分確保
	vertexResource_.Attach(dxCommon->CreateBufferResource(sizeof(VertexData) * vertexData_.size()));

	///- vertexBufferViewの作成
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	vertexBufferView_.SizeInBytes = UINT(sizeof(VertexData) * vertexData_.size());
	vertexBufferView_.StrideInBytes = sizeof(VertexData);

	///- 頂点データの書き込み
	pVertexData_ = vertexData_.data();
	vertexResource_->Map(0, nullptr, &pVertexMappedData_);
	memcpy(pVertexMappedData_, pVertexData_, vertexData_.size() * sizeof(VertexData));
	vertexResource_->Unmap(0, nullptr);



	/// -------------------------
	/// ↓ IndexResource
	/// -------------------------

	///- 頂点数分確保
	indexResource_.Attach(dxCommon->CreateBufferResource(sizeof(uint32_t) * indexData_.size()));
	///- Bufferの設定
	indexBuffer_.BufferLocation = indexResource_->GetGPUVirtualAddress();
	indexBuffer_.SizeInBytes = UINT(sizeof(uint32_t) * indexData_.size());
	indexBuffer_.Format = DXGI_FORMAT_R32_UINT;

	///- Resourceに対し情報を書き込む
	pIndexData_ = indexData_.data();
	indexResource_->Map(0, nullptr, &pIndexMappedData_);
	memcpy(pIndexMappedData_, pIndexData_, sizeof(uint32_t) * indexData_.size());
	indexResource_->Unmap(0, nullptr);



	/// -------------------------
	/// ↓ MaterialResource
	/// -------------------------

	materialResource_.Attach(dxCommon->CreateBufferResource(sizeof(Vector4)));
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
	*materialData_ = { 1.0f,1.0f,1.0f,1.0f };



	/// -------------------------
	/// ↓ wvpResource
	/// -------------------------

	wvpResource_.Attach(dxCommon->CreateBufferResource(sizeof(Matrix4x4)));
	wvpResource_->Map(0, nullptr, reinterpret_cast<void**>(&wvpData_));
	*wvpData_ = Matrix4x4::MakeIdentity(); //- とりあえずの単位行列


	




	/// ------------------------------------------------- 
	/// ↓ その他メンバ変数の初期化
	/// ------------------------------------------------- 


	worldTransform_.Init();
	//worldTransform_.scale = { 0.001f,1.0f,0.001f };
	color_ = { 1.0f,1.0f,1.0f,1.0f };

}



void Terrain::Update() {
	///- ImGuiでデバッグ
#ifdef _DEBUG
	ImGui::Begin("Terrain");

	ImGui::DragFloat3("scale", &worldTransform_.scale.x, 0.25f);
	ImGui::DragFloat3("rotate", &worldTransform_.rotate.x, 1.0f / 64.0f);
	ImGui::DragFloat3("translate", &worldTransform_.translate.x, 0.25f);
	ImGui::ColorEdit4("color", &color_.x);

	static int index = 0;
	ImGui::SliderInt("vertexIndex", &index, 0, static_cast<int>(vertexData_.size() - 1));
	ImGui::DragFloat4("vertex", &vertexData_[index].position.x, 0.25f);
	ImGui::DragFloat2("texcoord", &vertexData_[index].texcoord.x, 0.0f);

	ImGui::End();
#endif // _DEBUG


	/// -----------------------------------
	/// ↓ 更新処理ここから
	/// -----------------------------------




}


void Terrain::Draw() {
	ID3D12GraphicsCommandList* commandList = DxCommand::GetInstance()->GetList();

	commandList->IASetVertexBuffers(0, 1, &vertexBufferView_);
	commandList->IASetIndexBuffer(&indexBuffer_);

	///- データの書き込み

	///- 頂点情報
	memcpy(pVertexMappedData_, pVertexData_, vertexData_.size() * sizeof(VertexData));
	memcpy(pIndexMappedData_, pIndexData_, sizeof(uint32_t) * indexData_.size());

	///- 色情報
	*materialData_ = color_;
	///- 行列情報
	worldTransform_.MakeWorldMatrix();
	*wvpData_ = worldTransform_.worldMatrix * Engine::GetCamera()->GetVpMatrix();


	///- 各種設定
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	commandList->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
	commandList->SetGraphicsRootConstantBufferView(1, wvpResource_->GetGPUVirtualAddress());
	//commandList->SetGraphicsRootDescriptorTable(2, DirectXCommon::GetInstance()->GetTextureSrvHandleGPU());
	TextureManager::GetInstance()->SetGraphicsRootDescriptorTable("uvChecker");

	///- 描画 (DrawCall)
	commandList->DrawIndexedInstanced(UINT(indexData_.size()), 1, 0, 0, 0);

}

#include "Terrain.h"

#include <array>
#include "ImGuiManager.h"
#include "Engine.h"

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
	int vertexCount = kVerticalDivisionNum_ + 1; //- 縦の分割数+1
	vertexCount *= kHorizontalDivisionNum_ + 1;  //- 横の分割数+1
	vertexCount *= 6;	//- 三角形1個分の頂点数をかける
	vertexData_.resize(vertexCount);


	///- 頂点数分確保
	vertexResource_.Attach(dxCommon->CreateBufferResource(sizeof(VertexData) * vertexCount));

	///- vertexBufferViewの作成
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	vertexBufferView_.SizeInBytes = sizeof(VertexData) * vertexCount;
	vertexBufferView_.StrideInBytes = sizeof(VertexData);


	///- 頂点データの書き込み
	pData_ = vertexData_.data();
	vertexResource_->Map(0, nullptr, &pMappedData_);
	memcpy(pMappedData_, pData_, vertexData_.size() * sizeof(VertexData));
	vertexResource_->Unmap(0, nullptr);

	///- 頂点計算
	int index = 0;
	for(int32_t row = 0; row <= kVerticalDivisionNum_; row++) {
		for(int32_t col = 0; col <= kHorizontalDivisionNum_; col++) {

			///- 0割り防止
			if(col == 0) {
				for(uint32_t i = 0; i < 6; i++) {
					vertexData_[index + 1].texcoord.x = 0.0f;
				}
			} else {
				for(uint32_t i = 0; i < 6; i++) {
					vertexData_[index + i].texcoord.x =
						float(kHorizontalDivisionNum_) / float(row + (i % 2));
				}
			}

			///- 0割り防止
			if(row == 0) {
				for(uint32_t i = 0; i < 6; i++) {
					vertexData_[index + 1].texcoord.y = 1.0f;
				}
			} else {
				for(uint32_t i = 0; i < 6; i++) {
					vertexData_[index + 1].texcoord.y = 1.0f;
				}
				//vertexData_[index + 0].texcoord.y = 
			}


			//- 左下
			//vertexData_[index + 0].texcoord = {
			//	float(kHorizontalDivisionNum_) / float(col),
			//	1.0f - (float(kVerticalDivisionNum_) / float(row))
			//};
			////- 左上
			//vertexData_[index + 1].texcoord = {
			//	float(kHorizontalDivisionNum_) / float(col),
			//	1.0f - (float(kVerticalDivisionNum_) / float(row - 1))
			//};
			////- 右上
			//vertexData_[index + 2].texcoord = {
			//	float(kHorizontalDivisionNum_) / float(col + 1),
			//	1.0f - (float(kVerticalDivisionNum_) / float(row - 1))
			//};
			////- 右上
			//vertexData_[index + 3].texcoord = vertexData_[index + 2].texcoord;
			////- 右下
			//vertexData_[index + 4].texcoord = {
			//	float(kHorizontalDivisionNum_) / float(col + 1),
			//	1.0f - (float(kVerticalDivisionNum_) / float(row))
			//};
			////- 左下
			//vertexData_[index + 5].texcoord = vertexData_[index + 0].texcoord;

			///- positionの計算
			for(uint32_t i = 0; i < 6; i++) {
				vertexData_[index + i].position.x = float(col + (i % 2));
				vertexData_[index + i].position.y = 0.0f;
				if(i >= 2 && i <= 4) {
					vertexData_[index + i].position.z = float(row);
				} else {
					vertexData_[index + i].position.z = float(row + 1);
				}
				vertexData_[index + i].position.w = 1.0f;

			}

			///- texcoordの計算
			for(uint32_t i = 0; i < 6; i++) {
			}

			///- 地形の中心が原点になるように移動
			for(uint32_t i = 0; i < 6; i++) {
				vertexData_[index + i].position -= Vec4f{
					static_cast<float>(kHorizontalDivisionNum_ + 1) / 2.0f,
					0.0f,
					static_cast<float>(kVerticalDivisionNum_ + 1) / 2.0f,
					0.0f
				};
			}

			///- 頂点数の増加
			index += 6;
		}
	}

	///- マテリアルリソースの生成; 情報の書き込み
	materialResource_.Attach(dxCommon->CreateBufferResource(sizeof(Vector4)));
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
	*materialData_ = { 1.0f,1.0f,1.0f,1.0f };


	///- 行列リソースの生成; 書き込み
	wvpResource_.Attach(dxCommon->CreateBufferResource(sizeof(Matrix4x4)));
	wvpResource_->Map(0, nullptr, reinterpret_cast<void**>(&wvpData_));
	*wvpData_ = Matrix4x4::MakeIdentity(); //- とりあえずの単位行列


	///- 
	worldTransform_.Init();
	color_ = { 1.0f,0.0f,0.0f,1.0f };

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
	ImGui::SliderInt("vertexIndex", &index, 0, static_cast<int>(vertexData_.size()));
	ImGui::DragFloat4("vertex", &vertexData_[index].position.x, 0.25f);

	ImGui::End();
#endif // _DEBUG


	/// -----------------------------------
	/// ↓ 更新処理ここから
	/// -----------------------------------




}


void Terrain::Draw() {
	ID3D12GraphicsCommandList* commandList = DirectXCommon::GetInstance()->GetCommandList();

	commandList->IASetVertexBuffers(0, 1, &vertexBufferView_);

	///- データの書き込み

	///- 頂点情報
	memcpy(pMappedData_, pData_, vertexData_.size() * sizeof(VertexData));

	///- 色情報
	*materialData_ = color_;
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

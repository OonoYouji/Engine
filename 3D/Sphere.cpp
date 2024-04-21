#include "Sphere.h"

#include <numbers>

#include <TextureManager.h>
#include "ImGuiManager.h"
#include "Engine.h"
#include "DxCommand.h"

using namespace std::numbers;


Sphere::Sphere() {}
Sphere::~Sphere() {
	wvpResource_.Reset();
	materialResource_.Reset();
	vertexResource_.Reset();
}



void Sphere::Init() {
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	///- 分割数の指定
	subdivision_ = 16;

	///- 頂点数の計算
	///- 縦の分割数 x 横の分割数 x 6
	vertexData_.resize(subdivision_ * subdivision_ * 4);


	///- 頂点座標の計算
	const float kLonEvery = (2.0f * pi_v<float>) / float(subdivision_); // 経度分割1つ分の角度
	const float kLatEvery = pi_v<float> / float(subdivision_); // 緯度分割1つ分の角度

	// 緯度の方向に分割 -π/2 ~ π/2
	for(uint32_t latIndex = 0; latIndex < subdivision_; latIndex++) {
		float lat = -pi_v<float> / 2.0f + kLatEvery * latIndex; // 現在の緯度

		// 経度の方向に分割 0 ~ 2π
		for(uint32_t lonIndex = 0; lonIndex < subdivision_; lonIndex++) {
			uint32_t startIndex = (latIndex * subdivision_ + lonIndex) * 4;
			float lon = lonIndex * kLonEvery; // 現在の経度


			// world座標系でのa,b,cを求める
			// a
			vertexData_[startIndex + 0].position = {
				(std::cosf(lat) * std::cosf(lon)) ,
				(std::sinf(lat)) ,
				(std::cosf(lat) * std::sinf(lon)),
				1.0f
			};

			vertexData_[startIndex + 0].texcoord = {
				float(lonIndex + 0) / float(subdivision_),
				1.0f - float(latIndex + 0) / float(subdivision_)
			};

			// b
			vertexData_[startIndex + 1].position = {
				(std::cosf(lat + kLatEvery) * std::cosf(lon)) ,
				(std::sinf(lat + kLatEvery)) ,
				(std::cosf(lat + kLatEvery) * std::sinf(lon)),
				1.0f
			};

			vertexData_[startIndex + 1].texcoord = {
				float(lonIndex + 0) / float(subdivision_),
				1.0f - float(latIndex + 1) / float(subdivision_)
			};

			// c
			vertexData_[startIndex + 2].position = {
				(std::cosf(lat) * std::cosf(lon + kLonEvery)) ,
				(std::sinf(lat)) ,
				(std::cosf(lat) * std::sinf(lon + kLonEvery)),
				1.0f
			};

			vertexData_[startIndex + 2].texcoord = {
				float(lonIndex + 1) / float(subdivision_),
				1.0f - float(latIndex + 0) / float(subdivision_)
			};

			// d
			vertexData_[startIndex + 3].position = {
				(std::cosf(lat + kLatEvery) * std::cosf(lon + kLonEvery)) ,
				(std::sinf(lat + kLatEvery)) ,
				(std::cosf(lat + kLatEvery) * std::sinf(lon + kLonEvery)),
				1.0f
			};

			vertexData_[startIndex + 3].texcoord = {
				float(lonIndex + 1) / float(subdivision_),
				1.0f - float(latIndex + 1) / float(subdivision_)
			};

		}
	}


	/// -----------------------------------------------
	/// ↓ resoruceの初期化
	/// -----------------------------------------------


	/// -----------------------------------------------
	/// ↓ 頂点リソース
	/// -----------------------------------------------

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



	/// -----------------------------------------------
	/// ↓ インデックスリソース
	/// -----------------------------------------------


	///- 頂点の割り振り
	for(uint32_t latIndex = 0; latIndex < subdivision_; latIndex++) {
		for(uint32_t lonIndex = 0; lonIndex < subdivision_; lonIndex++) {
			uint32_t startIndex = (latIndex * subdivision_ + lonIndex) * 4;

			indexData_.push_back(startIndex + 0);
			indexData_.push_back(startIndex + 1);
			indexData_.push_back(startIndex + 2);
			indexData_.push_back(startIndex + 1);
			indexData_.push_back(startIndex + 3);
			indexData_.push_back(startIndex + 2);

		}
	}

	///- リソースの生成
	indexResource_.Attach(dxCommon->CreateBufferResource(sizeof(uint32_t) * indexData_.size()));

	///- bufferの設定
	indexBuffer_.BufferLocation = indexResource_->GetGPUVirtualAddress();
	indexBuffer_.SizeInBytes = UINT(sizeof(uint32_t) * indexData_.size());
	indexBuffer_.Format = DXGI_FORMAT_R32_UINT;

	///- resourceに書き込み
	pIndexData_ = indexData_.data();
	indexResource_->Map(0, nullptr, &pIndexMappedData_);
	memcpy(pIndexMappedData_, pIndexData_, indexData_.size() * sizeof(uint32_t));
	indexResource_->Unmap(0, nullptr);


	/// -----------------------------------------------
	/// ↓ マテリアルリソース
	/// -----------------------------------------------

	///- マテリアルリソースの生成; 情報の書き込み
	materialResource_.Attach(dxCommon->CreateBufferResource(sizeof(Vector4)));
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
	*materialData_ = { 1.0f,1.0f,1.0f,1.0f };



	/// -----------------------------------------------
	/// ↓ 行列リソース
	/// -----------------------------------------------

	///- 行列リソースの生成; 書き込み
	wvpResource_.Attach(dxCommon->CreateBufferResource(sizeof(Matrix4x4)));
	wvpResource_->Map(0, nullptr, reinterpret_cast<void**>(&wvpData_));
	*wvpData_ = Matrix4x4::MakeIdentity(); //- とりあえずの単位行列


	///- 
	worldTransform_.Init();
	//worldTransform_.scale = { 0.001f,1.0f,0.001f };
	color_ = { 1.0f,1.0f,1.0f,1.0f };



}



void Sphere::Draw() {

	ID3D12GraphicsCommandList* commandList = DxCommand::GetInstance()->GetList();

	commandList->IASetVertexBuffers(0, 1, &vertexBufferView_);
	commandList->IASetIndexBuffer(&indexBuffer_);


	///- 頂点情報
	memcpy(pMappedData_, pData_, vertexData_.size() * sizeof(VertexData));
	memcpy(pIndexMappedData_, pIndexData_, indexData_.size() * sizeof(uint32_t));

	///- 行列情報
	worldTransform_.MakeWorldMatrix();
	*wvpData_ = worldTransform_.worldMatrix * Engine::GetCamera()->GetVpMatrix();
	///- 色情報
	*materialData_ = color_;

	///- 形状を設定; PSOに設定している物とはまだ別; 同じものを設定すると考えておけばOK
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	///- マテリアルのCBufferの場所を設定
	commandList->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
	///- wvp用のCBufferの場所を設定
	commandList->SetGraphicsRootConstantBufferView(1, wvpResource_->GetGPUVirtualAddress());
	///- DescriptorTableを設定する
	TextureManager::GetInstance()->SetGraphicsRootDescriptorTable("uvChecker");

	///- 描画 (DrawCall)
	commandList->DrawIndexedInstanced(UINT(indexData_.size()), 1, 0, 0, 0);

}


#include "Sphere.h"

#include <numbers>

#include <TextureManager.h>
#include "ImGuiManager.h"
#include "Engine.h"
#include "DxCommand.h"
#include "DirectionalLight.h"

using namespace std::numbers;


Sphere::Sphere() {}
Sphere::~Sphere() {
	transformMatrixResource_.Reset();
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


	for(uint32_t i = 0; i < vertexData_.size(); i++) {
		vertexData_[i].normal = Vec3f::Normalize(Vec3f::Convert4To3(vertexData_[i].position));
	}



	/// -----------------------------------------------
	/// ↓ resoruceの初期化
	/// -----------------------------------------------


	/// -----------------------------------------------
	/// ↓ 頂点リソース
	/// -----------------------------------------------

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
	indexResource_ = dxCommon->CreateBufferResource(sizeof(uint32_t) * indexData_.size());

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
	materialResource_ = dxCommon->CreateBufferResource(sizeof(Material));
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
	materialData_->color = { 1.0f,1.0f,1.0f,1.0f };
	materialData_->enableLighting = true;



	/// -----------------------------------------------
	/// ↓ 行列リソース
	/// -----------------------------------------------

	///- 行列リソースの生成; 書き込み
	transformMatrixResource_ = dxCommon->CreateBufferResource(sizeof(TransformMatrix));
	transformMatrixResource_->Map(0, nullptr, reinterpret_cast<void**>(&transformMatrixData_));
	transformMatrixData_->World = Matrix4x4::MakeIdentity(); //- とりあえずの単位行列
	transformMatrixData_->WVP = Matrix4x4::MakeIdentity(); //- とりあえずの単位行列


	///- 
	worldTransform_.Initialize();
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
	worldTransform_.UpdateWorldMatrix();
	transformMatrixData_->WVP = worldTransform_.worldMatrix * Engine::GetCamera()->GetVpMatrix();
	transformMatrixData_->World = worldTransform_.worldMatrix;
	///- 色情報
	materialData_->color = color_;

	///- 形状を設定; PSOに設定している物とはまだ別; 同じものを設定すると考えておけばOK
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	///- マテリアルのCBufferの場所を設定
	commandList->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
	///- wvp用のCBufferの場所を設定
	commandList->SetGraphicsRootConstantBufferView(1, transformMatrixResource_->GetGPUVirtualAddress());
	///- DescriptorTableを設定する
	TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(2, "uvChecker");
	///- Light
	Light::GetInstance()->SetConstantBuffer(3, commandList);

	///- 描画 (DrawCall)
	commandList->DrawIndexedInstanced(UINT(indexData_.size()), 1, 0, 0, 0);

}

void Sphere::DebugDraw() {
#ifdef _DEBUG
	ImGui::Begin("Sphere");

	if(ImGui::TreeNodeEx("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {

		ImGui::DragFloat3("Scale", &worldTransform_.scale.x, 0.025f);
		ImGui::DragFloat3("Rotation", &worldTransform_.rotate.x, 0.025f);
		ImGui::DragFloat3("Translation", &worldTransform_.translate.x, 0.25f);

		ImGui::TreePop();
	}

	ImGui::Separator();

	if(ImGui::TreeNodeEx("Parameter", ImGuiTreeNodeFlags_DefaultOpen)) {

		static bool enableLighting = static_cast<bool>(materialData_->enableLighting);
		ImGui::Checkbox("EnableLighting", &enableLighting);
		if(ImGui::IsItemEdited()) {
			materialData_->enableLighting = static_cast<int32_t>(enableLighting);
		}

		ImGui::TreePop();
	}

	ImGui::End();
#endif // _DEBUG
}


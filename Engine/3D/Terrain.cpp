#include "Terrain.h"

#include <array>

#include <Engine.h>
#include <PipelineStateObjectManager.h>
#include <DxCommand.h>
#include <DxDescriptors.h>
#include <TextureManager.h>
#include <ImGuiManager.h>
#include <Input.h>
#include <DirectionalLight.h>
#include <PerlinNoise.h>

#include <CreateName.h>


Terrain::Terrain() {
	SetName(CreateName(this));
}

Terrain::~Terrain() {
	///- 解放処理
	matrixResource_.Reset();
	materialResource_.Reset();
	vertexResource_.Reset();
	indexResource_.Reset();
	readBackResource_.Reset();
}



void Terrain::Init() {
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	/// -------------------------------------------------
	/// ↓ 頂点計算
	/// ------------------------------------------------- 


	rowSubdivision_ = 128;
	colSubdivision_ = 128;
	/// -----------------------
	/// ↓ 頂点インデックス
	/// -----------------------

	///- 頂点インデックスの設定用
	IndexDataCulc(rowSubdivision_, colSubdivision_);


	/// -----------------------
	/// ↓ 頂点データ
	/// -----------------------

	VertexDataCulc(rowSubdivision_, colSubdivision_);


	/// ------------------------------------------------- 
	/// ↓ リソースの生成
	/// ------------------------------------------------- 


	/// -------------------------
	/// ↓ VertexResource
	/// -------------------------

	CreateVertexResource(flattenedVertexData_.size());

	/// -------------------------
	/// ↓ IndexResource
	/// -------------------------

	CreateIndexResource(indexData_.size());



	/// -------------------------
	/// ↓ MaterialResource
	/// -------------------------

	materialResource_ = dxCommon->CreateBufferResource(sizeof(Material));
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
	materialData_->color = { 1.0f,1.0f,1.0f,1.0f };
	materialData_->enableLighting = true;


	/// -------------------------
	/// ↓ MatrixResource
	/// -------------------------

	matrixResource_ = dxCommon->CreateBufferResource(sizeof(TransformMatrix));
	matrixResource_->Map(0, nullptr, reinterpret_cast<void**>(&matrixData_));
	matrixData_->World = Matrix4x4::MakeIdentity();
	matrixData_->WVP = Matrix4x4::MakeIdentity();




	/// ------------------------------------------------- 
	/// ↓ その他メンバ変数の初期化
	/// ------------------------------------------------- 


	worldTransform_.Initialize();
	worldTransform_.scale = { 0.5f,12.0f,0.5f };
	//worldTransform_.translate.y = -100.0f;
	color_ = { 1.0f,1.0f,1.0f,1.0f };


	///- ノイズから地形の高さを計算

	noise_ = std::make_unique<PerlinNoise>(uint32_t(123));
	noisePower_ = 1.0f;

	///- 法線ベクトルを計算
	//NormalVector();
	GameObject::Initialize();

}



void Terrain::Update() {

	///- 最初に行う処理
	TransferFlattenedVertexData();


	/// -----------------------------------
	/// ↓ 更新処理ここから
	/// -----------------------------------

	ReadBack();
	worldTransform_.UpdateMatTransform();


}



/// ------------------------------------------
/// ↓ 
/// ------------------------------------------
void Terrain::Draw() {
	ID3D12GraphicsCommandList* commandList = DxCommand::GetInstance()->GetList();
	PipelineStateObjectManager::GetInstance()->SetCommandList("Terrain", commandList);

	///- 頂点情報
	memcpy(pVertexData_, flattenedVertexData_.data(), flattenedVertexData_.size() * sizeof(VertexData));
	memcpy(pIndexData_, indexData_.data(),  indexData_.size() * sizeof(uint32_t));

	///- 色情報
	materialData_->color = color_;

	///- 行列情報
	matrixData_->World = worldTransform_.matTransform;
	matrixData_->WVP = worldTransform_.matTransform * Engine::GetCamera()->GetVpMatrix();

	///- psoを設定
	commandList->IASetVertexBuffers(0, 1, &vertexBufferView_);
	commandList->IASetIndexBuffer(&indexBuffer_);

	///- 各種設定
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	commandList->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
	commandList->SetGraphicsRootConstantBufferView(1, matrixResource_->GetGPUVirtualAddress());

	TextureManager* textureManager = TextureManager::GetInstance();
	textureManager->SetGraphicsRootDescriptorTable(2, "dragon");	 ///- terrain
	textureManager->SetGraphicsRootDescriptorTable(3, "dragon");	 ///- heightMap
	textureManager->SetGraphicsRootDescriptorTableUAV(4, "128x128Texture");  ///- operation
	Light::GetInstance()->SetConstantBuffer(5, commandList);


	///- 描画 (DrawCall)
	commandList->DrawIndexedInstanced(UINT(indexData_.size()), 1, 0, 0, 0);

}



void Terrain::ImGuiDebug() {

	worldTransform_.ImGuiTreeNodeDebug();

	ImGui::Separator();

	if(ImGui::TreeNodeEx("Material", ImGuiTreeNodeFlags_DefaultOpen)) {

		ImGui::ColorEdit4("color", &color_.x);

		ImGui::TreePop();
	}

	ImGui::Separator();

	ImGui::SliderFloat("noisePower", &noisePower_, 0.0f, 1.0f);

	static int seed = 0;
	ImGui::SliderInt("seed", &seed, 0, static_cast<int>(std::pow(2, 32)));
	if(ImGui::IsItemEdited()) {
		noise_->ResetSheed(seed);
		for(uint32_t row = 0; row < vertexData_.size(); ++row) {
			for(uint32_t col = 0; col < vertexData_[0].size(); col++) {

				vertexData_[row][col].position.y =
					noise_->GetNoise(
						Vec2f{ vertexData_[row][col].position.x, vertexData_[row][col].position.z } / (float(rowSubdivision_) * 0.2f)
					) * (noisePower_);

			}
		}
	}



	ImGui::Spacing();

	if(ImGui::Button("Terrain Flatten")) {
		for(uint32_t row = 0; row < vertexData_.size(); ++row) {
			for(uint32_t col = 0; col < vertexData_[0].size(); col++) {
				vertexData_[row][col].position.y = 0.0f;
			}
		}
	}



	if(ImGui::Button("Output")) {
		OutputImage();
	}

}

/// ------------------------------------------
/// ↓ 法線ベクトルの計算
/// ------------------------------------------
void Terrain::NormalVector() {


	///- local頂点をworld座標に変換

	Vec3f worldVertex[3] = {
		Mat4::Transform(vertexData_[0][0].position, worldTransform_.matTransform),
		Mat4::Transform(vertexData_[1][0].position, worldTransform_.matTransform),
		Mat4::Transform(vertexData_[0][1].position, worldTransform_.matTransform)
	};


	normalVector_ = Vector3::Cross(worldVertex[1] - worldVertex[0], worldVertex[0] - worldVertex[2]);
	normalVector_ = Vector3::Normalize(normalVector_);

	distance_ = Vector3::Dot(worldVertex[0], normalVector_);


	///- 地形の各頂点の法線ベクトルを計算


	Vec3f vertexPos[3]{};
	int vertexIndex = 0;
	for(uint32_t index = 0; index < flattenedVertexData_.size(); index++) {

		///- 三角形の頂点
		vertexIndex = index;
		while(indexData_[vertexIndex] != index) {
			vertexIndex++;
		}


		switch(vertexIndex % 3) {
		case 0:
			for(uint8_t i = 0; i < 3; i++) {
				vertexPos[i] = Vec3f::Convert4To3(flattenedVertexData_[indexData_[vertexIndex + i]].position);
			}

			break;
		case 1:
			for(int8_t i = -1; i < 2; i++) {
				vertexPos[i + 1] = Vec3f::Convert4To3(flattenedVertexData_[indexData_[vertexIndex + i]].position);
			}

			break;
		case 2:
			for(int8_t i = -2; i < 1; i++) {
				vertexPos[i + 2] = Vec3f::Convert4To3(flattenedVertexData_[indexData_[vertexIndex + i]].position);
			}

			break;
		}



		flattenedVertexData_[index].normal =
			Vec3f::Normalize(Vec3f::Cross(vertexPos[0] - vertexPos[2], vertexPos[1] - vertexPos[0]));


	}



	for(uint32_t row = 0; row < vertexData_.size(); row++) {
		for(uint32_t col = 0; col < vertexData_[0].size(); col++) {
			vertexData_[row][col] =
				flattenedVertexData_[(vertexData_.size() * row) + col];
		}
	}


}



/// ------------------------------------------
/// ↓ インデックスデータの計算
/// ------------------------------------------
void Terrain::IndexDataCulc(uint32_t maxRow, uint32_t maxCol) {

	int indexCount = (maxRow) * (maxCol);
	indexCount *= 6;	//- 図形1個分の頂点数をかける(四角形)
	indexData_.resize(indexCount);


	///- 頂点インデックスの設定用
	//uint32_t indices[kSubdivision + 1][kSubdivision + 1]{};
	std::vector<std::vector<uint32_t>> indices;
	indices.resize(maxRow + 1);
	for(uint32_t i = 0; i < indices.size(); i++) {
		indices[i].resize(maxCol + 1);
	}

	int number = 0;
	for(uint32_t row = 0; row < uint32_t(maxRow + 1); row++) {
		for(uint32_t col = 0; col < uint32_t(maxCol + 1); col++) {
			indices[row][col] = number;
			++number;
		}
	}


	uint32_t startIndex = 0;
	for(uint32_t row = 0; row < uint32_t(maxRow); row++) {
		for(uint32_t col = 0; col < uint32_t(maxCol); col++) {

			indexData_[startIndex + 0] = indices[row + 0][col + 0];
			indexData_[startIndex + 1] = indices[row + 1][col + 0];
			indexData_[startIndex + 2] = indices[row + 0][col + 1];
			indexData_[startIndex + 3] = indices[row + 1][col + 1];
			indexData_[startIndex + 4] = indices[row + 0][col + 1];
			indexData_[startIndex + 5] = indices[row + 1][col + 0];

			startIndex += 6;
		}
	}


	pIndexData_ = indexData_.data();

}



/// ------------------------------------------
/// ↓ 頂点データの計算
/// ------------------------------------------
void Terrain::VertexDataCulc(uint32_t maxRow, uint32_t maxCol) {

	vertexData_.resize(maxRow + 1);
	for(uint32_t i = 0; i < vertexData_.size(); i++) {
		vertexData_[i].resize(maxCol + 1);
	}


	for(int32_t row = 0; row <= int32_t(maxRow); row++) {
		for(int32_t col = 0; col <= int32_t(maxCol); col++) {


			/// -----------------------
			/// ↓ Position
			/// -----------------------

			vertexData_[row][col].position = {
				float(col), 0.0f,
				float(row), 1.0f
			};


			/// -----------------------
			/// ↓ Texcoord
			/// -----------------------


			vertexData_[row][col].texcoord = {
				float(col + 0) / float(maxCol + 1),
				1.0f - (float(row + 0) / float(maxRow + 1))
			};



			/// -----------------------
			/// ↓ Normal
			/// -----------------------

			vertexData_[row][col].normal = Vec3f{ 0.0f,1.0f,0.0f };


			///- 地形の中心が原点になるように移動
			/*vertexData_[row][col].position -= Vec4f{
				static_cast<float>(maxCol + 1) / 2.0f,
				0.0f,
				static_cast<float>(maxRow + 1) / 2.0f,
				0.0f
			};*/

		}
	}


	TransferFlattenedVertexData();
	pVertexData_ = flattenedVertexData_.data();


}



/// ------------------------------------------
/// ↓ 
/// ------------------------------------------
void Terrain::CreateVertexResource(size_t flattendVertexSize) {

	///- 頂点数分確保
	if(vertexResource_.Get()) { vertexResource_.Reset(); }
	vertexResource_ = DirectXCommon::GetInstance()->CreateBufferResource(sizeof(VertexData) * flattendVertexSize);

	///- vertexBufferViewの作成
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	vertexBufferView_.SizeInBytes = UINT(sizeof(VertexData) * flattendVertexSize);
	vertexBufferView_.StrideInBytes = sizeof(VertexData);

	///- 頂点データの書き込み
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&pVertexData_));
	memcpy(pVertexData_, flattenedVertexData_.data(), sizeof(VertexData) * flattendVertexSize);

}



/// ------------------------------------------
/// ↓ 
/// ------------------------------------------
void Terrain::CreateIndexResource(size_t indexDataSize) {

	///- 頂点数分確保
	if(indexResource_.Get()) { indexResource_.Reset(); }
	indexResource_ = DirectXCommon::GetInstance()->CreateBufferResource(sizeof(uint32_t) * indexDataSize);
	///- Bufferの設定
	indexBuffer_.BufferLocation = indexResource_->GetGPUVirtualAddress();
	indexBuffer_.SizeInBytes = UINT(sizeof(uint32_t) * indexDataSize);
	indexBuffer_.Format = DXGI_FORMAT_R32_UINT;

	///- Resourceに対し情報を書き込む
	indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&pIndexData_));
	memcpy(pIndexData_, indexData_.data(), sizeof(uint32_t) * indexDataSize);

}


void Terrain::CreateReadBackResource() {

	///- CommandListとDeviceのポインタを取得
	ID3D12GraphicsCommandList* commandList = DxCommand::GetInstance()->GetList();
	ID3D12Device* device = DirectXCommon::GetInstance()->GetDevice();

	readBackResource_.Reset();

	///- リソースの取得; 読み戻しバッファの作成
	pTexture_ = TextureManager::GetInstance()->GetUavTextureResource("128x128Texture");

	///- テクスチャの情報を取得
	D3D12_RESOURCE_DESC textureDesc = pTexture_->GetDesc();

	///- 中間バッファの作成
	D3D12_PLACED_SUBRESOURCE_FOOTPRINT footprint{};
	UINT numRows;
	UINT64 rowSizeInBytes;
	UINT64 totalBytes;
	device->GetCopyableFootprints(&textureDesc, 0, 1, 0, &footprint, &numRows, &rowSizeInBytes, &totalBytes);
	rowPitch_ = footprint.Footprint.RowPitch;

	///- 読み戻し用バッファの作成
	D3D12_HEAP_PROPERTIES heapProps = {};
	heapProps.Type = D3D12_HEAP_TYPE_READBACK;

	D3D12_RESOURCE_DESC bufferDesc = {};
	bufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	bufferDesc.Width = totalBytes;
	bufferDesc.Height = 1;
	bufferDesc.DepthOrArraySize = 1;
	bufferDesc.MipLevels = 1;
	bufferDesc.SampleDesc.Count = 1;
	bufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;


	HRESULT hr = device->CreateCommittedResource(
		&heapProps,
		D3D12_HEAP_FLAG_NONE,
		&bufferDesc,
		D3D12_RESOURCE_STATE_COPY_DEST,
		nullptr,
		IID_PPV_ARGS(&readBackResource_)
	);
	assert(SUCCEEDED(hr));


	///- テクスチャの状態をD3D12_RESOURCE_STATE_COPY_SOURCEに変換
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = pTexture_;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_SOURCE;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	commandList->ResourceBarrier(1, &barrier);

	
	///- テクスチャからリードバックバッファへコピー
	D3D12_TEXTURE_COPY_LOCATION srcLocation = {};
	srcLocation.pResource = pTexture_;
	srcLocation.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
	srcLocation.SubresourceIndex = 0;

	D3D12_TEXTURE_COPY_LOCATION dstLocation = {};
	dstLocation.pResource = readBackResource_.Get();
	dstLocation.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
	dstLocation.PlacedFootprint = footprint;

	commandList->CopyTextureRegion(&dstLocation, 0, 0, 0, &srcLocation, nullptr);

	///- stateを元の状態に戻す
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_SOURCE;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
	commandList->ResourceBarrier(1, &barrier);

	///- コマンドの実行を待つ
	DirectXCommon::GetInstance()->CommnadExecuteAndWait();


}


void Terrain::ReadBack() {
	/*D3D12_RANGE readRange = { 0, 0 };
	readBackResource_->Map(0, &readRange, &readBackData_);





	readBackResource_->Unmap(0, nullptr);*/

}

void Terrain::OutputImage() {
	CreateReadBackResource();

	readBackResource_->Map(0, nullptr, &readBackData_);

	UINT height = pTexture_->GetDesc().Height;
	UINT width = static_cast<UINT>(pTexture_->GetDesc().Width);

	cv::Mat image(height, width, CV_8UC4, readBackData_, rowPitch_);

	// 輝度画像をファイルに保存
	cv::imwrite("./Resources/Images/128x128Texture.png", image);
	cv::imshow("128x128Texture.png", image);

	readBackResource_->Unmap(0, nullptr);
}



/// ------------------------------------------
/// ↓ 
/// ------------------------------------------
void Terrain::TransferVertexData() {

	for(uint32_t row = 0; row < vertexData_.size(); row++) {
		for(uint32_t col = 0; col < vertexData_[0].size(); col++) {
			vertexData_[row][col] =
				flattenedVertexData_[(vertexData_.size() * row) + col];
		}
	}

}




/// ------------------------------------------
/// ↓ 
/// ------------------------------------------
void Terrain::TransferFlattenedVertexData() {

	///- フラット化頂点データに二次元配列の頂点データを挿入
	flattenedVertexData_.clear();
	for(auto& vertexData : vertexData_) {
		flattenedVertexData_.insert(flattenedVertexData_.end(), vertexData.begin(), vertexData.end());
	}
}


void Terrain::SetVertexHeight(int row, int col, float height) {
	if(!CheckRange(row, col)) { return; }
	vertexData_[row][col].position.y = height;
}


void Terrain::AddVertexHeight(int row, int col, float height) {
	if(!CheckRange(row, col)) { return; }
	vertexData_[row][col].position.y += height;
}


void Terrain::SubVertexHeight(int row, int col, float height) {
	if(!CheckRange(row, col)) { return; }
	vertexData_[row][col].position.y -= height;
}


bool Terrain::CheckRange(int row, int col) {
	if(row < 0 || row > vertexData_.size() - 1) { return false; }
	if(col < 0 || col > vertexData_[0].size() - 1) { return false; }
	return true;
}
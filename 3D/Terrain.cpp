#include "Terrain.h"

#include <array>

#include <DxCommand.h>
#include <DxDescriptors.h>
#include <TextureManager.h>
#include <ImGuiManager.h>
#include <Engine.h>
#include <Input.h>
#include <PerlinNoise.h>
#include <DirectionalLight.h>
#include <InputImage.h>
#include <PipelineStateObjectManager.h>




Vec3f CalculateLuminance(const unsigned char* pixel) {
	float r = pixel[0];
	float g = pixel[1];
	float b = pixel[2];
	return Vec3f(r, g, b);
}





Terrain::Terrain() {}
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



	CreateReadBackResource();



	/// ------------------------------------------------- 
	/// ↓ その他メンバ変数の初期化
	/// ------------------------------------------------- 


	worldTransform_.Initialize();
	worldTransform_.scale = { 0.5f,12.0f,0.5f };
	color_ = { 1.0f,1.0f,1.0f,1.0f };


	///- ノイズから地形の高さを計算

	noise_ = std::make_unique<PerlinNoise>(uint32_t(123));
	noisePower_ = 1.0f;

	///- 法線ベクトルを計算
	//NormalVector();

	verticalIntensity_ = 1.0f;
	saveVerticalIntensity_ = verticalIntensity_;

}



void Terrain::Update() {

	///- 最初に行う処理
	TransferFlattenedVertexData();


	///- ImGuiでデバッグ
#ifdef _DEBUG
	ImGui::Begin("Terrain");


	/// ------------------------------------------------
	///- 地形のトランスフォーム
	/// ------------------------------------------------
	if(ImGui::TreeNodeEx("Transform", true)) {

		ImGui::DragFloat3("scale", &worldTransform_.scale.x, 0.25f);
		ImGui::DragFloat3("rotate", &worldTransform_.rotate.x, 1.0f / 64.0f);
		ImGui::DragFloat3("translate", &worldTransform_.translate.x, 0.25f);
		ImGui::ColorEdit4("color", &color_.x);

		ImGui::TreePop();
	}

	ImGui::Separator();

	/// ------------------------------------------------
	///- 頂点の情報
	/// ------------------------------------------------
	if(ImGui::TreeNodeEx("VertexData", true)) {

		ImGui::Text("vertexData dataSize: %d", sizeof(VertexData) * flattenedVertexData_.size());

		ImGui::Spacing();

		static int rowIndex = 0;
		static int colIndex = 0;
		ImGui::SliderInt("vertexRowIndex", &rowIndex, 0, static_cast<int>(vertexData_.size() - 1));
		ImGui::SliderInt("vertexColIndex", &colIndex, 0, static_cast<int>(vertexData_[0].size() - 1));
		ImGui::DragFloat4("vertex", &vertexData_[rowIndex][colIndex].position.x, 0.25f);
		ImGui::DragFloat3("normal", &vertexData_[rowIndex][colIndex].normal.x, 0.0f);

		ImGui::TreePop();
	}


	ImGui::Separator();
	ImGui::Spacing();


	ImGui::DragFloat3("normalVector", &normalVector_.x, 0.0f);

	if(ImGui::Button("normalVector Reset")) {
		NormalVector();
	}


	ImGui::Spacing();


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


	ImGui::End();
#endif // _DEBUG


	/// -----------------------------------
	/// ↓ 更新処理ここから
	/// -----------------------------------

	ReadBack();



}



/// ------------------------------------------
/// ↓ 
/// ------------------------------------------
void Terrain::Draw() {
	ID3D12GraphicsCommandList* commandList = DxCommand::GetInstance()->GetList();
	PipelineStateObjectManager::GetInstance()->SetCommandList(1, commandList);

	///- 頂点情報
	memcpy(pVertexMappedData_, pVertexData_, flattenedVertexData_.size() * sizeof(VertexData));
	memcpy(pIndexMappedData_, pIndexData_, sizeof(uint32_t) * indexData_.size());

	///- 色情報
	materialData_->color = color_;

	///- 行列情報
	worldTransform_.UpdateWorldMatrix();
	matrixData_->World = worldTransform_.worldMatrix;
	matrixData_->WVP = worldTransform_.worldMatrix * Engine::GetCamera()->GetVpMatrix();

	///- psoを設定
	commandList->IASetVertexBuffers(0, 1, &vertexBufferView_);
	commandList->IASetIndexBuffer(&indexBuffer_);

	///- 各種設定
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	commandList->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
	commandList->SetGraphicsRootConstantBufferView(1, matrixResource_->GetGPUVirtualAddress());
	TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(2, "dragon");	 ///- terrain
	TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(3, "dragon");	 ///- heightMap
	TextureManager::GetInstance()->SetGraphicsRootDescriptorTableUAV(4, "GrayTexture");  ///- operation
	Light::GetInstance()->SetConstantBuffer(5, commandList);


	///- 描画 (DrawCall)
	commandList->DrawIndexedInstanced(UINT(indexData_.size()), 1, 0, 0, 0);

}




/// ------------------------------------------
/// ↓ 法線ベクトルの計算
/// ------------------------------------------
void Terrain::NormalVector() {


	///- local頂点をworld座標に変換

	Vec3f worldVertex[3] = {
		Mat4::Transform(vertexData_[0][0].position, worldTransform_.worldMatrix),
		Mat4::Transform(vertexData_[1][0].position, worldTransform_.worldMatrix),
		Mat4::Transform(vertexData_[0][1].position, worldTransform_.worldMatrix)
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
			vertexData_[row][col].position -= Vec4f{
				static_cast<float>(maxCol + 1) / 2.0f,
				0.0f,
				static_cast<float>(maxRow + 1) / 2.0f,
				0.0f
			};

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
	vertexResource_->Map(0, nullptr, &pVertexMappedData_);
	memcpy(pVertexMappedData_, pVertexData_, sizeof(VertexData) * flattendVertexSize);
	vertexResource_->Unmap(0, nullptr);

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
	indexResource_->Map(0, nullptr, &pIndexMappedData_);
	memcpy(pIndexMappedData_, pIndexData_, sizeof(uint32_t) * indexDataSize);
	indexResource_->Unmap(0, nullptr);
}


void Terrain::CreateReadBackResource() {
	ID3D12Resource* pTexture = TextureManager::GetInstance()->GetUavTextureResource("GrayTexture");
	D3D12_RESOURCE_DESC readBackDesc = CD3DX12_RESOURCE_DESC::Buffer(GetRequiredIntermediateSize(pTexture, 0, 1));

	D3D12_HEAP_PROPERTIES heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_READBACK);
	ID3D12Device* device = DirectXCommon::GetInstance()->GetDevice();
	HRESULT hr = device->CreateCommittedResource(
		&heapProps,
		D3D12_HEAP_FLAG_NONE,
		&readBackDesc,
		D3D12_RESOURCE_STATE_COPY_DEST,
		nullptr,
		IID_PPV_ARGS(&readBackResource_)
	);
	assert(SUCCEEDED(hr));

	ID3D12GraphicsCommandList* commandList = DxCommand::GetInstance()->GetList();
	CD3DX12_RESOURCE_BARRIER transition = CD3DX12_RESOURCE_BARRIER::Transition(
		pTexture,
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
		D3D12_RESOURCE_STATE_COPY_SOURCE
	);
	commandList->ResourceBarrier(1, &transition);

	D3D12_PLACED_SUBRESOURCE_FOOTPRINT footprint{};
	UINT numRows;
	UINT64 rowSizeInBytes;
	UINT64 totalBytes;
	D3D12_RESOURCE_DESC textureDesc = pTexture->GetDesc();
	device->GetCopyableFootprints(&textureDesc, 0, 1, 0, &footprint, &numRows, &rowSizeInBytes, &totalBytes);

	D3D12_TEXTURE_COPY_LOCATION srcLocation = {};
	srcLocation.pResource = pTexture;
	srcLocation.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
	srcLocation.SubresourceIndex = 0;

	D3D12_TEXTURE_COPY_LOCATION dstLocation = {};
	dstLocation.pResource = readBackResource_.Get();
	dstLocation.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
	dstLocation.PlacedFootprint = footprint;

	commandList->CopyTextureRegion(&dstLocation, 0, 0, 0, &srcLocation, nullptr);

	///- copy後のbarrier設定
	CD3DX12_RESOURCE_BARRIER transitionAfterCopy = CD3DX12_RESOURCE_BARRIER::Transition(
		pTexture,
		D3D12_RESOURCE_STATE_COPY_SOURCE,
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS
	);
	commandList->ResourceBarrier(1, &transitionAfterCopy);

}


void Terrain::ReadBack() {
	D3D12_RANGE readRange = { 0, 0 };
	readBackResource_->Map(0, &readRange, &readBackData_);





	readBackResource_->Unmap(0, nullptr);

}

void Terrain::OutputImage() {
	//CreateReadBackResource();

	readBackResource_->Map(0, nullptr, &readBackData_);

	ID3D12Resource* pTexture = TextureManager::GetInstance()->GetUavTextureResource("GrayTexture");
	UINT height = pTexture->GetDesc().Height;
	UINT width = static_cast<UINT>(pTexture->GetDesc().Width);

	cv::Mat image(height, width, CV_8UC4);

	for(UINT y = 0; y < height; ++y) {
		for(UINT x = 0; x < width; ++x) {
			size_t pixelIndex = (y * width + x);
			Vec3f color = CalculateLuminance(static_cast<unsigned char*>(readBackData_) + pixelIndex);
			image.at<cv::Vec3b>(y, x) = cv::Vec3b(
				static_cast<uchar>(std::clamp(color.x, 0.0f, 255.0f)),
				static_cast<uchar>(std::clamp(color.y, 0.0f, 255.0f)),
				static_cast<uchar>(std::clamp(color.z, 0.0f, 255.0f))
			);
		}
	}

	cv::imwrite("output.png", image);
	cv::imshow("output.png", image);

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
#include "Terrain.h"

#include <array>

#include <DxCommand.h>
#include <DxDescriptors.h>
#include <TextureManager.h>
#include "ImGuiManager.h"
#include "Engine.h"
#include "Input.h"

#include "PerlinNoise.h"
#include "DirectionalLight.h"

#include "InputImage.h"


Terrain::Terrain() {}
Terrain::~Terrain() {
	///- 解放処理
	wvpResource_.Reset();
	materialResource_.Reset();
	vertexResource_.Reset();
}



void Terrain::Init() {
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	/// -------------------------------------------------
	/// ↓ 頂点計算
	/// ------------------------------------------------- 


	rowSubdivision_ = 1000;
	colSubdivision_ = 1000;
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

	materialResource_.Attach(dxCommon->CreateBufferResource(sizeof(Material)));
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
	materialData_->color = { 1.0f,1.0f,1.0f,1.0f };
	materialData_->enableLighting = true;


	/// -------------------------
	/// ↓ wvpResource
	/// -------------------------

	wvpResource_.Attach(dxCommon->CreateBufferResource(sizeof(TransformMatrix)));
	wvpResource_->Map(0, nullptr, reinterpret_cast<void**>(&matrixData_));
	matrixData_->World = Matrix4x4::MakeIdentity(); //- とりあえずの単位行列
	matrixData_->WVP = Matrix4x4::MakeIdentity(); //- とりあえずの単位行列







	/// ------------------------------------------------- 
	/// ↓ その他メンバ変数の初期化
	/// ------------------------------------------------- 


	worldTransform_.Init();
	worldTransform_.scale = { 0.1f,0.1f,0.1f };
	color_ = { 1.0f,1.0f,1.0f,1.0f };


	///- ノイズから地形の高さを計算

	noise_ = std::make_unique<PerlinNoise>(uint32_t(123));
	noisePower_ = 1.0f;

	///- 法線ベクトルを計算
	//NormalVector();

	verticalIntensity_ = 1.0f;
	saveVerticalIntensity_ = verticalIntensity_;

	image_ = cv::Mat();
	saveImage_ = cv::Mat();

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


	ImGui::Spacing();



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
						Vec2f{ vertexData_[row][col].position.x, vertexData_[row][col].position.z } / (float(rowSubdivision_) * 0.5f)
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



	/// ------------------------------------------------
	/// 入力した画像で地形を再生成
	/// ------------------------------------------------


	if(ImGui::TreeNodeEx("Inmort/Export")) {

		ImGui::SliderFloat("verticalIntensity", &verticalIntensity_, 0.0f, 1.0f);

		///- 読み込んだ画像から地形を再生成
		if(ImGui::Button("Regenarate Terrain")) {

			///- 画像情報をコピー
			image_ = InputImage::GetInstance()->GetInputImage();

			if(!image_.empty()) {

				rowSubdivision_ = image_.rows;
				colSubdivision_ = image_.cols;

				///- 頂点インデックスデータの再計算
				IndexDataCulc(rowSubdivision_, colSubdivision_);
				VertexDataCulc(rowSubdivision_, colSubdivision_);

				///- 計算したデータの大きさにresourceを作り直す
				CreateIndexResource(indexData_.size());
				CreateVertexResource(flattenedVertexData_.size());

				///- 高さの調整

				cv::Mat grayImage;
				cv::cvtColor(image_, grayImage, cv::COLOR_BGR2GRAY);

				cv::imshow("grayImage", grayImage);


				cv::normalize(grayImage, grayImage, 0, 255, cv::NORM_MINMAX, CV_8U);


				cv::flip(grayImage, grayImage, 0);
				for(uint32_t row = 0; row < static_cast<uint32_t>(rowSubdivision_); row++) {
					for(uint32_t col = 0; col < static_cast<uint32_t>(colSubdivision_); col++) {

						vertexData_[row][col].position.y =
							(grayImage.at<uint8_t>(row, col) - (255.0f / 2.0f)) * verticalIntensity_;

					}
				}

				TransferFlattenedVertexData();
				///- ここで使用した高低差の力を保存しておく; 出力の時に使う
				saveVerticalIntensity_ = verticalIntensity_;
			}
		}

		ImGui::Spacing();
		ImGui::Spacing();

		///- 出力用画像にセーブする
		if(ImGui::Button("SaveImage")) {

			TransferVertexData();

			saveImage_ = image_;


			for(uint32_t row = 0; row < static_cast<uint32_t>(vertexData_.size() - 1); row++) {
				for(uint32_t col = 0; col < static_cast<uint32_t>(vertexData_[0].size() - 1); col++) {

					saveImage_.at<uint8_t>(row, col) =
						static_cast<uint8_t>((vertexData_[row][col].position.y / saveVerticalIntensity_) + (255.0f / 2.0f));

				}
			}

			///- 上下反転しているので元に戻す
			//cv::flip(saveImage_, saveImage_, 0);

			cv::imshow("saveImage", saveImage_);
			InputImage::GetInstance()->SetOutputImage(saveImage_);

		}

		ImGui::TreePop();
	}


	ImGui::End();
#endif // _DEBUG


	/// -----------------------------------
	/// ↓ 更新処理ここから
	/// -----------------------------------

	//worldTransform_.rotate.y += 1.0f / 128.0f;




}



/// ------------------------------------------
/// ↓ 
/// ------------------------------------------
void Terrain::Draw() {
	ID3D12GraphicsCommandList* commandList = DxCommand::GetInstance()->GetList();

	commandList->IASetVertexBuffers(0, 1, &vertexBufferView_);
	commandList->IASetIndexBuffer(&indexBuffer_);

	///- データの書き込み

	///- 頂点情報
	memcpy(pVertexMappedData_, pVertexData_, flattenedVertexData_.size() * sizeof(VertexData));
	memcpy(pIndexMappedData_, pIndexData_, sizeof(uint32_t) * indexData_.size());

	///- 色情報
	materialData_->color = color_;


	///- 行列情報
	worldTransform_.MakeWorldMatrix();
	matrixData_->World = worldTransform_.worldMatrix;
	matrixData_->WVP = worldTransform_.worldMatrix * Engine::GetCamera()->GetVpMatrix();

	///- 各種設定
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	commandList->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
	commandList->SetGraphicsRootConstantBufferView(1, wvpResource_->GetGPUVirtualAddress());
	TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(2, "tileMap");
	TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(3, "dragon");
	Light::GetInstance()->SetConstantBuffer(commandList);


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
	vertexResource_.Attach(DirectXCommon::GetInstance()->CreateBufferResource(sizeof(VertexData) * flattendVertexSize));

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
	indexResource_.Attach(DirectXCommon::GetInstance()->CreateBufferResource(sizeof(uint32_t) * indexDataSize));
	///- Bufferの設定
	indexBuffer_.BufferLocation = indexResource_->GetGPUVirtualAddress();
	indexBuffer_.SizeInBytes = UINT(sizeof(uint32_t) * indexDataSize);
	indexBuffer_.Format = DXGI_FORMAT_R32_UINT;

	///- Resourceに対し情報を書き込む
	indexResource_->Map(0, nullptr, &pIndexMappedData_);
	memcpy(pIndexMappedData_, pIndexData_, sizeof(uint32_t) * indexDataSize);
	indexResource_->Unmap(0, nullptr);
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
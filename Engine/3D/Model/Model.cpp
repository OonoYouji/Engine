#include <Model.h>

#pragma region Include
#include <fstream>
#include <sstream>
#include <cassert>
#include <iostream>
#include <filesystem>

#include <Vector4.h>
#include <Vector3.h>
#include <Vector2.h>
#include <Matrix4x4.h>

#include <Engine.h>
#include <DxCommand.h>
#include <DxDescriptors.h>
#include <TextureManager.h>
#include <ImGuiManager.h>
#include <DirectionalLight.h>
#include <PipelineStateObjectManager.h>
#pragma endregion


Model::Model() {}
Model::~Model() {

	vertexBuffer_.Reset();
	materialBuffer_.Reset();
	transformBuffer_.Reset();

}



/// ===================================================
/// 初期化
/// ===================================================
void Model::Initialize(const std::string& directoryPath, const std::string& fileName) {

	*this = LoadObjFile(directoryPath, fileName);
	size_t pos = fileName.find(".obj");
	name_ = fileName.substr(0, pos);

	dxCommon_ = DirectXCommon::GetInstance();
	dxDescriptors_ = DxDescriptors::GetInstance();

	CreateVertexBuffer();
	CreateIndexBuffer();
	CreateMaterialBuffer();
	CreateTransformBuffer();

}



/// ===================================================
/// 描画処理
/// ===================================================
void Model::Draw(const WorldTransform& worldTransform, const Mat4& uvTransform) {

	///- 描画数が上限を超えた
	if(instanceCount_ >= kMaxInstanceCount_) {
		assert(false);
	}

	///- 構造体のデータを設定
	transformMatrixDatas_[instanceCount_].World = worldTransform.matTransform;
	transformMatrixDatas_[instanceCount_].WVP = worldTransform.matTransform * Engine::GetCamera()->GetVpMatrix();
	materialData_[instanceCount_].uvTransform = uvTransform;

	instanceCount_++;
}


/// ===================================================
/// 描画前処理
/// ===================================================
void Model::PreDraw() {
	Reset();
}


/// ===================================================
/// 描画語処理
/// ===================================================
void Model::PostDraw() {
	ID3D12GraphicsCommandList* commandList = DxCommand::GetInstance()->GetList();
	PipelineStateObjectManager::GetInstance()->SetCommandList("Model", commandList);

	///- 頂点情報のコピー
	std::memcpy(vertexData_, vertices_.data(), sizeof(VertexData) * vertices_.size());

	///- IASet
	commandList->IASetVertexBuffers(0, 1, &vbv_);
	commandList->IASetIndexBuffer(&ibv_);
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	DxDescriptors::GetInstance()->SetCommandListSrvHeap(commandList);
	commandList->SetGraphicsRootDescriptorTable(2, materialGpuHandle_);
	commandList->SetGraphicsRootDescriptorTable(0, transformGpuHandle_);

	TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(1, material_.textureName);
	Light::GetInstance()->SetConstantBuffer(3, commandList);

	commandList->DrawIndexedInstanced(UINT(indices_.size()), instanceCount_, 0, 0, 0);
}


/// ===================================================
/// インスタンス数のリセット
/// ===================================================
void Model::Reset() {
	instanceCount_ = 0;
}



/// ===================================================
/// .objファイルの読み込み
/// ===================================================
Model Model::LoadObjFile(const std::string& directoryPath, const std::string& fileName) {
	Model model = Model();
	std::vector<Vector4> positions;	//- 位置
	std::vector<Vector3> normals;	//- 法線
	std::vector<Vector2> texcoords;	//- テクスチャ座標
	std::vector<uint32_t> indices;	//- インデックス
	std::string line;


	/// ---------------------------------
	/// ↓ ファイルを開く
	/// ---------------------------------

	std::ifstream file(directoryPath + "/" + fileName);
	assert(file.is_open() && "ファイルを開けませんでした ファイルパスを確認してください");



	/// ---------------------------------
	/// ↓ ファイルの中身を読み解く
	/// ---------------------------------

	while(std::getline(file, line)) {
		std::string identifier;
		std::istringstream s(line);

		s >> identifier;	//- 先頭の識別子を読む


		/// ---------------------------------
		/// ↓ 識別子しだいの処理を行う
		/// ---------------------------------

		if(identifier == "v") {				///- position

			Vec4f position;
			s >> position.x >> position.y >> position.z;
			position.x *= -1.0f;
			position.w = 1.0f;
			positions.push_back(position);

		} else if(identifier == "vt") {		///- texcood

			Vec2f texcoord;
			s >> texcoord.x >> texcoord.y;
			texcoord.y = 1.0f - texcoord.y;
			texcoords.push_back(texcoord);

		} else if(identifier == "vn") {		///- normal

			Vec3f normal;
			s >> normal.x >> normal.y >> normal.z;
			normal.x *= -1.0f;
			normals.push_back(normal);

		} else if(identifier == "f") {		///- triangle

			///- 面は三角形限定
			VertexData triangle[3]{};
			for(int32_t faceVertex = 0; faceVertex < 3; ++faceVertex) {
				std::string vertexDefinition;
				s >> vertexDefinition;

				///- 頂点の要素へのindexは [位置/UV/法線] で格納されているので、分解してindexを取得する
				std::istringstream v(vertexDefinition);
				uint32_t elementIndices[3];

				for(int32_t element = 0; element < 3; ++element) {
					std::string index;
					std::getline(v, index, '/'); //- 区切りでindexを読んでいく
					elementIndices[element] = std::stoi(index);
				}

				///- 要素へのindexから、実際の要素の値を取得して、頂点を構築する
				Vec4f position = positions[elementIndices[0] - 1];
				Vec2f texcoord = texcoords[elementIndices[1] - 1];
				Vec3f normal = normals[elementIndices[2] - 1];
				triangle[faceVertex] = { position, texcoord, normal };
				//model.vertices_.push_back({ position, texcoord, normal });

				indices.push_back(uint32_t(indices.size()));

			}

			///- 頂点を逆順で保存する (モデルが右手座標系のため)
			model.vertices_.push_back(triangle[2]);
			model.vertices_.push_back(triangle[1]);
			model.vertices_.push_back(triangle[0]);

		} else if(identifier == "mtllib") {

			std::string materialFilename;
			s >> materialFilename;
			model.material_ = LoadMaterialTemplateFile(directoryPath, materialFilename);

			///- .mtlにテクスチャがなければwhite1x1を読み込む
			if(model.material_.textureName.empty()) {
				model.material_.textureName = "white1x1";
				model.material_.textureFilePath = "./Engine/Resources/Images/white1x1";
			}
			TextureManager::GetInstance()->Load(model.material_.textureName, model.material_.textureFilePath);

		}

	}

	model.indices_ = indices;

	return model;
}



/// ===================================================
/// .mtlファイルの読み込み
/// ===================================================
Model::MaterialData Model::LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& fileName) {
	MaterialData materialData;
	std::string line;
	std::ifstream file(directoryPath + "/" + fileName);
	assert(file.is_open() && "マテリアルの読み込みに失敗しましたファイルパスを確認してください");

	while(std::getline(file, line)) {
		std::string identifier;
		std::istringstream s(line);
		s >> identifier;

		///- identifierに応じた処理
		if(identifier == "map_Kd") {
			std::string textureFileName;
			s >> textureFileName;

			///- 連結してファイルパスにする
			materialData.textureFilePath = directoryPath + "/" + textureFileName;

			///- .objを消してテクスチャの名前とする
			for(uint32_t i = 0; i < 4; i++) {
				textureFileName.pop_back();
			}

			std::string name;

			if(!textureFileName.empty()) {
				for(int i = static_cast<int>(textureFileName.size() - 1); i >= 0; --i) {
					if(textureFileName[i] != '/') {
						name.push_back(textureFileName[i]);
					} else {
						break;
					}
				}
			}

			std::reverse(name.begin(), name.end());

			materialData.textureName = name;
		}

	}

	return materialData;
}



/// ===================================================
/// 頂点バッファの生成
/// ===================================================
void Model::CreateVertexBuffer() {

	///- バッファの生成
	vertexBuffer_ = dxCommon_->CreateBufferResource(sizeof(VertexData) * vertices_.size());

	vbv_.BufferLocation = vertexBuffer_->GetGPUVirtualAddress();
	vbv_.SizeInBytes = UINT(sizeof(VertexData) * vertices_.size());
	vbv_.StrideInBytes = sizeof(VertexData);

	///- 頂点リソースにデータを書き込む
	vertexBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));
	std::memcpy(vertexData_, vertices_.data(), sizeof(VertexData) * vertices_.size());

}


/// ===================================================
/// インデックスバッファの生成
/// ===================================================
void Model::CreateIndexBuffer() {

	///- インデックスバッファの生成
	indexBuffer_ = dxCommon_->CreateBufferResource(sizeof(uint32_t) * indices_.size());

	///- ビューの初期化
	ibv_.BufferLocation = indexBuffer_->GetGPUVirtualAddress();
	ibv_.SizeInBytes = static_cast<UINT>(sizeof(uint32_t) * indices_.size());
	ibv_.Format = DXGI_FORMAT_R32_UINT;

	///- データのマップ
	indexBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&indexData_));
	std::memcpy(indexData_, indices_.data(), sizeof(uint32_t) * indices_.size());

}



/// ===================================================
/// マテリアルバッファの生成
/// ===================================================
void Model::CreateMaterialBuffer() {

	///- バッファの生成
	materialBuffer_ = dxCommon_->CreateBufferResource(sizeof(Material) * kMaxInstanceCount_);

	///- マッピング
	materialBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
	for(uint32_t index = 0; index < kMaxInstanceCount_; ++index) {
		materialData_[index].color = Vec4f(1.0f, 1.0f, 1.0f, 1.0f);
		materialData_[index].enableLighting = true;
		materialData_[index].uvTransform = Mat4::MakeIdentity();
	}

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
	dxCommon_->GetDevice()->CreateShaderResourceView(materialBuffer_.Get(), &materialDesc, materialCpuHandle);

}



/// ===================================================
/// 行列バッファの生成
/// ===================================================
void Model::CreateTransformBuffer() {

	///- 行列バッファの生成
	transformBuffer_ = dxCommon_->CreateBufferResource(sizeof(TransformMatrix) * kMaxInstanceCount_);

	///- マッピング
	transformBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&transformMatrixDatas_));
	for(uint32_t index = 0; index < kMaxInstanceCount_; ++index) {
		transformMatrixDatas_[index].WVP = Mat4::MakeIdentity();
		transformMatrixDatas_[index].World = Mat4::MakeIdentity();
	}

	D3D12_SHADER_RESOURCE_VIEW_DESC transformMatrixDesc{};
	transformMatrixDesc.Format = DXGI_FORMAT_UNKNOWN;
	transformMatrixDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	transformMatrixDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	transformMatrixDesc.Buffer.FirstElement = 0;
	transformMatrixDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	transformMatrixDesc.Buffer.NumElements = kMaxInstanceCount_;
	transformMatrixDesc.Buffer.StructureByteStride = sizeof(TransformMatrix);

	D3D12_CPU_DESCRIPTOR_HANDLE transformCpuHandle = dxDescriptors_->GetCPUDescriptorHandle();
	transformGpuHandle_ = dxDescriptors_->GetGPUDescriptorHandle();
	dxDescriptors_->AddSrvUsedCount();
	dxCommon_->GetDevice()->CreateShaderResourceView(transformBuffer_.Get(), &transformMatrixDesc, transformCpuHandle);

}





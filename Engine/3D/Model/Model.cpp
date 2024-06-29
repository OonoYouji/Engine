#include <Model.h>

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


Model::Model() {}
Model::~Model() {

	vertexResource_.Reset();
	materialResource_.Reset();
	transformMatrixResource_.Reset();

}



void Model::Initialize(const std::string& directoryPath, const std::string& fileName) {

	*this = LoadObjFile(directoryPath, fileName);
	size_t pos = fileName.find(".obj");
	name_ = fileName.substr(0, pos);

	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	/// ------------------------------------------
	/// ↓ Vertex Resourceの生成
	/// ------------------------------------------

	///- 頂点リソース
	vertexResource_ = dxCommon->CreateBufferResource(sizeof(VertexData) * vertexDatas_.size());

	///- 頂点バッファビュー
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	vertexBufferView_.SizeInBytes = UINT(sizeof(VertexData) * vertexDatas_.size());
	vertexBufferView_.StrideInBytes = sizeof(VertexData);

	///- 頂点リソースにデータを書き込む
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));
	std::memcpy(vertexData_, vertexDatas_.data(), sizeof(VertexData) * vertexDatas_.size());



	/// ------------------------------------------
	/// ↓ Material Resourceの生成
	/// ------------------------------------------

	///- マテリアルリソース
	materialResource_ = dxCommon->CreateBufferResource(sizeof(Material));

	///- マッピング
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
	materialData_->color = Vec4f(1.0f, 1.0f, 1.0f, 1.0f);
	materialData_->enableLighting = true;
	materialData_->uvTransform = Mat4::MakeIdentity();


	/// ------------------------------------------
	/// ↓ TransformMatrix Resourceの生成
	/// ------------------------------------------

	///- トランスフォームマトリクス
	transformMatrixResource_ = dxCommon->CreateBufferResource(sizeof(TransformMatrix) * kMaxInstanceCount_);

	///- マッピング
	transformMatrixResource_->Map(0, nullptr, reinterpret_cast<void**>(&transformMatrixDatas_));
	for(uint32_t index = 0; index < kMaxInstanceCount_; ++index) {
		transformMatrixDatas_[index].WVP = Mat4::MakeIdentity();
		transformMatrixDatas_[index].World = Mat4::MakeIdentity();
	}

	///- srvの作成

	D3D12_SHADER_RESOURCE_VIEW_DESC transformMatrixDesc{};
	transformMatrixDesc.Format = DXGI_FORMAT_UNKNOWN;
	transformMatrixDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	transformMatrixDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	transformMatrixDesc.Buffer.FirstElement = 0;
	transformMatrixDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	transformMatrixDesc.Buffer.NumElements = kMaxInstanceCount_;
	transformMatrixDesc.Buffer.StructureByteStride = sizeof(TransformMatrix);

	DxDescriptors* descriptiors = DxDescriptors::GetInstance();
	D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = descriptiors->GetCPUDescriptorHandle();
	gpuHandle_ = descriptiors->GetGPUDescriptorHandle();
	descriptiors->AddSrvUsedCount();

	dxCommon->GetDevice()->CreateShaderResourceView(transformMatrixResource_.Get(), &transformMatrixDesc, cpuHandle);

}



void Model::Draw(const WorldTransform& worldTransform) {
	ID3D12GraphicsCommandList* commandList = DxCommand::GetInstance()->GetList();
	PipelineStateObjectManager::GetInstance()->SetCommandList("Model", commandList);

	///- 頂点情報のコピー
	std::memcpy(vertexData_, vertexDatas_.data(), sizeof(VertexData) * vertexDatas_.size());

	if(instanceCount_ >= kMaxInstanceCount_) {
		///- 描画数が上限を超えた
		assert(false);
	}

	///- Data
	transformMatrixDatas_[instanceCount_].World = worldTransform.matTransform;
	transformMatrixDatas_[instanceCount_].WVP = worldTransform.matTransform * Engine::GetCamera()->GetVpMatrix();
	instanceCount_++;

	///- IASet
	commandList->IASetVertexBuffers(0, 1, &vertexBufferView_);

	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	commandList->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());

	DxDescriptors::GetInstance()->SetCommandListSrvHeap(commandList);
	commandList->SetGraphicsRootDescriptorTable(1, gpuHandle_);
	TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(2, material_.textureName);
	Light::GetInstance()->SetConstantBuffer(3, commandList);

	commandList->DrawInstanced(UINT(vertexDatas_.size()), instanceCount_, 0, 0);
}


void Model::DebugDraw(const std::string& windowName) {
#ifdef _DEBUG
	ImGui::Begin(windowName.c_str());

	//worldTransform_->ImGuiTreeNodeDebug();

	ImGui::Separator();

	if(ImGui::TreeNodeEx("Material", true)) {

		ImGui::ColorEdit4("Color", &materialData_->color.x);
		static bool enableLighting = materialData_->enableLighting;
		ImGui::Checkbox("EnableLighting", &enableLighting);
		if(ImGui::IsItemEdited()) {
			materialData_->enableLighting = enableLighting;
		}

		ImGui::TreePop();
	}

	ImGui::End();
#endif // _DEBUG
}

void Model::Reset() {
	instanceCount_ = 0;
}



Model Model::LoadObjFile(const std::string& directoryPath, const std::string& fileName) {
	Model model = Model();
	std::vector<Vector4> positions;	//- 位置
	std::vector<Vector3> normals;	//- 法線
	std::vector<Vector2> texcoords;	//- テクスチャ座標
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
			position.z *= -1.0f;
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
			normal.z *= -1.0f;
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


			}

			///- 頂点を逆順で保存する (モデルが右手座標系のため)
			model.vertexDatas_.push_back(triangle[0]);
			model.vertexDatas_.push_back(triangle[1]);
			model.vertexDatas_.push_back(triangle[2]);

		} else if(identifier == "mtllib") {

			std::string materialFilename;
			s >> materialFilename;
			model.material_ = LoadMaterialTemplateFile(directoryPath, materialFilename);
			TextureManager::GetInstance()->Load(model.material_.textureName, model.material_.textureFilePath);

		}

	}

	return model;
}



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
			for(size_t i = textureFileName.size() - 1; i >= 0; --i) {
				if(textureFileName[i] != '/') {
					name.push_back(textureFileName[i]);
				} else {
					break;
				}
			}

			std::reverse(name.begin(), name.end());

			materialData.textureName = name;
		}

	}

	return materialData;
}


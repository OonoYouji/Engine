#include <Model.h>

#include <fstream>
#include <sstream>
#include <cassert>
#include <string>

#include <Vector4.h>
#include <Vector3.h>
#include <Vector2.h>
#include <Matrix4x4.h>

#include <Engine.h>
#include <DxCommand.h>
#include <TextureManager.h>
#include <ImGuiManager.h>
#include <DirectionalLight.h>


Model::Model() {}
Model::~Model() {

	vertexResource_.Reset();
	materialResource_.Reset();
	transformMatrixResource_.Reset();

}



void Model::Initialize(const std::string& directoryPath, const std::string& fileName) {

	*this = LoadObjFile(directoryPath, fileName);

	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	/// ------------------------------------------
	/// ↓ Vertex Resourceの生成
	/// ------------------------------------------

	///- 頂点リソース
	vertexResource_.Attach(dxCommon->CreateBufferResource(sizeof(VertexData) * vertices_.size()));

	///- 頂点バッファビュー
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	vertexBufferView_.SizeInBytes = UINT(sizeof(VertexData) * vertices_.size());
	vertexBufferView_.StrideInBytes = sizeof(VertexData);

	///- 頂点リソースにデータを書き込む
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));
	std::memcpy(vertexData_, vertices_.data(), sizeof(VertexData) * vertices_.size());



	/// ------------------------------------------
	/// ↓ Material Resourceの生成
	/// ------------------------------------------

	///- マテリアルリソース
	materialResource_.Attach(dxCommon->CreateBufferResource(sizeof(Material)));

	///- マッピング
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
	materialData_->color = Vec4f(1.0f, 1.0f, 1.0f, 1.0f);
	materialData_->enableLighting = true;



	/// ------------------------------------------
	/// ↓ TransformMatrix Resourceの生成
	/// ------------------------------------------

	///- トランスフォームマトリクス
	transformMatrixResource_.Attach(dxCommon->CreateBufferResource(sizeof(TransformMatrix)));

	///- マッピング
	transformMatrixResource_->Map(0, nullptr, reinterpret_cast<void**>(&transformMatrixData_));
	transformMatrixData_->WVP = Mat4::MakeIdentity();
	transformMatrixData_->World = Mat4::MakeIdentity();




	worldTransform_.Initialize();

}



void Model::Draw() {
	ID3D12GraphicsCommandList* commandList = DxCommand::GetInstance()->GetList();

	///- 頂点情報のコピー
	std::memcpy(vertexData_, vertices_.data(), sizeof(VertexData) * vertices_.size());

	///- IASet
	commandList->IASetVertexBuffers(0, 1, &vertexBufferView_);

	///- Data
	worldTransform_.UpdateWorldMatrix();
	transformMatrixData_->World = worldTransform_.worldMatrix;
	transformMatrixData_->WVP = worldTransform_.worldMatrix * Engine::GetCamera()->GetVpMatrix();


	///- Resource Setting

	///- 形状を設定
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	///- マテリアルのCBufferの場所を設定
	commandList->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
	///- wvp用のCBufferの場所を設定
	commandList->SetGraphicsRootConstantBufferView(1, transformMatrixResource_->GetGPUVirtualAddress());
	///- DescriptorTableを設定する
	TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(2, material_.textureName);
	///- Light
	Light::GetInstance()->SetConstantBuffer(commandList);

	commandList->DrawInstanced(UINT(vertices_.size()), 1, 0, 0);

}


void Model::DebugDraw(const std::string& windowName) {
#ifdef _DEBUG
	ImGui::Begin(windowName.c_str());

	worldTransform_.ImGuiTreeNodeDebug();

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
			model.vertices_.push_back(triangle[0]);
			model.vertices_.push_back(triangle[1]);
			model.vertices_.push_back(triangle[2]);

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
			materialData.textureName = textureFileName;
		}

	}

	return materialData;
}

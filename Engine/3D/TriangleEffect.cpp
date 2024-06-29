#include <TriangleEffect.h>

#include <Engine.h>
#include <Camera.h>
#include <DxCommand.h>
#include <PipelineStateObjectManager.h>
#include <TextureManager.h>
#include <DirectionalLight.h>
#include <ImGuiManager.h>

TriangleEffect::TriangleEffect() {}
TriangleEffect::~TriangleEffect() {
	elements_.clear();
}

void TriangleEffect::Initialize() {

	/*for(uint32_t i = 0; i < 10; ++i) {
		elements_.push_back(Element());
	}

	for(auto& element : elements_) {
		element.Initialize();
		element.worldTransform_.translate = Vec3f(0.0f, 0.0f, 10.0f);
		element.velocity_ = Vec3f::Normalize(Vec3f(rand() % 5 - 2.0f, rand() % 5 - 2.0f, -10.0f)) / 10.0f;
	}*/

	perlinNoise_ = std::make_unique<PerlinNoise>(0);

	color_ = Vec4f(0.75f, 0.75f, 0.75f, 1.0);

	respawnTime_ = 5;
	frameCount_ = 0;

	speed_ = 0.025f;

}


void TriangleEffect::Update() {


	if(++frameCount_ % respawnTime_ == 0) {
		elements_.push_back(Element());

		Element& element = elements_.back();
		element.Initialize();
		element.worldTransform_.translate = Vec3f(
			(rand() % 30 - 15.0f) / 10.0f,
			-1.5f,
			float(rand() % 5)
		);

		element.velocity_ = Vec3f::Normalize(Vec3f(0.0f, 1.0f, 0.0f)) / 100.0f;
		perlinNoise_->ResetSheed(rand() % 100);
		float noise = perlinNoise_->GetNoise(element.worldTransform_.translate) * 2.0f;
		element.worldTransform_.rotate = { noise,noise,noise };


	}


#ifdef _DEBUG
	ImGui::Begin("Triangle Effect");

	static float scale = 1.0f;
	ImGui::DragFloat("Scale", &scale, 0.025f);
	for(auto& element : elements_) {
		element.worldTransform_.scale = Vec3f(scale, scale, scale);
	}

	///- 回転
	if(ImGui::TreeNodeEx("Rotation", ImGuiTreeNodeFlags_DefaultOpen)) {

		ImGui::DragFloat("Speed", &rotateSpeed_, 0.01f, 0.0f, 1.0f);

		static bool isRotationX = false;
		ImGui::Checkbox("IsRotationX", &isRotationX);
		if(isRotationX) {
			for(auto& element : elements_) {
				element.worldTransform_.rotate.x += rotateSpeed_;
			}
		}

		static bool isRotationY = false;
		ImGui::Checkbox("IsRotationY", &isRotationY);
		if(isRotationY) {
			for(auto& element : elements_) {
				element.worldTransform_.rotate.y += rotateSpeed_;
			}
		}

		static bool isRotationZ = false;
		ImGui::Checkbox("IsRotationZ", &isRotationZ);
		if(isRotationZ) {
			for(auto& element : elements_) {
				element.worldTransform_.rotate.z += rotateSpeed_;
			}
		}

		ImGui::TreePop();
	}

	ImGui::ColorEdit3("Color", &color_.x);

	ImGui::DragInt("RespawnTime", &respawnTime_, 1, 1, 100);
	ImGui::DragFloat("Speed", &speed_, 0.025f);

	ImGui::End();
#endif // _DEBUG


	for(auto& element : elements_) {
		element.velocity_ = Vec3f(0.0f, 1.0f, 0.0f) * speed_;
		element.Update();
	}

	elements_.remove_if([](auto& element) {
		if(element.isEnd_) {
			return true;
		} else {
			return false;
		}
	});

}


void TriangleEffect::Draw() {

	for(auto& element : elements_) {
		element.Draw(color_);
	}
}




void TriangleEffect::Element::Initialize() {
	worldTransform_.Initialize();

	vertexData_.push_back(VertexData({ 0.0f,  1.2f,  0.0f, 1.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }));
	vertexData_.push_back(VertexData({-1.0f, -0.8f, -1.0f, 1.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }));
	vertexData_.push_back(VertexData({ 1.0f, -0.8f, -1.0f, 1.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }));
	vertexData_.push_back(VertexData({ 0.0f, -0.8f,  1.0f, 1.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }));

	///- 法線ベクトルの計算
	for(auto& vertex : vertexData_) {
		vertex.normal = Vec3f::Convert4To3(vertex.position) - worldTransform_.translate;
	}

	indexData_.push_back(1);
	indexData_.push_back(2);
	indexData_.push_back(3);

	indexData_.push_back(0);
	indexData_.push_back(2);
	indexData_.push_back(1);

	indexData_.push_back(0);
	indexData_.push_back(1);
	indexData_.push_back(3);

	indexData_.push_back(0);
	indexData_.push_back(3);
	indexData_.push_back(2);

	///- リソース群の初期化
	resource_ = std::make_unique<Object3dResources>();
	resource_->CreateVertexResource(vertexData_.size());
	resource_->CreateIndexResouces(indexData_.size());
	resource_->CreateMaterialResource();
	resource_->CreateTransformationMatrix();

	resource_->materialData_->color = { 1.0f, 1.0f, 1.0f, 1.0f };
	resource_->materialData_->enableLighting = true;
	resource_->materialData_->uvTransform = Mat4::MakeIdentity();

	resource_->transformationMatrixData_->World = Mat4::MakeIdentity();
	resource_->transformationMatrixData_->WVP = Mat4::MakeIdentity();

	memcpy(resource_->indexData_, indexData_.data(), indexData_.size() * sizeof(uint32_t));

}



void TriangleEffect::Element::Update() {
	worldTransform_.translate += velocity_;
	worldTransform_.UpdateMatTransform();

	if(--deathTimer_ <= 0) {
		isEnd_ = true;
	}

}



void TriangleEffect::Element::Draw(const Vec4f& color) {
	ID3D12GraphicsCommandList* commandList = DxCommand::GetInstance()->GetList();
	PipelineStateObjectManager::GetInstance()->SetCommandList(0, commandList);

	commandList->IASetVertexBuffers(0, 1, &resource_->vertexBufferView_);
	commandList->IASetIndexBuffer(&resource_->indexBufferView_);

	memcpy(resource_->vertexData_, vertexData_.data(), vertexData_.size() * sizeof(VertexData));

	resource_->materialData_->color = color;

	resource_->transformationMatrixData_->World = worldTransform_.matTransform;
	resource_->transformationMatrixData_->WVP = worldTransform_.matTransform * Engine::GetCamera()->GetVpMatrix();

	resource_->SetConstantBufferView(commandList);
	TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(2, "white1x1");
	Light::GetInstance()->SetConstantBuffer(3, commandList);


	commandList->DrawIndexedInstanced(static_cast<UINT>(indexData_.size()), 1, 0, 0, 0);

}

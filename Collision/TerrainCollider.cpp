#include <TerrainCollider.h>

#include <DxCommand.h>
#include <DxDescriptors.h>
#include <PipelineStateObjectManager.h>
#include <TextureManager.h>
#include <ImGuiManager.h>

#include <Terrain.h>
#include <Player.h>


namespace {

	Vec3f Support(const std::vector<Vec3f>& vertices, const Vec3f& direction) {
		float maxDot = 0.0f;
		Vec3f result = vertices[0];
		for(const auto& vertex : vertices) {
			float d = Vec3f::Dot(vertex, direction);
			if(d > maxDot) {
				maxDot = d;
				result = vertex;
			}
		}
		return result;
	}

} ///- namespace


TerrainCollider::TerrainCollider() {}
TerrainCollider::~TerrainCollider() {}



void TerrainCollider::Initialize() {
	assert(pPlayer_);
	assert(pTerrain_);

	///- 地形の頂点データから座標のみを取得する
	terrainVertices_.clear();
	for(const auto& vertexData : pTerrain_->GetVertexDatas()) {
		terrainVertices_.push_back(Vec3f::Convert4To3(vertexData.position));
	}

	///- サポート写像から地形の最大値と最小値を取る
	terrainMinPosition_ = Support(terrainVertices_, Vec3f(-1.0f, 0.0f, -1.0f));
	terrainMaxPosition_ = Support(terrainVertices_, Vec3f(1.0f, 0.0f, 1.0f));

	///- Terrainの大きさを計算
	terrainSize_ = terrainMaxPosition_ - terrainMinPosition_;

	///- 地形のワールド行列の逆行列を計算
	terrainWorldMatrix_ = pTerrain_->GetWorldTransform().worldMatrix;
	inverseTerrainWorldMatrix_ = Mat4::MakeInverse(terrainWorldMatrix_);

	CreateOutputBuffer();

	height_ = 0.0f;

}



void TerrainCollider::Update() {

	///- debug表示
	ImGuiDebug();

	terrainWorldMatrix_ = pTerrain_->GetWorldTransform().worldMatrix;
	inverseTerrainWorldMatrix_ = Mat4::MakeInverse(terrainWorldMatrix_);

	texcoord_ = ConvertTexcoord(pPlayer_->GetWorldTransform().translate);
	if(IsWithinRange()) {
		height_ = GetHeight(texcoord_);
	}

}



void TerrainCollider::SetPlayer(const Player* player) {
	pPlayer_ = player;
}

void TerrainCollider::SetTerrain(const Terrain* terrain) {
	pTerrain_ = terrain;
}


bool TerrainCollider::IsCollision() {
	return false;
}


Vec2f TerrainCollider::ConvertTexcoord(const Vec3f& position) {

	///- playerの座標をTerrainのLocal空間に変換
	playerTerrainLocalPosition_ = Mat4::Transform(position, inverseTerrainWorldMatrix_);

	///- PlayerのLocal座標をTerrianの大きさで割ってtexcoordを計算
	Vec3f texcoord = playerTerrainLocalPosition_ / terrainSize_;

	///- Terrainは横、奥に伸びているのでy座標は使わない
	return Vec2f(texcoord.x, texcoord.z);
}



bool TerrainCollider::IsWithinRange() {

	///- TerrainのLocal空間に変換
	playerTerrainLocalPosition_ = Mat4::Transform(pPlayer_->GetWorldTransform().translate, inverseTerrainWorldMatrix_);

	///- サポート写像から地形の最大値と最小値を取る
	terrainMinPosition_ = Support(terrainVertices_, Vec3f(-1.0f, 0.0f, -1.0f));
	terrainMaxPosition_ = Support(terrainVertices_, Vec3f(1.0f, 0.0f, 1.0f));

	terrainMinPosition_ = Mat4::Transform(terrainMinPosition_, terrainWorldMatrix_);
	terrainMaxPosition_ = Mat4::Transform(terrainMaxPosition_, terrainWorldMatrix_);

	///- 最小値より何か一つでも小さければ範囲外
	if(playerTerrainLocalPosition_.x < terrainMinPosition_.x
	   || playerTerrainLocalPosition_.z < terrainMinPosition_.z) {
		return false;
	}

	///- 最大値より何か一つでも大きければ範囲外
	if(playerTerrainLocalPosition_.x > terrainMaxPosition_.x
	   || playerTerrainLocalPosition_.z > terrainMaxPosition_.z) {
		return false;
	}

	return true;
}

void TerrainCollider::CreateOutputBuffer() {
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	///- Output Bufferの生成
	outputBuffer_ = dxCommon->CreateBufferResource(sizeof(Vector4));
	outputBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&outputData_));

	///- Texcoord Bufferの生成
	texcoordResource_ = dxCommon->CreateBufferResource(sizeof(Vector2));
	texcoordResource_->Map(0, nullptr, reinterpret_cast<void**>(&texcoordData_));

}

void TerrainCollider::ImGuiDebug() {
#ifdef _DEBUG
	ImGui::Begin("Collider");

	ImGui::DragFloat2("Texcoord", &texcoord_.x, 0.01f, 0.0f, 1.0f);

	ImGui::Separator();

	ImGui::Text("Height : %f", height_);
	ImGui::Text("Texooord : %f,%f", texcoord_.x, texcoord_.y);

	ImGui::Separator();

	ImGui::Text("terrain min: %f, %f, %f", terrainMinPosition_.x, terrainMinPosition_.y, terrainMinPosition_.z);
	ImGui::Text("terrain max: %f, %f, %f", terrainMaxPosition_.x, terrainMaxPosition_.y, terrainMaxPosition_.z);

	ImGui::End();
#endif // _DEBUG
}



float TerrainCollider::GetHeight(const Vec2f& texcoord) {

	///- インスタンス確保
	ID3D12GraphicsCommandList* commandList = DxCommand::GetInstance()->GetList();
	PipelineStateObjectManager::GetInstance()->SetComputeCommandList(3, commandList);
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	///- Textureの取得
	TextureManager::Texture texture = TextureManager::GetInstance()->GetSrvTextureResource("dragon");

	*texcoordData_ = texcoord; ///- Texcoordの変更

	commandList->SetComputeRootConstantBufferView(0, texcoordResource_->GetGPUVirtualAddress());

	//// デスクリプタヒープの設定
	ID3D12DescriptorHeap* descriptorHeaps[] = { DxDescriptors::GetInstance()->GetSRVHeap() }; // yourDescriptorHeap は作成した CBV_SRV_UAV デスクリプタヒープ
	commandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
	commandList->SetComputeRootDescriptorTable(1, texture.handleGPU);

	commandList->SetComputeRootUnorderedAccessView(2, outputBuffer_->GetGPUVirtualAddress());

	commandList->Dispatch(1, 1, 1);

	dxCommon->CommnadExecuteAndWait();

	outputBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&outputData_));
	Vec4f result = *outputData_;
	result.y += result.x;
	result.y += result.z;

	Vec3f vertex = Mat4::Transform(
		{ result.x, result.y, result.z },
		Mat4::MakeScale(pTerrain_->GetWorldTransform().scale)
	);

	return vertex.y;
}

float TerrainCollider::GetHeight() {
	return height_;
}



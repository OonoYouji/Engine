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
	terrainWorldMatrix_ = pTerrain_->GetWorldTransform().matTransform;
	inverseTerrainWorldMatrix_ = Mat4::MakeInverse(terrainWorldMatrix_);

	CreateOutputBuffer();

	height_ = 0.0f;

}



void TerrainCollider::Update() {
	///- debug表示
	ImGuiDebug();

	preTexcoord_ = texcoord_;
	preHeight_ = height_;

	terrainWorldMatrix_ = pTerrain_->GetWorldTransform().matTransform;
	inverseTerrainWorldMatrix_ = Mat4::MakeInverse(terrainWorldMatrix_);

	texcoord_ = ConvertTexcoord(pPlayer_->GetWorldTransform().translate);
	height_ = GetHeight(texcoord_);

	if(height_ - preHeight_ < kWallHeight_) {
		if(pPlayer_->GetWorldTransform().translate.y < height_) {
			pPlayer_->SetHeight(height_);
		}
	} else {
		Vec3f position = ConvertPosition(preTexcoord_);
		pPlayer_->SetPosition(position);
	}

	pPlayer_->UpdateMatrix();
}



void TerrainCollider::SetPlayer(Player* player) {
	pPlayer_ = player;
}

void TerrainCollider::SetTerrain(Terrain* terrain) {
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
	return Vec2f(texcoord.x, 1.0f - texcoord.z);
}



Vec3f TerrainCollider::ConvertPosition(const Vec2f& texcoord) {

	///- terrain上のローカル座標にする
	Vec3f localPosition = Vec3f(texcoord.x, 0.0f, 1.0f - texcoord.y) * terrainSize_;

	///- world座標に変換
	Vec3f position = Mat4::Transform(localPosition, terrainWorldMatrix_);

	return position;
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

	ImGui::Text("Wall Height : %f", kWallHeight_);

	ImGui::Separator();

	ImGui::Text("Height : %f", height_);
	ImGui::Text("PreHeight : %f", preHeight_);

	ImGui::Separator();

	ImGui::Text("Texooord : %f,%f", texcoord_.x, texcoord_.y);
	ImGui::Text("PreTexooord : %f,%f", preTexcoord_.x, preTexcoord_.y);

	ImGui::Separator();

	ImGui::Text("terrain min: %f, %f, %f", terrainMinPosition_.x, terrainMinPosition_.y, terrainMinPosition_.z);
	ImGui::Text("terrain max: %f, %f, %f", terrainMaxPosition_.x, terrainMaxPosition_.y, terrainMaxPosition_.z);

	ImGui::Separator();

	ImGui::ColorEdit4("OutputColor", &outputColor_.x);

	ImGui::End();
#endif // _DEBUG
}



float TerrainCollider::GetHeight(const Vec2f& texcoord) {

	///- インスタンス確保
	ID3D12GraphicsCommandList* commandList = DxCommand::GetInstance()->GetList();
	PipelineStateObjectManager::GetInstance()->SetComputeCommandList("TerrainCS", commandList);
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	///- Textureの取得
	TextureManager::Texture srvTex = TextureManager::GetInstance()->GetSrvTexture("dragon");
	TextureManager::Texture uavTex = TextureManager::GetInstance()->GetUavTexture("128x128Texture");

	*texcoordData_ = texcoord; ///- Texcoordの変更

	commandList->SetComputeRootConstantBufferView(0, texcoordResource_->GetGPUVirtualAddress());
	commandList->SetComputeRootUnorderedAccessView(2, outputBuffer_->GetGPUVirtualAddress());

	///- Descriptorの設定
	ID3D12DescriptorHeap* descriptorHeaps[] = { DxDescriptors::GetInstance()->GetSRVHeap() };
	commandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

	///- Textureの設定
	commandList->SetComputeRootDescriptorTable(1, srvTex.handleGPU);
	commandList->SetComputeRootDescriptorTable(3, uavTex.handleGPU);

	commandList->Dispatch(1, 1, 1);

	///- コマンドに実行と終了まで待機
	dxCommon->CommnadExecuteAndWait();

	outputBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&outputData_));
	Vec4f result = *outputData_;
	outputColor_ = result;

	// 輝度の計算
	float luminance = 0.2126f * result.x + 0.7152f * result.y + 0.0722f * result.z;
	return luminance * pTerrain_->GetWorldTransform().scale.y;
}




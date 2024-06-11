#include <TerrainCollider.h>

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
	inverseTerrainWorldMatrix_ = Mat4::MakeInverse(pTerrain_->GetWorldTransform().worldMatrix);

	IsWithinRange();

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
	playerTerrainLocalPosition_ = Mat4::Transform(pPlayer_->GetWorldTransform().translate, inverseTerrainWorldMatrix_);

	///- PlayerのLocal座標をTerrianの大きさで割ってtexcoordを計算
	Vec3f texcoord = playerTerrainLocalPosition_ / terrainSize_;

	///- Terrainは横、奥に伸びているのでy座標は使わない
	return Vec2f(texcoord.x, texcoord.z);
}



bool TerrainCollider::IsWithinRange() {

	///- TerrainのLocal空間に変換
	playerTerrainLocalPosition_ = Mat4::Transform(pPlayer_->GetWorldTransform().translate, inverseTerrainWorldMatrix_);

	///- 最小値より何か一つでも小さければ範囲外
	if(playerTerrainLocalPosition_.x < terrainMinPosition_.x
	   || playerTerrainLocalPosition_.y < terrainMinPosition_.y
	   || playerTerrainLocalPosition_.z < terrainMinPosition_.z) {
		return false;
	}

	///- 最大値より何か一つでも大きければ範囲外
	if(playerTerrainLocalPosition_.x > terrainMaxPosition_.x
	   || playerTerrainLocalPosition_.y > terrainMaxPosition_.y
	   || playerTerrainLocalPosition_.z > terrainMaxPosition_.z) {
		return false;
	}

	return true;
}

void TerrainCollider::CreateOutputBuffer() {
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	outputBuffer_ = dxCommon->CreateBufferResource(sizeof(Vector4));


}

float TerrainCollider::GetHeight(const Vec2f& texcoord) {

	return 0.0f;
}



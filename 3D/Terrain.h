#pragma once

#include <d3d12.h>
#include <wrl/client.h>

#include "DirectXCommon.h"
#include <cmath>
#include "WorldTransform.h"
#include "Vector3.h"
#include "Matrix4x4.h"
#include "DirectionalLight.h"


using namespace Microsoft::WRL;

class PerlinNoise;

/// <summary>
/// 地形
/// </summary>
class Terrain {
public:

	Terrain();
	~Terrain();

	void Init();

	void Update();

	void Draw();

	///- 縦横の分割数
	static const int kSubdivision = 100;


private:


	///- 描画用Resource
	ComPtr<ID3D12Resource> vertexResource_;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_;
	ComPtr<ID3D12Resource> materialResource_;
	ComPtr<ID3D12Resource> wvpResource_;
	ComPtr<ID3D12Resource> indexResource_;
	D3D12_INDEX_BUFFER_VIEW indexBuffer_;

	///- 書き込み用データ
	std::vector<std::vector<VertexData>> vertexData_;
	std::vector<VertexData> flattenedVertexData_;
	VertexData* pVertexData_ = nullptr;
	void* pVertexMappedData_ = nullptr;

	std::vector<uint32_t> indexData_;
	uint32_t* pIndexData_;
	void* pIndexMappedData_;

	Material* materialData_ = nullptr;
	TransformMatrix* matrixData_ = nullptr;

	WorldTransform worldTransform_;
	Vector4 color_;


	///- 法線ベクトル
	Vec3f normalVector_;

	float distance_;

	float noisePower_;

	///- noise取得用
	std::unique_ptr<PerlinNoise> noise_;


	/// <summary>
	/// 法線ベクトルの計算
	/// </summary>
	void NormalVector();

public:

	const Vec3f& GetNormalVector() const { return normalVector_; }

	float GetDistance() const { return distance_; }

	const Vec3f& GetWorldPos() const { return worldTransform_.translate; }

	Vec3f GetLTPos() const {
		return worldTransform_.translate + Vec3f{
			-kSubdivision / 2.0f, 0.0f, kSubdivision / 2.0f
		};
	}


	Vec3f GetRBPos() const {
		return worldTransform_.translate + Vec3f{
			kSubdivision / 2.0f, 0.0f, -kSubdivision / 2.0f
		};
	}


	/// <summary>
	/// 頂点データをフラット化する
	/// </summary>
	void TransferVertexData();

	void TransferFlattenedVertexData();

	void SetVertexHeight(int row, int col, float height);

	void AddVertexHeight(int row, int col, float height);
	void SubVertexHeight(int row, int col, float height);

	/// <summary>
	/// 配列の範囲内か確認する
	/// 範囲内: True,  範囲外: False
	/// </summary>
	bool CheckRange(int row, int col);

};
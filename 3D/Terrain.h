#pragma once

#include <d3d12.h>
#include <wrl/client.h>

#include "DirectXCommon.h"
#include <cmath>
#include "WorldTransform.h"
#include "Vector3.h"
#include "Matrix4x4.h"

using namespace Microsoft::WRL;

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

private:

	///- 縦横の分割数
	//static const int kSubdivision_ = 100;
	int kSubdivision_;


	///- 描画用Resource
	ComPtr<ID3D12Resource> vertexResource_;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_;
	ComPtr<ID3D12Resource> materialResource_;
	ComPtr<ID3D12Resource> wvpResource_;
	ComPtr<ID3D12Resource> indexResource_;
	D3D12_INDEX_BUFFER_VIEW indexBuffer_;

	///- 書き込み用データ
	std::vector<VertexData> vertexData_;
	VertexData* pVertexData_ = nullptr;
	void* pVertexMappedData_ = nullptr;

	std::vector<uint32_t> indexData_;
	uint32_t* pIndexData_;
	void* pIndexMappedData_;

	Vector4* materialData_ = nullptr;
	Matrix4x4* wvpData_ = nullptr;

	WorldTransform worldTransform_;
	Vector4 color_;


	///- 法線ベクトル
	Vec3f normalVector_;

	float distance_;


	/// <summary>
	/// 法線ベクトルの計算
	/// </summary>
	void NormalVector();

public:

	const Vec3f& GetNormalVector() const { return normalVector_; }

	float GetDistance() const { return distance_; }

};
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
	static const int kVerticalDivisionNum_ = 1024;
	static const int kHorizontalDivisionNum_ = 1024;


	///- 描画用Resource
	ComPtr<ID3D12Resource> vertexResource_;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_;
	ComPtr<ID3D12Resource> materialResource_;
	ComPtr<ID3D12Resource> wvpResource_;

	///- 書き込み用データ
	std::vector<VertexData> vertexData_;
	VertexData* pData_ = nullptr;
	void* pMappedData_ = nullptr;
	Vector4* materialData_ = nullptr;
	Matrix4x4* wvpData_ = nullptr;

	WorldTransform worldTransform_;
	Vector4 color_;


};
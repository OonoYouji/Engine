#pragma once

#include <d3d12.h>
#include <wrl/client.h>

#include <cmath>
#include <vector>

#include "DirectXCommon.h"
#include "Vector4.h"
#include "Matrix4x4.h"
#include "WorldTransform.h"
#include <CBuffer.h>


using namespace Microsoft::WRL;

/// <summary>
/// スフィア
/// </summary>
class Sphere {
public:

	Sphere();
	~Sphere();

	void Init();
	void Draw();

	void DebugDraw();

private:
	const uint32_t subdivision_ = 16;

	ComPtr<ID3D12Resource> vertexResource_;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_;
	ComPtr<ID3D12Resource> materialResource_;
	ComPtr<ID3D12Resource> transformMatrixResource_;

	ComPtr<ID3D12Resource> indexResource_;
	D3D12_INDEX_BUFFER_VIEW indexBuffer_;

	std::vector<uint32_t> indexData_;
	uint32_t* pIndexData_ = nullptr;
	void* pIndexMappedData_ = nullptr;

	std::vector<VertexData> vertexData_;
	VertexData* pData_ = nullptr;
	void* pMappedData_ = nullptr;

	Material* materialData_ = nullptr;
	TransformMatrix* transformMatrixData_ = nullptr;

	Vec4f color_;

	WorldTransform worldTransform_;




};
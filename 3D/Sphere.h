#pragma once

#include <d3d12.h>
#include <wrl/client.h>

#include <cmath>
#include <vector>

#include "DirectXCommon.h"
#include "Vector4.h"
#include "Matrix4x4.h"
#include "WorldTransform.h"


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

private:

	ComPtr<ID3D12Resource> vertexResource_;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_;
	ComPtr<ID3D12Resource> materialResource_;
	ComPtr<ID3D12Resource> wvpResource_;

	std::vector<VertexData> vertexData_;
	VertexData* pData_ = nullptr;
	void* pMappedData_ = nullptr;
	Vec4f* materialData_ = nullptr;
	Matrix4x4* wvpData_ = nullptr;

	Vec4f color_;

	WorldTransform worldTransform_;


	uint32_t subdivision_;


};
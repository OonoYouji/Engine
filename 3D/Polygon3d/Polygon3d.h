#pragma once

#include <wrl/client.h>
#include <d3d12.h>

#include <cmath>
#include <vector>

#include <CBuffer.h>

using namespace Microsoft::WRL;


/// <summary>
/// Textureなし図形
/// </summary>
class Polygon3d {
public:

	Polygon3d();
	~Polygon3d();

	void Initialize();
	void Draw();

private:

	///- 頂点
	ComPtr<ID3D12Resource> vertexResource_;
	D3D12_VERTEX_BUFFER_VIEW verrexBufferView_;
	std::vector<Polygon3d> vertexData_;

	///- 頂点インデックス
	ComPtr<ID3D12Resource> indexResource_;
	D3D12_INDEX_BUFFER_VIEW indexBufferView_;
	std::vector<uint32_t> indexData_;

	///- 色
	ComPtr<ID3D12Resource> materialResource_;
	Material* materialData_ = nullptr;

	///- 行列
	ComPtr<ID3D12Resource> matrixResource_;
	TransformMatrix* transformationMatrixData_ = nullptr;

};
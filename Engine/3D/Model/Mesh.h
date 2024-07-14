#pragma once

#include <d3d12.h>
#include <wrl/client.h>

#include <vector>
#include <string>

#include <CBuffer.h>


using namespace Microsoft::WRL;

/// ===================================================
/// 
/// ===================================================
class Mesh final {
public:

	Mesh();
	~Mesh();

	/// ===================================================
	/// public : methods
	/// ===================================================

	void AddVertex(const VertexData& vertex);

	void AddIndex(uint32_t index);

	void CreateBuffer();

	const std::vector<uint32_t> GetIndices() const { return indices_; }

	const std::vector<VertexData>& GetVertices() const { return vertices_; }

private:

	void CreataVertexBuffer();

	void CreateIndexBuffer();

private:

	/// ===================================================
	/// shader : buffers
	/// ===================================================

	///- vertex
	ComPtr<ID3D12Resource> vertexBuffer_;
	D3D12_VERTEX_BUFFER_VIEW vbv_;
	std::vector<VertexData> vertices_;

	///- index
	ComPtr<ID3D12Resource> indexBuffer_;
	D3D12_INDEX_BUFFER_VIEW ibv_;
	std::vector<uint32_t> indices_;

};
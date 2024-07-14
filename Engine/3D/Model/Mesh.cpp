#include "Mesh.h"

#include <DirectXCommon.h>


namespace {

	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

} ///- namespace


Mesh::Mesh() {}
Mesh::~Mesh() {}



/// ===================================================
/// 頂点データの追加
/// ===================================================
void Mesh::AddVertex(const VertexData& vertex) {
	vertices_.push_back(vertex);
}



/// ===================================================
/// インデックスの追加
/// ===================================================
void Mesh::AddIndex(uint32_t index) {
	indices_.push_back(index);
}



/// ===================================================
/// バッファの生成
/// ===================================================
void Mesh::CreateBuffer() {
	CreataVertexBuffer();
	CreateIndexBuffer();
}



/// ===================================================
/// 頂点バッファの生成
/// ===================================================
void Mesh::CreataVertexBuffer() {
	///- バッファの生成
	vertexBuffer_ = dxCommon->CreateBufferResource(sizeof(VertexData) * vertices_.size());

	vbv_.BufferLocation = vertexBuffer_->GetGPUVirtualAddress();
	vbv_.SizeInBytes = UINT(sizeof(VertexData) * vertices_.size());
	vbv_.StrideInBytes = sizeof(VertexData);

	///- 頂点リソースにデータを書き込む
	VertexData* vertexData = nullptr;
	vertexBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
	std::memcpy(vertexData, vertices_.data(), sizeof(VertexData) * vertices_.size());

}



/// ===================================================
/// インデックスバッファの生成
/// ===================================================
void Mesh::CreateIndexBuffer() {

	///- インデックスバッファの生成
	indexBuffer_ = dxCommon->CreateBufferResource(sizeof(uint32_t) * indices_.size());

	///- ビューの初期化
	ibv_.BufferLocation = indexBuffer_->GetGPUVirtualAddress();
	ibv_.SizeInBytes = static_cast<UINT>(sizeof(uint32_t) * indices_.size());
	ibv_.Format = DXGI_FORMAT_R32_UINT;

	///- データのマップ
	uint32_t* indexData = nullptr;
	indexBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&indexData));
	std::memcpy(indexData, indices_.data(), sizeof(uint32_t) * indices_.size());
}

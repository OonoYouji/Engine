#pragma once

#include <d3d12.h>
#include "WorldTransform.h"
#include <wrl/client.h>
#include "Vector4.h"
#include "Vector2.h"
#include <vector>
#include <string>

#include <CBuffer.h>

///- ComPtrの省略用
using namespace Microsoft::WRL;

struct VertexData;
class DirectXCommon;


/// <summary>
/// 2Dテクスチャ
/// </summary>
class Sprite {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	Sprite();
	~Sprite();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

private:

	/// ===================================================
	/// private : methods
	/// ===================================================

	void CreateVertexResource();

	void CreateIndexResource();

	void CreateMaterialResource();

	void CreateTransformaitonResource();

public: ///- 

	Vec2f position;
	Vec4f color;
	Vec2f anchor;

private:

	/// ===================================================
	/// other class : pointer
	/// ===================================================

	DirectXCommon* dxCommon_ = nullptr;



	/// ===================================================
	/// shader : buffers
	/// ===================================================

	///- 頂点データ
	ComPtr<ID3D12Resource> vertexResource_;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_;
	
	///- インデックスデータ
	ComPtr<ID3D12Resource> indexResource_;
	D3D12_INDEX_BUFFER_VIEW indexBufferView_;

	///- cbvデータ
	ComPtr<ID3D12Resource> transformationMatrixResource_;
	ComPtr<ID3D12Resource> materialResource_;

	Material* materialData_;
	TransformMatrix* transformationMatrixData_;

	WorldTransform worldTransform_;

	VertexData* vertexData_;
	uint32_t* indexData_ = nullptr;
	std::vector<Vector4> localVertex_;

	Vec3f uvScale_;
	Vec3f uvRotate_;
	Vec3f uvTranslate_;

};
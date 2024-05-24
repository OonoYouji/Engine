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


/// <summary>
/// 2Dテクスチャ
/// </summary>
class Sprite {
public:

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

	/// <summary>
	/// ImGuiを使って編集
	/// </summary>
	void DebugDraw(const std::string& windowName);

public: ///- 

	Vec2f position;
	Vec4f color;

private: ///- メンバ変数

	ComPtr<ID3D12Resource> vertexResource_;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_;
	ComPtr<ID3D12Resource> transformationMatrixResource_;
	ComPtr<ID3D12Resource> materialResource_;

	ComPtr<ID3D12Resource> indexResource_;
	D3D12_INDEX_BUFFER_VIEW indexBufferView_;

	WorldTransform worldTransform_;

	Material* materialData_;
	VertexData* vertexData_;
	uint32_t* indexData_ = nullptr;
	std::vector<Vector4> localVertex_;
	TransformMatrix* transformationMatrixData_;

	Vec3f uvScale_;
	Vec3f uvRotate_;
	Vec3f uvTranslate_;

};
#pragma once

#include <Vector3.h>

#include <DirectXCommon.h>

using namespace Microsoft::WRL;


/// ===================================================
/// 線の描画
/// ===================================================
class LineDrawer final {
	LineDrawer() = default;
	~LineDrawer() = default;
public:

	/// ===================================================
	/// public : nested struct
	/// ===================================================

	struct VertexData {
		Vec3f position;
		Vec4f color;
	};

public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	static LineDrawer* GetInstance();

	void PreDraw();

	void PostDraw();

	void Draw(const Vec3f& v1, const Vec3f& v2, const Vec4f& color);

private:

	/// ===================================================
	/// static : objects
	/// ===================================================
	static const int kVertexCount_ = 2;


	/// ===================================================
	/// shader : buffers
	/// ===================================================

	ComPtr<ID3D12Resource> vertexBuffer_;
	D3D12_VERTEX_BUFFER_VIEW vbv_;
	ComPtr<ID3D12Resource> indexBuffer_;
	D3D12_INDEX_BUFFER_VIEW ibv_;

	LineDrawer::VertexData* vertexData_;
	uint32_t* indexData_;

private:
	LineDrawer(const LineDrawer&) = delete;
	LineDrawer& operator= (const LineDrawer&) = delete;
	LineDrawer& operator= (LineDrawer&&) = delete;
};
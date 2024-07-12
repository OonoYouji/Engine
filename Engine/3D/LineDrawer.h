#pragma once

#include <Vector3.h>
#include <Vector4.h>

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
		Vec4f position;
		Vec4f color;
	};

public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	static LineDrawer* GetInstance();

	void Initialize();

	void Finalize();

	void PreDraw();

	void PostDraw();

	void Draw(const Vec3f& v1, const Vec3f& v2, const Vec4f& color);

private:
	
	/// ===================================================
	/// private : methods
	/// ===================================================

	void CreateVertexBuffer(size_t vertexCount);

	void CreateViewProjectionBuffer();

private:

	/// ===================================================
	/// static : objects
	/// ===================================================
	static const int kVertexCount_ = 2;
	
	
	/// ===================================================
	/// other class : pointer
	/// ===================================================
	DirectXCommon* dxCommon_ = nullptr;


	/// ===================================================
	/// shader : buffers
	/// ===================================================

	ComPtr<ID3D12Resource> vertexBuffer_;
	D3D12_VERTEX_BUFFER_VIEW vbv_;

	std::vector<LineDrawer::VertexData> vertices_;

	ComPtr<ID3D12Resource> viewProjectionBuffer_;
	Mat4* viewProjectionData_ = nullptr;


	int vertexUsedCount_ = 0;

private:
	LineDrawer(const LineDrawer&) = delete;
	LineDrawer& operator= (const LineDrawer&) = delete;
	LineDrawer& operator= (LineDrawer&&) = delete;
};
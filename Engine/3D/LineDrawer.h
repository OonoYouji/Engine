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

	/// <summary>
	/// インスタンス確保
	/// </summary>
	/// <returns></returns>
	static LineDrawer* GetInstance();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 終了処理
	/// </summary>
	void Finalize();

	/// <summary>
	/// 描画前処理
	/// </summary>
	void PreDraw();

	/// <summary>
	/// 描画後処理
	/// </summary>
	void PostDraw();

	/// <summary>
	/// 線の描画
	/// </summary>
	void Draw(const Vec3f& v1, const Vec3f& v2, const Vec4f& color);

private:
	
	/// ===================================================
	/// private : methods
	/// ===================================================

	/// <summary>
	/// 頂点バッファの生成
	/// </summary>
	/// <param name="vertexCount"></param>
	void CreateVertexBuffer(size_t vertexCount);

	/// <summary>
	/// ビュープロジェクションバッファの生成
	/// </summary>
	void CreateViewProjectionBuffer();

private:

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

private:
	LineDrawer(const LineDrawer&) = delete;
	LineDrawer& operator= (const LineDrawer&) = delete;
	LineDrawer& operator= (LineDrawer&&) = delete;
};
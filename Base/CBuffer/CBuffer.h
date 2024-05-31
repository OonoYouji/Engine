#pragma once 

#include <wrl/client.h>
#include <d3d12.h>

#include <cmath>

#include <Vector2.h>
#include <Vector3.h>
#include <Vector4.h>
#include <Matrix3x3.h>
#include <Matrix4x4.h>

using namespace Microsoft::WRL;


/// <summary>
/// Textureあり図形の頂点情報
/// </summary>
struct VertexData {
	Vec4f position;
	Vec2f texcoord;
	Vec3f normal;
};


/// <summary>
/// Textureなし図形の頂点情報
/// </summary>
struct Polygon3dVertexData {
	Vec4f position;
	Vec3f normal;
};


/// <summary>
/// 色情報
/// </summary>
struct Material {
	Vec4f color;
	int32_t enableLighting;
private:
	float p[3];
public:
	Mat4 uvTransform;
};


/// <summary>
/// 行列情報
/// </summary>
struct TransformMatrix {
	Mat4 WVP;
	Mat4 World;
};



class Object3dResources final {
public:

	Object3dResources();
	~Object3dResources();

	void Release();

	void CreateVertexResource(size_t size);

	void CreateIndexResouces(size_t size);

	void CreateMaterialResource();

	void CreateTransformationMatrix();

	void SetConstantBufferView(ID3D12GraphicsCommandList* commandList);

public:
	///- 頂点リソース
	ComPtr<ID3D12Resource> vertexResource_;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_;
	VertexData* vertexData_ = nullptr;

	///- インデックスリソース
	ComPtr<ID3D12Resource> indexResource_;
	D3D12_INDEX_BUFFER_VIEW indexBufferView_;
	uint32_t* indexData_ = nullptr;

	///- マテリアルリソース
	ComPtr<ID3D12Resource> materialResource_;
	Material* materialData_ = nullptr;

	///- 行列リソース
	ComPtr<ID3D12Resource> transformationMatrixResource_;
	TransformMatrix* transformationMatrixData_ = nullptr;
};
#pragma once

#include <d3d12.h>
#include <wrl/client.h>

#include <vector>

#include "Vector2.h"
#include "Vector4.h"
#include "Matrix4x4.h"
#include "WorldTransform.h"
#include "DirectXCommon.h"



/// <summary>
/// 地形を弄るためのブラシ
/// </summary>
class Brush {
public:

	Brush();
	~Brush();

	void Init();

	void Update();

	void Draw();

private: ///- メンバ変数

	static const int kVertexCount_ = 64;
	static const int kTriangleVertexCount_ = 3;

	///- 描画用Resource
	ComPtr<ID3D12Resource> vertexResource_;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_;
	ComPtr<ID3D12Resource> materialResource_;
	ComPtr<ID3D12Resource> wvpResource_;

	///- 書き込み用データ
	std::vector<VertexData> vertexData_;
	VertexData* pData_ = nullptr;
	void* pMappedData_;
	Material* materialData_ = nullptr;
	TransformMatrix* matrixData_  = nullptr;


	Vec3f mousePos_;
	WorldTransform worldTransform_;

	Vec3f mouseLayDirection_;

	float distanceTestObject_;
	float circleRadius_;

	Vec3f posNear_;
	Vec3f posFar_;


	void ConvertMousePosition();



public: ///- 

	const Vec3f& GetLayDir() const { return mouseLayDirection_; }

	const Vec3f& GetNearPos() const { return posNear_; }
	const Vec3f& GetFarPos() const { return posFar_; }

	void SetWorldPosition(const Vec3f& position) {
		worldTransform_.translate = position;
	}

	void SetColot(const Vec4f& color);

};
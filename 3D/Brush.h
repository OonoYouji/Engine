#pragma once

#include <d3d12.h>
#include <wrl/client.h>

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

	///- 描画用Resource
	ComPtr<ID3D12Resource> vertexResource_;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_;
	ComPtr<ID3D12Resource> materialResource_;
	ComPtr<ID3D12Resource> wvpResource_;

	///- 書き込み用データ
	VertexData* vertexData_ = nullptr;
	Vector4* materialData_ = nullptr;
	Matrix4x4* wvpData_ = nullptr;

	Vec3f mousePos_;
	WorldTransform worldTransform_;

	float circleRadius_;

	void ConvertMousePosition();

};
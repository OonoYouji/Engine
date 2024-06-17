#pragma once

#include <d3d12.h>
#include <d3dx12.h>
#include <wrl/client.h>
#include <opencv.hpp>


#include "DirectXCommon.h"
#include <cmath>
#include "WorldTransform.h"
#include "Vector3.h"
#include "Matrix4x4.h"
#include "DirectionalLight.h"
#include "CBuffer.h"

#include <GameObject.h>

using namespace Microsoft::WRL;

class PerlinNoise;

/// <summary>
/// 地形
/// </summary>
class Terrain
	: public GameObject {
public:

	Terrain();
	~Terrain();

	void Init();

	void Update();

	void Draw();

public: ///- METHODS AND ACCESSOR

	const Vec3f& GetNormalVector() const { return normalVector_; }

	float GetDistance() const { return distance_; }

	const Vec3f& GetWorldPos() const { return worldTransform_.translate; }

	/// <summary>
	/// 頂点データをフラット化する
	/// </summary>
	void TransferVertexData();

	void TransferFlattenedVertexData();

	void SetVertexHeight(int row, int col, float height);

	void AddVertexHeight(int row, int col, float height);
	void SubVertexHeight(int row, int col, float height);

	/// <summary>
	/// 配列の範囲内か確認する
	/// 範囲内: True,  範囲外: False
	/// </summary>
	bool CheckRange(int row, int col);

	/// <summary>
	/// 頂点データを得る
	/// </summary>
	/// <returns></returns>
	const std::vector<VertexData>& GetVertexDatas() const { return flattenedVertexData_; }

private: ///- METHODS

	/// <summary>
	/// 法線ベクトルの計算
	/// </summary>
	void NormalVector();

	/// <summary>
	/// 縦横の最大頂点数からIndexDataを計算する
	/// </summary>
	void IndexDataCulc(uint32_t maxRow, uint32_t maxCol);

	/// <summary>
	/// 縦横の最大頂点数からVertexDataを計算する
	/// </summary>
	void VertexDataCulc(uint32_t maxRow, uint32_t maxCol);

	/// <summary>
	/// VertexResourceを生成する
	/// </summary>
	void CreateVertexResource(size_t flattendVertexDataSize);

	/// <summary>
	/// IndexResourceを生成する
	/// </summary>
	void CreateIndexResource(size_t indexDataSize);

	/// <summary>
	/// ReadBackResourcecを作成する
	/// </summary>
	void CreateReadBackResource();

	/// <summary>
	/// GPUから読み込む
	/// </summary>
	void ReadBack();

	/// <summary>
	/// 編集した地形を画像として出力する
	/// </summary>
	void OutputImage();

private: ///- OBJECTS



	///- 縦横分割比
	int rowSubdivision_;
	int colSubdivision_;


	///- 描画用Resource
	ComPtr<ID3D12Resource> vertexResource_;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_;
	ComPtr<ID3D12Resource> materialResource_;
	ComPtr<ID3D12Resource> matrixResource_;
	ComPtr<ID3D12Resource> indexResource_;
	D3D12_INDEX_BUFFER_VIEW indexBuffer_;

	///- 書き込み用データ
	std::vector<std::vector<VertexData>> vertexData_;
	std::vector<VertexData> flattenedVertexData_;
	VertexData* pVertexData_ = nullptr;

	std::vector<uint32_t> indexData_;
	uint32_t* pIndexData_;

	Material* materialData_ = nullptr;
	TransformMatrix* matrixData_ = nullptr;

	Vector4 color_;


	///- 法線ベクトル
	Vec3f normalVector_;
	float distance_;

	///- noise取得用
	std::unique_ptr<PerlinNoise> noise_;
	float noisePower_;


	///- GPUで操作した地形をCPUに戻す
	ComPtr<ID3D12Resource> readBackResource_;
	ID3D12Resource* pTexture_ = nullptr;
	void* readBackData_ = nullptr;
	UINT rowPitch_;

};
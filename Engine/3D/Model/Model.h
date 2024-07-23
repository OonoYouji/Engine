#pragma once

#include <wrl/client.h>

#include <utility>
#include <map>
#include <unordered_map>
#include <vector>
#include <string>
#include <memory>
#include <array>

#include <DirectXCommon.h>
#include <WorldTransform.h>
#include <CBuffer.h>

#include <Vector3.h>


using namespace Microsoft::WRL;


/// ===================================================
/// 3dモデル
/// ===================================================
class Model {
private:
	/// ===================================================
	/// private : nested struct
	/// ===================================================
	struct MaterialData {
		std::string textureFilePath;
		std::string textureName;
	};

public:

	Model();
	~Model();

	/// ===================================================
	/// public : methods
	/// ===================================================

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="directoryPath"></param>
	/// <param name="fileName"></param>
	void Initialize(const std::string& directoryPath, const std::string& fileName);

	/// <summary>
	/// モデルの描画
	/// </summary>
	/// <param name="worldTransform"></param>
	void Draw(const WorldTransform& worldTransform, const Mat4& uvTransform = Mat4::MakeIdentity());
	void Draw(const WorldTransform& worldTransform, const Material& material);

	/// <summary>
	/// 描画前処理
	/// </summary>
	void PreDraw();

	/// <summary>
	/// 描画後処理
	/// </summary>
	void PostDraw();

	/// <summary>
	/// 頂点データのGetter
	/// </summary>
	/// <returns></returns>
	const std::vector<VertexData>& GetVertexDatas() const {
		return vertices_;
	}

	/// <summary>
	/// インスタンスカウントのリセット
	/// </summary>
	void Reset();

	/// <summary>
	/// objファイルの読み込み
	/// </summary>
	/// <param name="directoryPath"></param>
	/// <param name="fileName"></param>
	/// <returns></returns>
	Model LoadObjFile(const std::string& directoryPath, const std::string& fileName);

	/// <summary>
	/// mtlファイルの読み込み
	/// </summary>
	/// <param name="directoryPath"></param>
	/// <param name="fileName"></param>
	/// <returns></returns>
	MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& fileName);

	/// <summary>
	/// 色のSetter
	/// </summary>
	/// <param name="color"></param>
	void SetColor(const Vec4f& color) {
		materialData_->color = color;
	}

private: 

	/// ===================================================
	/// private : methods
	/// ===================================================

	void CreateVertexBuffer();

	void CreateIndexBuffer();

	void CreateMaterialBuffer();
	
	void CreateTransformBuffer();

private:
	///- モデルの描画上限
	static const int kMaxInstanceCount_ = 10;
	int instanceCount_;

	std::string name_;


	/// ===================================================
	/// other class : pointer
	/// ===================================================
	
	DirectXCommon* dxCommon_ = nullptr;
	DxDescriptors* dxDescriptors_ = nullptr;


	/// ===================================================
	/// shader : buffers
	/// ===================================================

	std::vector<VertexData> vertices_;
	MaterialData material_;

	///- Vertex
	ComPtr<ID3D12Resource> vertexBuffer_;
	D3D12_VERTEX_BUFFER_VIEW vbv_;

	///- index
	ComPtr<ID3D12Resource> indexBuffer_;
	D3D12_INDEX_BUFFER_VIEW ibv_;
	std::vector<uint32_t> indices_;

	///- Material
	ComPtr<ID3D12Resource> materialBuffer_;
	Material* materialData_;
	D3D12_GPU_DESCRIPTOR_HANDLE materialGpuHandle_;

	///- TransformMatrix
	ComPtr<ID3D12Resource> transformBuffer_;
	TransformMatrix* transformMatrixDatas_;
	D3D12_GPU_DESCRIPTOR_HANDLE transformGpuHandle_;


};



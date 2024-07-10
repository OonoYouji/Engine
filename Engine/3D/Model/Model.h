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


/// <summary>
/// 3Dモデル
/// </summary>
class Model {
private:
	struct MaterialData {
		std::string textureFilePath;
		std::string textureName;
	};
public:

	Model();
	~Model();

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
		return vertexDatas_;
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

	void SetUvTransform(const Mat4& uvTransform);

private:
	///- モデルの描画上限
	static const int kMaxInstanceCount_ = 10;
	int instanceCount_;

	std::string name_;

	std::vector<VertexData> vertexDatas_;
	MaterialData material_;

	///- Vertex
	ComPtr<ID3D12Resource> vertexResource_;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_;
	VertexData* vertexData_ = nullptr;

	///- Material
	ComPtr<ID3D12Resource> materialResource_;
	Material* materialData_;
	D3D12_GPU_DESCRIPTOR_HANDLE materialGpuHandle_;

	///- TransformMatrix
	ComPtr<ID3D12Resource> transformMatrixResource_;
	TransformMatrix* transformMatrixDatas_;
	D3D12_GPU_DESCRIPTOR_HANDLE transformGpuHandle_;


};



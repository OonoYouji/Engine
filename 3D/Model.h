#pragma once

#include <wrl/client.h>

#include <vector>
#include <string>
#include <memory>

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

	void Initialize(const std::string& directoryPath, const std::string& fileName);

	void Draw(const WorldTransform& worldTransform);

	void DebugDraw(const std::string& windowName);

	const std::vector<VertexData>& GetVertexDatas() const {
		return vertexDatas_;
	}

private:


	std::vector<VertexData> vertexDatas_;
	MaterialData material_;

	///- Vertex
	ComPtr<ID3D12Resource> vertexResource_;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_;
	VertexData* vertexData_ = nullptr;

	///- Material
	ComPtr<ID3D12Resource> materialResource_;
	Material* materialData_ = nullptr;

	///- TransformMatrix
	ComPtr<ID3D12Resource> transformMatrixResource_;
	TransformMatrix* transformMatrixData_ = nullptr;

public:

	Model LoadObjFile(const std::string& directoryPath, const std::string& fileName);

	MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& fileName);

	void SetColor(const Vec4f& color) {
		materialData_->color = color;
	}

};
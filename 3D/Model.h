#pragma once

#include <wrl/client.h>

#include <vector>
#include <string>
#include <memory>

#include <DirectXCommon.h>
#include <WorldTransform.h>

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

	void Draw();

	void DebugDraw(const std::string& windowName);

private:

	
	std::vector<VertexData> vertices_;
	MaterialData material_;
	WorldTransform worldTransform_;

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

	void SetWorldTransform(const WorldTransform& worldTransform) {
		worldTransform_ = worldTransform;
	}

};
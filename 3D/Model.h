#pragma once

#include <vector>
#include <string>
#include <memory>

#include <wrl/client.h>

#include <DirectXCommon.h>
#include <WorldTransform.h>

using namespace Microsoft::WRL;


/// <summary>
/// 3Dモデル
/// </summary>
class Model {
public:


	Model();
	~Model();

	void Initialize(const std::string& directoryPath, const std::string& fileName);

	void Draw();

private:

	
	std::vector<VertexData> vertices_;
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


};
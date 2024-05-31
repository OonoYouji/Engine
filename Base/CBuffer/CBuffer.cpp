#include <CBuffer.h>

#include <DirectXCommon.h>


Object3dResources::Object3dResources() {}
Object3dResources::~Object3dResources() {
	Release();
}


void Object3dResources::Release() {
	vertexResource_.Reset();
	indexResource_.Reset();
	materialResource_.Reset();
	transformationMatrixResource_.Reset();
}


void Object3dResources::CreateVertexResource(size_t size) {
	vertexResource_ = DirectXCommon::GetInstance()->CreateBufferResource(sizeof(VertexData) * size);
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));

	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	vertexBufferView_.SizeInBytes = static_cast<UINT>(sizeof(VertexData) * size);
	vertexBufferView_.StrideInBytes = sizeof(VertexData);
}

void Object3dResources::CreateIndexResouces(size_t size) {
	indexResource_ = DirectXCommon::GetInstance()->CreateBufferResource(sizeof(uint32_t) * size);
	indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&indexData_));

	indexBufferView_.BufferLocation = indexResource_->GetGPUVirtualAddress();
	indexBufferView_.SizeInBytes = static_cast<UINT>(sizeof(uint32_t) * size);
	indexBufferView_.Format = DXGI_FORMAT_R32_UINT;

}

void Object3dResources::CreateMaterialResource() {
	materialResource_ = DirectXCommon::GetInstance()->CreateBufferResource(sizeof(Material));
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
}

void Object3dResources::CreateTransformationMatrix() {
	transformationMatrixResource_ = DirectXCommon::GetInstance()->CreateBufferResource(sizeof(TransformMatrix));
	transformationMatrixResource_->Map(0, nullptr, reinterpret_cast<void**>(&transformationMatrixData_));
}

void Object3dResources::SetConstantBufferView(ID3D12GraphicsCommandList* commandList) {
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	commandList->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
	commandList->SetGraphicsRootConstantBufferView(1, transformationMatrixResource_->GetGPUVirtualAddress());
}

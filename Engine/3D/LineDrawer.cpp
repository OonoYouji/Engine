#include <LineDrawer.h>

#include <Engine.h>

#include <DxCommand.h>
#include <PipelineStateObjectManager.h>


LineDrawer* LineDrawer::GetInstance() {
	static LineDrawer instance;
	return &instance;
}

void LineDrawer::Initialize() {
	dxCommon_ = DirectXCommon::GetInstance();
	CreateViewProjectionBuffer();
}


void LineDrawer::Finalize() {
	vertexBuffer_.Reset();
	viewProjectionBuffer_.Reset();
}



void LineDrawer::PreDraw() {
	vertexUsedCount_ = 0;
	vertexData_.clear();
}



void LineDrawer::PostDraw() {

	ID3D12GraphicsCommandList* commandList = DxCommand::GetInstance()->GetList();
	PipelineStateObjectManager::GetInstance()->SetCommandList("Line", commandList);

	CreateVertexBuffer(vertexUsedCount_);

	*viewProjectionData_ = Engine::GetCamera()->GetVpMatrix();

	commandList->IASetVertexBuffers(0, 1, &vbv_);
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);

	commandList->SetGraphicsRootConstantBufferView(0, viewProjectionBuffer_->GetGPUVirtualAddress());

	commandList->DrawInstanced(static_cast<UINT>(vertexUsedCount_), 1, 0, 0);

}



void LineDrawer::Draw(const Vec3f& v1, const Vec3f& v2, const Vec4f& color) {

	vertexData_.push_back({ { v1.x, v1.y, v1.z, 1.0f },color });
	vertexData_.push_back({ { v2.x, v2.y, v2.z, 1.0f },color });

	vertexUsedCount_ += 2;

}



void LineDrawer::CreateVertexBuffer(size_t vertexCount) {

	vertexBuffer_ = dxCommon_->CreateBufferResource(sizeof(VertexData) * vertexData_.size());

	vbv_.BufferLocation = vertexBuffer_->GetGPUVirtualAddress(); //- リソースの先頭アドレス
	vbv_.SizeInBytes = static_cast<UINT>(sizeof(LineDrawer::VertexData) * vertexData_.size());		 //- 使用する頂点のデータ
	vbv_.StrideInBytes = static_cast<UINT>(sizeof(LineDrawer::VertexData));		 //- 使用する頂点の1つ分のデータ

	VertexData* vertexData = nullptr;
	vertexBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
	std::memcpy(vertexData, vertexData_.data(), sizeof(VertexData) * vertexData_.size());


}

void LineDrawer::CreateViewProjectionBuffer() {

	///- resourceの生成
	viewProjectionBuffer_ = dxCommon_->CreateBufferResource(sizeof(Matrix4x4));

	///- データのmap
	viewProjectionBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&viewProjectionData_));

}

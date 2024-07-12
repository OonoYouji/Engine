#include <LineDrawer.h>

#include <Engine.h>

#include <DxCommand.h>
#include <PipelineStateObjectManager.h>


/// ===================================================
/// インスタンス確保
/// ===================================================
LineDrawer* LineDrawer::GetInstance() {
	static LineDrawer instance;
	return &instance;
}


/// ===================================================
/// 初期化
/// ===================================================
void LineDrawer::Initialize() {
	dxCommon_ = DirectXCommon::GetInstance();
	CreateViewProjectionBuffer();
}


/// ===================================================
/// 終了処理
/// ===================================================
void LineDrawer::Finalize() {
	vertexBuffer_.Reset();
	viewProjectionBuffer_.Reset();
}


/// ===================================================
/// 描画前処理
/// ===================================================
void LineDrawer::PreDraw() {
	vertexUsedCount_ = 0;
	vertices_.clear();
}


/// ===================================================
/// 描画後処理
/// ===================================================
void LineDrawer::PostDraw() {

	ID3D12GraphicsCommandList* commandList = DxCommand::GetInstance()->GetList();
	PipelineStateObjectManager::GetInstance()->SetCommandList("Line", commandList);

	CreateVertexBuffer(vertices_.size());

	*viewProjectionData_ = Engine::GetCamera()->GetVpMatrix();

	commandList->IASetVertexBuffers(0, 1, &vbv_);
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);

	commandList->SetGraphicsRootConstantBufferView(0, viewProjectionBuffer_->GetGPUVirtualAddress());

	commandList->DrawInstanced(static_cast<UINT>(vertexUsedCount_), 1, 0, 0);

}


/// ===================================================
/// 描画
/// ===================================================
void LineDrawer::Draw(const Vec3f& v1, const Vec3f& v2, const Vec4f& color) {

	vertices_.push_back({ { v1.x, v1.y, v1.z, 1.0f },color });
	vertices_.push_back({ { v2.x, v2.y, v2.z, 1.0f },color });

	vertexUsedCount_ += 2;

}


/// ===================================================
/// 頂点バッファの生成
/// ===================================================
void LineDrawer::CreateVertexBuffer(size_t vertexCount) {

	vertexBuffer_ = dxCommon_->CreateBufferResource(sizeof(VertexData) * vertexCount);

	vbv_.BufferLocation = vertexBuffer_->GetGPUVirtualAddress(); //- リソースの先頭アドレス
	vbv_.SizeInBytes = static_cast<UINT>(sizeof(LineDrawer::VertexData) * vertexCount);		 //- 使用する頂点のデータ
	vbv_.StrideInBytes = static_cast<UINT>(sizeof(LineDrawer::VertexData));		 //- 使用する頂点の1つ分のデータ

	VertexData* vertexData = nullptr;
	vertexBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
	std::memcpy(vertexData, vertices_.data(), sizeof(VertexData) * vertexCount);

}


/// ===================================================
/// ビュープロジェクションバッファの生成
/// ===================================================
void LineDrawer::CreateViewProjectionBuffer() {

	///- resourceの生成
	viewProjectionBuffer_ = dxCommon_->CreateBufferResource(sizeof(Matrix4x4));

	///- データのmap
	viewProjectionBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&viewProjectionData_));

}

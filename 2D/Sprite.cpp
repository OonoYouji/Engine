#include "Sprite.h"

#include "DirectXCommon.h"
#include "Environment.h"
#include "ImGuiManager.h"

Sprite::Sprite() {}
Sprite::~Sprite() {
	materialResource_.Reset();
	transformationMatrixResource_.Reset();
	vertexResource_.Reset();
}



/// <summary>
/// 初期化
/// </summary>
void Sprite::Init() {
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();


	position = { 0.0f,0.0f };
	color = { 1.0f,1.0f,1.0f,1.0f };


	/// ---------------------------------
	/// ↓ 頂点リソースの初期化
	/// ---------------------------------
	vertexResource_.Attach(dxCommon->CreateBufferResource(sizeof(VertexData) * 6));

	///- 頂点バッファビューを作成
	///- リソースの先頭のアドレスから使う
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	///- 使用するリソースのサイズ; 頂点数分確保する
	vertexBufferView_.SizeInBytes = sizeof(VertexData) * 6;
	///- 1頂点あたりのサイズ
	vertexBufferView_.StrideInBytes = sizeof(VertexData);


	vertexData_ = nullptr;
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));

	///- 1枚目
	vertexData_[0].position = { 0.0f,360.0f,0.0f,1.0f };
	vertexData_[0].texcoord = { 0.0f,1.0f };
	vertexData_[1].position = { 0.0f,0.0f,0.0f,1.0f };
	vertexData_[1].texcoord = { 0.0f,0.0f };
	vertexData_[2].position = { 640.0f,360.0f,0.0f,1.0f };
	vertexData_[2].texcoord = { 1.0f,1.0f };

	///- 2枚目
	vertexData_[3].position = { 0.0f,0.0f,0.0f,1.0f };
	vertexData_[3].texcoord = { 0.0f,0.0f };
	vertexData_[4].position = { 640.0f,0.0f,0.0f,1.0f };
	vertexData_[4].texcoord = { 1.0f,0.0f };
	vertexData_[5].position = { 640.0f,360.0f,0.0f,1.0f };
	vertexData_[5].texcoord = { 1.0f,1.0f };

	///- 頂点座標を保存
	localVertex_.resize(6);
	for(uint32_t index = 0; index < 6; index++) {
		localVertex_[index] = vertexData_[index].position;
	}

	/// ---------------------------
	/// ↓ Transform周りを作る
	/// ---------------------------


	transformationMatrixResource_.Attach(dxCommon->CreateBufferResource(sizeof(Matrix4x4)));

	///- データを書き込む
	transformationMatrixData_ = nullptr;
	///- 書き込むためのアドレスを取得
	transformationMatrixResource_->Map(0, nullptr, reinterpret_cast<void**>(&transformationMatrixData_));
	*transformationMatrixData_ = Matrix4x4::MakeIdentity();

	transform_.Init();

	Matrix4x4 viewMatrixSprite = Matrix4x4::MakeIdentity();
	Matrix4x4 projectionMatrixSprite = Matrix4x4::MakeOrthographicMatrix(0.0f, 0.0f, float(kWindowSize.x), float(kWindowSize.y), 0.0f, 100.0f);
	Matrix4x4 wvpMatrixSprite = transform_.worldMatrix * (viewMatrixSprite * projectionMatrixSprite);
	*transformationMatrixData_ = wvpMatrixSprite;



	/// ---------------------------------
	/// ↓ マテリアルリソースの初期化
	/// ---------------------------------
	materialResource_.Attach(dxCommon->CreateBufferResource(sizeof(Vector4)));
	materialData_ = nullptr;
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
	*materialData_ = { 1.0f,1.0f,1.0f,1.0f };

}


/// <summary>
/// 描画
/// </summary>
void Sprite::Draw() {
	ID3D12GraphicsCommandList* commandList = DirectXCommon::GetInstance()->GetCommandList();

	///- マテリアルにデータをRGBAデータを書き込む
	*materialData_ = color;

	///- 頂点データに中心点のプラスして書き込む
	for(uint32_t index = 0; index < 6; index++) {
		Vec4f pos = { position.x, position.y, 0.0f, 0.0f };
		vertexData_[index].position = localVertex_[index] + pos;
	}

	commandList->IASetVertexBuffers(0, 1, &vertexBufferView_);
	commandList->SetGraphicsRootConstantBufferView(1, transformationMatrixResource_->GetGPUVirtualAddress());
	commandList->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());

	///- DrawCall
	commandList->DrawInstanced(6, 1, 0, 0);

}


void Sprite::ImGui([[maybe_unused]] const std::string& windowName) {
#ifdef _DEBUG
	
	ImGui::Begin(windowName.c_str());

	ImGui::DragFloat2("position", &position.x, 1.0f);
	ImGui::ColorEdit4("color", &color.x);

	ImGui::End();

#endif // DEBUG
}

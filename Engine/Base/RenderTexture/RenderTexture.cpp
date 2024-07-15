#define NOMINMAX
#include <RenderTexture.h>

#include <d3dx12.h>

#include <DirectXCommon.h>
#include <DxDescriptors.h>
#include <DxCommand.h>
#include <TextureManager.h>
#include <ImGuiManager.h>

#include <RenderTextureManager.h>



RenderTexture::RenderTexture() {
	RenderTextureManager::GetInstance()->AddRenderTexture(this);
}
RenderTexture::~RenderTexture() {}

/// ===================================================
/// スワップチェーンのバッファ用に初期化
/// ===================================================
void RenderTexture::InitializeSwapChain(UINT buffer, const Vector4& clearColor) {
	dxCommon_ = DirectXCommon::GetInstance();
	dxDescriptors_ = DxDescriptors::GetInstance();
	dxCommand_ = DxCommand::GetInstance();

	clearColor_ = clearColor;

	auto hr = dxCommon_->GetSwapChain()->GetBuffer(buffer, IID_PPV_ARGS(&targetBuffer_));
	assert(SUCCEEDED(hr));

	CreateRenderTargetBuffer();
}


/// ===================================================
/// offscreen用に初期化
/// ===================================================
void RenderTexture::Initialize(UINT width, UINT height, const Vector4& clearColor) {
	dxCommon_ = DirectXCommon::GetInstance();
	dxDescriptors_ = DxDescriptors::GetInstance();
	dxCommand_ = DxCommand::GetInstance();

	width_ = width;
	height_ = height;

	clearColor_ = clearColor;

	CreateOffScreenBuffer();
	CreateRenderTargetBuffer();
	CreateSRV();

}



void RenderTexture::InitializeUAV(UINT width, UINT height, const Vector4& clearColor) {

	dxCommon_ = DirectXCommon::GetInstance();
	dxDescriptors_ = DxDescriptors::GetInstance();
	dxCommand_ = DxCommand::GetInstance();

	width_ = width;
	height_ = height;

	clearColor_ = clearColor;

	//CreateBarrier(currentState_, D3D12_RESOURCE_STATE_GENERIC_READ);
	//CreateRenderTargetBuffer();
	CreateUAV();
	CreateSRV();
	CreateBarrier(currentState_, D3D12_RESOURCE_STATE_GENERIC_READ);
	//gpuHandle_ = std::move(texture_.handleGPU);
	
}


/// ===================================================
/// commandListにRTVを設定
/// ===================================================
void RenderTexture::SetRenderTarget() {
	ID3D12GraphicsCommandList* commandList = dxCommand_->GetList();

	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = dxDescriptors_->GetDSVHeap()->GetCPUDescriptorHandleForHeapStart();
	commandList->OMSetRenderTargets(1, &cpuHandle_, false, &dsvHandle);

}


/// ===================================================
/// RTVの色をクリア
/// ===================================================
void RenderTexture::Clear() {
	ID3D12GraphicsCommandList* commandList = dxCommand_->GetList();

	float clearColor[] = { clearColor_.x, clearColor_.y, clearColor_.z, clearColor_.w };
	commandList->ClearRenderTargetView(
		cpuHandle_, clearColor, 0, nullptr
	);

	dxCommon_->ClearDepthBuffer();
}


/// ===================================================
/// バリアーの生成
/// ===================================================
void RenderTexture::CreateBarrier(D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after) {

	if(currentState_ != before) {
		assert(false);
	}

	ID3D12GraphicsCommandList* commandList = dxCommand_->GetList();

	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = targetBuffer_.Get();
	barrier.Transition.StateBefore = before;
	barrier.Transition.StateAfter = after;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	currentState_ = after;

	commandList->ResourceBarrier(1, &barrier);

}


/// ===================================================
/// RTVの生成
/// ===================================================
void RenderTexture::CreateRenderTargetBuffer() {

	D3D12_RENDER_TARGET_VIEW_DESC desc{};
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

	cpuHandle_ = dxDescriptors_->GetCpuHandleRTV();
	dxDescriptors_->AddRtvUsedCount();

	dxCommon_->GetDevice()->CreateRenderTargetView(targetBuffer_.Get(), &desc, cpuHandle_);

}


/// ===================================================
/// オフスクリーン用のバッファに生成
/// ===================================================
void RenderTexture::CreateOffScreenBuffer() {

	cpuHandle_ = dxDescriptors_->GetCpuHandleRTV();
	dxDescriptors_->AddRtvUsedCount();

	// 適切なサイズやフォーマットでテクスチャリソースを作成する例
	CD3DX12_RESOURCE_DESC textureDesc(
		D3D12_RESOURCE_DIMENSION_TEXTURE2D, 0,
		width_, height_, 1, 1,
		DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, 1, 0,
		D3D12_TEXTURE_LAYOUT_UNKNOWN,
		D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET
	);

	CD3DX12_HEAP_PROPERTIES heapProperticse(D3D12_HEAP_TYPE_DEFAULT);
	D3D12_CLEAR_VALUE clearValue{};
	clearValue.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	clearValue.Color[0] = clearColor_.x;
	clearValue.Color[1] = clearColor_.y;
	clearValue.Color[2] = clearColor_.z;
	clearValue.Color[3] = clearColor_.w;

	// リソースの作成
	HRESULT hr = dxCommon_->GetDevice()->CreateCommittedResource(
		&heapProperticse,
		D3D12_HEAP_FLAG_NONE,
		&textureDesc,
		D3D12_RESOURCE_STATE_PRESENT,
		&clearValue,
		IID_PPV_ARGS(&targetBuffer_)
	);

	currentState_ = D3D12_RESOURCE_STATE_PRESENT;

}


/// ===================================================
/// SRVの生成
/// ===================================================
void RenderTexture::CreateSRV() {

	texture_.handleCPU = dxDescriptors_->GetCPUDescriptorHandle();
	texture_.handleGPU = dxDescriptors_->GetGPUDescriptorHandle();
	dxDescriptors_->AddSrvUsedCount();

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; // RTVと同じフォーマット
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.PlaneSlice = 0;
	srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;

	// SRVを作成する
	dxCommon_->GetDevice()->CreateShaderResourceView(targetBuffer_.Get(), &srvDesc, texture_.handleCPU);
}



/// ===================================================
/// UAVの生成
/// ===================================================
void RenderTexture::CreateUAV() {

	// オフスクリーンテクスチャの作成
	D3D12_RESOURCE_DESC textureDesc = {};
	textureDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	textureDesc.Alignment = 0;
	textureDesc.Width = 1280;
	textureDesc.Height = 720;
	textureDesc.DepthOrArraySize = 1;
	textureDesc.MipLevels = 1;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	textureDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

	D3D12_HEAP_PROPERTIES heapProperties = {};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;

	HRESULT hr = dxCommon_->GetDevice()->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&textureDesc,
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
		nullptr,
		IID_PPV_ARGS(&targetBuffer_)
	);

	currentState_ = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;


	assert(SUCCEEDED(hr));


	D3D12_UNORDERED_ACCESS_VIEW_DESC desc{};
	desc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.Texture2D.MipSlice = 0;


	cpuHandleUAV_ = dxDescriptors_->GetCpuHandleUAV();
	gpuHandle_ = dxDescriptors_->GetGpuHandleUAV();
	dxDescriptors_->AddUavUsedCount();

	dxCommon_->GetDevice()->CreateUnorderedAccessView(targetBuffer_.Get(), nullptr, &desc, cpuHandleUAV_);

}


/// ===================================================
/// textureのゲッタ
/// ===================================================
const TextureManager::Texture& RenderTexture::GetTexture() const {
	return texture_;
}


/// ===================================================
/// ImGui::Image()で作成したテクスチャの描画
/// ===================================================
void RenderTexture::ImGuiImage() {
#ifdef _DEBUG
	if(!ImGui::Begin(name_.c_str())) {
		ImGui::End();
		return;
	}


	ImVec2 max = ImGui::GetWindowContentRegionMax();
	ImVec2 min = ImGui::GetWindowContentRegionMin();
	ImVec2 winSize = {
		max.x - min.x,
		max.y - min.y
	};



	///- 大きさの調整
	ImVec2 texSize = winSize;
	if(texSize.x <= texSize.y) {
		///- x優先
		texSize.y = (texSize.x / 16.0f) * 9.0f;
	} else {
		///- y優先
		float x = (texSize.y / 9.0f) * 16.0f;
		if(x < texSize.x) {
			texSize.x = x;
		} else {
			texSize.y = (texSize.x / 16.0f) * 9.0f;
		}
	}

	ImVec2 texPos = {
		winSize.x * 0.5f,
		winSize.y * 0.5f
	};

	texPos.y -= texSize.y / 2.0f;
	texPos.x -= texSize.x / 2.0f;

	texPos.x = std::max(texPos.x, min.x);
	texPos.y = std::max(texPos.y, min.y);

	ImGui::SetCursorPos(texPos);

	ImTextureID id = ImTextureID(GetTexture().handleGPU.ptr);
	ImGui::Image(id, texSize);
	ImGui::End();
#endif // DEBUG
}


/// ===================================================
/// 名前のセット
/// ===================================================
void RenderTexture::SetName(const std::string& name) {
	name_ = name;
}

void RenderTexture::SetSceneLink(IScene* link) {
	sceneLink_ = link;
}

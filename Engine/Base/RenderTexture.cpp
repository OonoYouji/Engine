#include "RenderTexture.h"

#include <d3dx12.h>

#include <DirectXCommon.h>
#include <DxDescriptors.h>
#include <DxCommand.h>
#include <TextureManager.h>



RenderTexture::RenderTexture() {}
RenderTexture::~RenderTexture() {}


void RenderTexture::Initialize(UINT buffer) {
	dxCommon_ = DirectXCommon::GetInstance();
	dxDescriptors_ = DxDescriptors::GetInstance();
	dxCommand_ = DxCommand::GetInstance();

	auto hr = dxCommon_->GetSwapChain()->GetBuffer(buffer, IID_PPV_ARGS(&targetBuffer_));
	assert(SUCCEEDED(hr));

	CreateRenderTargetBuffer();
}

void RenderTexture::InitializeOffScreen(UINT width, UINT height) {
	dxCommon_ = DirectXCommon::GetInstance();
	dxDescriptors_ = DxDescriptors::GetInstance();
	dxCommand_ = DxCommand::GetInstance();

	width_ = width;
	height_ = height;

	CreateOffScreenBuffer();
	CreateRenderTargetBuffer();
	CreateSRV();
}


void RenderTexture::SetRenderTarget() {
	ID3D12GraphicsCommandList* commandList = dxCommand_->GetList();

	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = dxDescriptors_->GetDSVHeap()->GetCPUDescriptorHandleForHeapStart();
	commandList->OMSetRenderTargets(1, &cpuHandle_, false, &dsvHandle);

}

void RenderTexture::Clear(const Vec4f& color) {
	ID3D12GraphicsCommandList* commandList = dxCommand_->GetList();

	float clearColor[] = { color.x, color.y, color.z, color.w };
	commandList->ClearRenderTargetView(
		cpuHandle_, clearColor, 0, nullptr
	);
}

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


void RenderTexture::CreateRenderTargetBuffer() {

	D3D12_RENDER_TARGET_VIEW_DESC desc{};
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

	cpuHandle_ = dxDescriptors_->GetCpuHandleRTV();
	dxDescriptors_->AddRtvUsedCount();

	dxCommon_->GetDevice()->CreateRenderTargetView(targetBuffer_.Get(), &desc, cpuHandle_);

}

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
	clearValue.Color[0] = 0.0f;
	clearValue.Color[1] = 0.0f;
	clearValue.Color[2] = 0.0f;
	clearValue.Color[3] = 1.0f;

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



void RenderTexture::CopyBuffer() {

	//texture_.resource = targetBuffer_;

}

const TextureManager::Texture& RenderTexture::GetTexture() const {
	return texture_;
}

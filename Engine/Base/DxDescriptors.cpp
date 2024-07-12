#include "DxDescriptors.h"

#include <cassert>

#include "DirectXCommon.h"


/// <summary>
/// インスタンス確保
/// </summary>
DxDescriptors* DxDescriptors::GetInstance() {
	static DxDescriptors instance;
	return &instance;
}


/// <summary>
/// 初期化
/// </summary>
void DxDescriptors::Initialize() {
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	///- RTV Initialize
	rtvDescriptorHeap_ = nullptr;
	rtvDescriptorHeap_ = CreateDescriptorHeap(dxCommon->GetDevice(), D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 12, false);
	assert(rtvDescriptorHeap_);

	///- SRV Initialize
	srvDescriptorHeap_ = nullptr;
	srvDescriptorHeap_ = CreateDescriptorHeap(dxCommon->GetDevice(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 128, true);
	assert(srvDescriptorHeap_);

	///- DSV Initialize
	dsvDescriptorHeap_ = nullptr;
	dsvDescriptorHeap_ = CreateDescriptorHeap(dxCommon->GetDevice(), D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1, false);
	assert(dsvDescriptorHeap_);



	///- descriptorSizeの取得
	descriptorRTV_ = dxCommon->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	descriptorSRV_ = dxCommon->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	descriptorDSV_ = dxCommon->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);


}


/// <summary>
/// 終了処理
/// </summary>
void DxDescriptors::Finalize() {

	///- 解放処理
	dsvDescriptorHeap_.Reset();
	srvDescriptorHeap_.Reset();
	rtvDescriptorHeap_.Reset();

}


/// <summary>
/// DescriptorHeap作成関数
/// </summary>
ComPtr<ID3D12DescriptorHeap> DxDescriptors::CreateDescriptorHeap(ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible) {
	ComPtr<ID3D12DescriptorHeap> heap;
	D3D12_DESCRIPTOR_HEAP_DESC desc{};
	desc.Type = heapType;
	desc.NumDescriptors = numDescriptors;
	desc.Flags = shaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	HRESULT result = device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&heap));
	assert(SUCCEEDED(result));

	return heap;
}



/// <summary>
/// DescriptorHandleの取得
/// </summary>
D3D12_CPU_DESCRIPTOR_HANDLE DxDescriptors::GetCPUDescriptorHandle(ID3D12DescriptorHeap* descriptorHeap, uint32_t descriptorSize, uint32_t index) {
	D3D12_CPU_DESCRIPTOR_HANDLE handleCPU = descriptorHeap->GetCPUDescriptorHandleForHeapStart();
	handleCPU.ptr += (descriptorSize * index);
	return handleCPU;
}


/// <summary>
/// DescriptorHandleの取得
/// </summary>
D3D12_GPU_DESCRIPTOR_HANDLE DxDescriptors::GetGPUDescriptorHandle(ID3D12DescriptorHeap* descriptorHeap, uint32_t descriptorSize, uint32_t index) {
	D3D12_GPU_DESCRIPTOR_HANDLE handleGPU = descriptorHeap->GetGPUDescriptorHandleForHeapStart();
	handleGPU.ptr += (descriptorSize * index);
	return handleGPU;
}

D3D12_CPU_DESCRIPTOR_HANDLE DxDescriptors::GetCPUDescriptorHandle() {
	return GetCPUDescriptorHandle(srvDescriptorHeap_.Get(), descriptorSRV_, srvUsedCount_);
}

D3D12_GPU_DESCRIPTOR_HANDLE DxDescriptors::GetGPUDescriptorHandle() {
	return GetGPUDescriptorHandle(srvDescriptorHeap_.Get(), descriptorSRV_, srvUsedCount_);
}

D3D12_CPU_DESCRIPTOR_HANDLE DxDescriptors::GetCpuHandleRTV() {
	return GetCPUDescriptorHandle(rtvDescriptorHeap_.Get(), descriptorRTV_, rtvUsedCount_);
}

D3D12_GPU_DESCRIPTOR_HANDLE DxDescriptors::GetGpuHandleRTV() {
	return GetGPUDescriptorHandle(rtvDescriptorHeap_.Get(), descriptorRTV_, rtvUsedCount_);
}

void DxDescriptors::AddSrvUsedCount() {
	srvUsedCount_++;
}

void DxDescriptors::AddRtvUsedCount() {
	rtvUsedCount_++;
}

void DxDescriptors::SetCommandListSrvHeap(ID3D12GraphicsCommandList* commandList) {
	//// デスクリプタヒープの設定
	ID3D12DescriptorHeap* descriptorHeaps[] = { GetSRVHeap() };
	commandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

}

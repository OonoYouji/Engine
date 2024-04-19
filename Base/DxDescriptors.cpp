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
	rtvDescriptorHeap_ = CreateDescriptorHeap(dxCommon->GetDevice(), D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 2, false);
	assert(rtvDescriptorHeap_);

	///- SRV Initialize
	srvDescriptorHeap_ = nullptr;
	srvDescriptorHeap_ = CreateDescriptorHeap(dxCommon->GetDevice(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 128, true);
	assert(srvDescriptorHeap_);

	///- DSV Initialize
	dsvDescriptorHeap_ = nullptr;
	dsvDescriptorHeap_ = CreateDescriptorHeap(dxCommon->GetDevice(), D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1, false);
	assert(dsvDescriptorHeap_);

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

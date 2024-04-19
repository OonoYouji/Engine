#pragma once

#include <d3d12.h>
#include <wrl/client.h>

using namespace Microsoft::WRL;


/// <summary>
/// デスクリプターを集めたクラス
/// </summary>
class DxDescriptors final {
public:

	/// <summary>
	/// インスタンス確保関数
	/// </summary>
	/// <returns> DxDescriptorsクラスのインスタンス </returns>
	static DxDescriptors* GetInstance();



	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 終了処理
	/// </summary>
	void Finalize();



	/// <summary>
	/// rtvDescriptorHeapのgetter
	/// </summary>
	ID3D12DescriptorHeap* GetRTVHeap() const { return rtvDescriptorHeap_.Get(); }

	/// <summary>
	/// srvDescriptorHeapのgetter
	/// </summary>
	ID3D12DescriptorHeap* GetSRVHeap() const { return srvDescriptorHeap_.Get(); }

	/// <summary>
	/// dsvDescriptorHeapのgetter
	/// </summary>
	ID3D12DescriptorHeap* GetDSVHeap() const { return dsvDescriptorHeap_.Get(); }


private: ///- member obejct

	ComPtr<ID3D12DescriptorHeap> rtvDescriptorHeap_;
	ComPtr<ID3D12DescriptorHeap> srvDescriptorHeap_;
	ComPtr<ID3D12DescriptorHeap> dsvDescriptorHeap_;

private: ///- member method

	/// <summary>
	/// DescriptorHeapの作成関数
	/// </summary>
	/// <param name="device"></param>
	/// <param name="heapType"></param>
	/// <param name="numDescriptors"></param>
	/// <param name="shaderVisible"></param>
	/// <returns></returns>
	ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeap(ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible);



	DxDescriptors() = default;
	~DxDescriptors() = default;

	DxDescriptors& operator= (const DxDescriptors&) = delete;
	DxDescriptors(const DxDescriptors&) = delete;
	DxDescriptors(DxDescriptors&&) = delete;

};

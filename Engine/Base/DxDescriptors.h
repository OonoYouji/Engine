#pragma once

#include <d3d12.h>
#include <wrl/client.h>

#include <cmath>

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


	/// <summary>
	/// DescriptorHandleを取得する [ CPU ver ]
	/// </summary>
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(ID3D12DescriptorHeap* descriptorHeap, uint32_t descriptorSize, uint32_t index);

	/// <summary>
	/// DescriptorHandleを取得する [ GPU ver ]
	/// </summary>
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(ID3D12DescriptorHeap* descriptorHeap, uint32_t descriptorSize, uint32_t index);

	
	/// <summary>
	/// DescriptorHandleを取得する [ CPU ver ]
	/// </summary>
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle();

	/// <summary>
	/// DescriptorHandleを取得する [ GPU ver ]
	/// </summary>
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle();


	D3D12_CPU_DESCRIPTOR_HANDLE GetCpuHandleRTV();

	D3D12_GPU_DESCRIPTOR_HANDLE GetGpuHandleRTV();


	void AddSrvUsedCount();
	void AddRtvUsedCount();

	void SetCommandListSrvHeap(ID3D12GraphicsCommandList* commandList);


	uint32_t GetSRVSize() const { return descriptorSRV_; }
	uint32_t GetRTVSize() const { return descriptorRTV_; }
	uint32_t GetDSVSize() const { return descriptorDSV_; }


private: ///- member obejct

	ComPtr<ID3D12DescriptorHeap> rtvDescriptorHeap_;
	ComPtr<ID3D12DescriptorHeap> srvDescriptorHeap_;
	ComPtr<ID3D12DescriptorHeap> dsvDescriptorHeap_;

	uint32_t descriptorSRV_;
	uint32_t descriptorRTV_;
	uint32_t descriptorDSV_;

	uint32_t srvUsedCount_ = 0;
	uint32_t rtvUsedCount_ = 0;
	uint32_t uavUsedCount_ = 0;

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

#pragma once

#include <d3d12.h>
#include <wrl/client.h>


using namespace Microsoft::WRL;


/// <summary>
/// DirectX12のCommand
/// </summary>
class DxCommand final {
public:

	/// <summary>
	/// インスタンス確保用関数
	/// </summary>
	/// <returns> DxCommandのインスタンス </returns>
	static DxCommand* GetInstance();



	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(ID3D12Device* device);

	/// <summary>
	/// 終了処理
	/// </summary>
	void Finalize();

	/// <summary>
	/// バリアを貼る
	/// </summary>
	/// <param name="bbIndex"> 現在のバックバッファのindex </param>
	/// <param name="stateBefore"> 現在のstate </param>
	/// <param name="stateAfter"> 変更後のstate </param>
	void CreateBarrier(UINT bbIndex, D3D12_RESOURCE_STATES stateBefore, D3D12_RESOURCE_STATES stateAfter);

	/// <summary>
	/// 次フレームのためにListをリセットする
	/// </summary>
	void ResetCommandList();


	/// <summary>
	/// CommandQueueのgetter
	/// </summary
	ID3D12CommandQueue* GetQueue() const { return commandQueue_.Get(); }

	/// <summary>
	/// CommandAllocatorのgetter
	/// </summary
	ID3D12CommandAllocator* GetAllocator() const { return commandAllocator_.Get(); }

	/// <summary>
	/// CommandListのgetter
	/// </summary
	ID3D12GraphicsCommandList* GetList() const { return commandList_.Get(); }


	void Close();

private:

	ComPtr<ID3D12CommandQueue> commandQueue_;
	ComPtr<ID3D12CommandAllocator> commandAllocator_;
	ComPtr<ID3D12GraphicsCommandList> commandList_;


private:


	DxCommand() = default;
	~DxCommand() = default;

	DxCommand& operator= (const DxCommand&) = delete;
	DxCommand(const DxCommand&) = delete;
	DxCommand(DxCommand&&) = delete;

};
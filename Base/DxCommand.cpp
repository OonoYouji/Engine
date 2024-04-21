#include "DxCommand.h"

#include <cassert>
#include "DirectXCommon.h"


/// <summary>
/// インスタンス確保
/// </summary>
DxCommand* DxCommand::GetInstance() {
	static DxCommand instance;
	return &instance;
}



/// <summary>
/// 初期化
/// </summary>
void DxCommand::Initialize(ID3D12Device* device) {
	HRESULT result = S_FALSE;

	/// ---------------------------
	/// ↓ CommandQueueを生成
	/// ---------------------------
	commandQueue_ = nullptr;
	D3D12_COMMAND_QUEUE_DESC desc{};

	///- 生成; 生成できたか確認
	result = device->CreateCommandQueue(&desc, IID_PPV_ARGS(&commandQueue_));
	assert(SUCCEEDED(result));



	/// ---------------------------
	/// ↓ CommandAllocatorを生成
	/// ---------------------------
	commandAllocator_ = nullptr;

	///- 生成; 生成できたか確認
	result = device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator_));
	assert(SUCCEEDED(result));



	/// ---------------------------
	/// ↓ CommandListを生成
	/// ---------------------------
	commandList_ = nullptr;

	///- 生成; 生成できたか確認
	result = device->CreateCommandList(
		0,
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		commandAllocator_.Get(),
		nullptr,
		IID_PPV_ARGS(&commandList_)
	);
	assert(SUCCEEDED(result));


}



/// <summary>
/// 終了処理
/// </summary>
void DxCommand::Finalize() {

	///- 解放処理
	commandList_.Reset();
	commandAllocator_.Reset();
	commandQueue_.Reset();

}



/// <summary>
/// バリアーを貼る
/// </summary>
void DxCommand::CreateBarrier(UINT bbIndex, D3D12_RESOURCE_STATES stateBefore, D3D12_RESOURCE_STATES stateAfter) {

	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = DirectXCommon::GetInstance()->GetSwapChainResource()[bbIndex].Get();
	barrier.Transition.StateBefore = stateBefore;
	barrier.Transition.StateAfter = stateAfter;
	commandList_->ResourceBarrier(1, &barrier);

}



/// <summary>
/// CommandListのリセット
/// </summary>
void DxCommand::ResetCommandList() {
	HRESULT result = S_FALSE;

	///- 次のフレーム用にコマンドリストを準備
	result = commandAllocator_->Reset();
	assert(SUCCEEDED(result));
	result = commandList_->Reset(DxCommand::GetInstance()->GetAllocator(), nullptr);
	assert(SUCCEEDED(result));

}

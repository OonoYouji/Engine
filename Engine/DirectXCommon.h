#pragma once

#include <d3d12.h>
#include <dxgi1_6.h>
#include <cassert>

class DirectXCommon final {
private:

	DirectXCommon() = default;
	~DirectXCommon() = default;

private:

	IDXGIFactory7* dxgiFactory_ = nullptr;
	ID3D12Device* device_ = nullptr;

public:

	static DirectXCommon* GetInstance();

	void InitializeDXGIDevice();

private:

	/// 代入演算子、コピーコンストラクタの禁止
	DirectXCommon(const DirectXCommon& other) = delete;
	DirectXCommon(DirectXCommon&& other) = delete;
	DirectXCommon& operator= (const DirectXCommon& other) = delete;

};
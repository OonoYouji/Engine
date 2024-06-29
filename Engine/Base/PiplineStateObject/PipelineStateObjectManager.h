#pragma once

#include <d3d12.h>

#include <map>
#include <string>
#include <memory>
#include <cmath>

class PipelineStateObject;
class ShaderCompile;


/// <summary>
/// PipelineStateObjectをまとめるクラス
/// </summary>
class PipelineStateObjectManager {
public:

	/// <summary>
	/// インスタンス確保関数
	/// </summary>
	/// <returns></returns>
	static PipelineStateObjectManager* GetInstance();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(ID3D12Device* device);

	/// <summary>
	/// 終了処理
	/// </summary>
	void Finalize();

	/// <summary>
	/// コマンドリストへpsoをセットする
	/// </summary>
	/// <param name="index"></param>
	/// <param name="commandList"></param>
	void SetCommandList(const std::string& key, ID3D12GraphicsCommandList* commandList);
	
	/// <summary>
	/// コマンドリストへpsoをセットする
	/// </summary>
	/// <param name="index"></param>
	/// <param name="commandList"></param>
	void SetComputeCommandList(const std::string& key, ID3D12GraphicsCommandList* commandList);

private:

	std::unique_ptr<ShaderCompile> shaderCompile_;
	std::map<std::string, std::unique_ptr<PipelineStateObject>> pipelineStateObjects_;

private:

	PipelineStateObjectManager() = default;
	~PipelineStateObjectManager() = default;

	PipelineStateObjectManager& operator=(const PipelineStateObjectManager&) = delete;
	PipelineStateObjectManager(const PipelineStateObjectManager&) = delete;
	PipelineStateObjectManager(PipelineStateObjectManager&&) = delete;

};
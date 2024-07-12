#pragma once

#include <wrl/client.h>
#include <d3d12.h>
#include <dxcapi.h>

#include <string>
#include <vector>
#include <memory>

using namespace Microsoft::WRL;

class ShaderCompile;
class ShaderBlob;

/// <summary>
/// PiplineStateクラス
/// </summary>
class PipelineStateObject {
public:

	PipelineStateObject();
	~PipelineStateObject();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="device"></param>
	/// <param name="shaderBlob"></param>
	void Initialize(ID3D12Device* device, ShaderBlob* shaderBlob);


	void InitializeComputeShaderVer(ID3D12Device* device, ShaderBlob* shaderBlob);


	/// <summary>
	/// 新しいInputElementの設定
	/// </summary>
	/// <param name="semanticName"></param>
	/// <param name="semanticIndex"></param>
	/// <param name="format"></param>
	void SetInputElement(const std::string& semanticName, uint32_t semanticIndex, DXGI_FORMAT format);

	/// <summary>
	/// 新しいRootParameterの設定
	/// </summary>
	/// <param name="parameterType"></param>
	/// <param name="shaderVisibilty"></param>
	/// <param name="shaderRegister"></param>
	void SetRootParameter(D3D12_ROOT_PARAMETER_TYPE parameterType, D3D12_SHADER_VISIBILITY shaderVisibilty, uint32_t shaderRegister);

	/// <summary>
	/// 
	/// </summary>
	/// <param name="shaderVisibilty"></param>
	/// <param name="shaderRegister"></param>
	void SetCBV(D3D12_SHADER_VISIBILITY shaderVisibilty, uint32_t shaderRegister);

	/// <summary>
	/// 
	/// </summary>
	/// <param name="shaderVisibilty"></param>
	/// <param name="shaderRegister"></param>
	void SetUAV(D3D12_SHADER_VISIBILITY shaderVisibilty, uint32_t shaderRegister);

	/// <summary>
	/// 新しいRootParameterの設定 (DescriptorRangeを設定するver)
	/// </summary>
	/// <param name="parameterType"></param>
	/// <param name="shaderVisibilty"></param>
	/// <param name="descriptorIndex"></param>
	void SetRootParameterDescriptorTable(D3D12_ROOT_PARAMETER_TYPE parameterType, D3D12_SHADER_VISIBILITY shaderVisibilty, uint32_t descriptorIndex);

	/// <summary>
	/// 
	/// </summary>
	/// <param name="shaderVisibilty"></param>
	/// <param name="descriptorIndex"></param>
	void SetDescriptorTable(D3D12_SHADER_VISIBILITY shaderVisibilty, uint32_t descriptorIndex);

	/// <summary>
	/// 新しいDescriptorRangeの設定
	/// </summary>
	/// <param name="baseShaderRegister"></param>
	/// <param name="numDescriptor"></param>
	/// <param name="rangeType"></param>
	void SetDescriptorRange(uint32_t baseShaderRegister, uint32_t numDescriptor, D3D12_DESCRIPTOR_RANGE_TYPE rangeType);

	/// <summary>
	/// 新しいStaticSamplerの設定
	/// </summary>
	/// <param name="shaderRegister"></param>
	/// <param name="shaderVisibility"></param>
	void SetStaticSampler(uint32_t shaderRegister, D3D12_SHADER_VISIBILITY shaderVisibility);

	/// <summary>
	/// CommandListにRootSignatureとPipelineStateを設定する
	/// </summary>
	/// <param name="commandList"></param>
	void SetCommandList(ID3D12GraphicsCommandList* commandList);

	/// <summary>
	/// 
	/// </summary>
	/// <param name="commandList"></param>
	void SetComputeCommandList(ID3D12GraphicsCommandList* commandList);

	void SetTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE topolpgyType);

private:

	ComPtr<ID3D12RootSignature> rootSignature_;
	ComPtr<ID3D12PipelineState> pipelineState_;

	std::vector<D3D12_INPUT_ELEMENT_DESC> inputElements_;
	std::vector<D3D12_ROOT_PARAMETER> rootParameters_;
	std::vector<D3D12_DESCRIPTOR_RANGE> descriptorRanges_;
	std::vector<D3D12_STATIC_SAMPLER_DESC> staticSamplers_;

	std::vector<std::string> semanticNames_;
	D3D12_PRIMITIVE_TOPOLOGY_TYPE topolpgyType_ = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	/// <summary>
	/// RootSignatureの生成
	/// </summary>
	void CreateRootSignature(ID3D12Device* device);

	/// <summary>
	/// ID3D12PipelineStateの生成
	/// </summary>
	/// <param name="device"></param>
	/// <param name="shaderBlob"></param>
	void CreatePipelineState(ID3D12Device* device, ShaderBlob* shaderBlob);


	/// <summary>
	/// ID3D12PipelineStateの生成 ComputeShader ver
	/// </summary>
	/// <param name="device"></param>
	/// <param name="shaderBlob"></param>
	void CreatePipelineStateComputeVer(ID3D12Device* device, ShaderBlob* shaderBlob);

};


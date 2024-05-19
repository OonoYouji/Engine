#include "PipelineStateObject.h"

#include <cassert>

#include <Engine.h>
#include <ShaderBlob.h>


PipelineStateObject::PipelineStateObject() {}
PipelineStateObject::~PipelineStateObject() {}



void PipelineStateObject::Initialize(ID3D12Device* device, ShaderBlob* shaderBlob) {

	CreateRootSignature(device);
	CreatePipelineState(device, shaderBlob);
}

void PipelineStateObject::SetInputElement(const std::string& semanticName, uint32_t semanticIndex, DXGI_FORMAT format) {
	semanticNames_.push_back(semanticName);

	///- 頂点シェーダのInputLayout例
	/// struct VertexShaderInput {
	///		float4 position : POSITION0;
	/// };

	D3D12_INPUT_ELEMENT_DESC desc{};
	desc.SemanticName = semanticNames_.rbegin()->c_str();		//- "POSITION"
	desc.SemanticIndex = semanticIndex;							//- "0"
	desc.Format = format;										//- "DXGI_FORMAT_R32G32B32A32_FLOAT"
	desc.AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;		//- 

	inputElements_.push_back(desc);
}


void PipelineStateObject::SetRootParameter(D3D12_ROOT_PARAMETER_TYPE parameterType, D3D12_SHADER_VISIBILITY shaderVisibilty, uint32_t shaderRegister) {
	D3D12_ROOT_PARAMETER rootParameter{};
	rootParameter.ParameterType = parameterType;				//- 使用するRegisterの種類
	rootParameter.ShaderVisibility = shaderVisibilty;			//- 使用するShader
	rootParameter.Descriptor.ShaderRegister = shaderRegister;	//- バインドするRegister番号

	rootParameters_.push_back(rootParameter);

}


void PipelineStateObject::SetRootParameterDescriptorTable(D3D12_ROOT_PARAMETER_TYPE parameterType, D3D12_SHADER_VISIBILITY shaderVisibilty, uint32_t descriptorIndex) {
	D3D12_ROOT_PARAMETER rootParameter{};
	rootParameter.ParameterType = parameterType;															//- 使用するRegisterの種類
	rootParameter.ShaderVisibility = shaderVisibilty;														//- 使用するShader
	rootParameter.DescriptorTable.pDescriptorRanges = &descriptorRanges_[descriptorIndex];					//- Tableの中身の配列
	rootParameter.DescriptorTable.NumDescriptorRanges = descriptorRanges_[descriptorIndex].NumDescriptors;	//- Tableで使用する数

	rootParameters_.push_back(rootParameter);
}


void PipelineStateObject::SetDescriptorRange(uint32_t baseShaderRegister, uint32_t numDescriptor, D3D12_DESCRIPTOR_RANGE_TYPE rangeType) {
	D3D12_DESCRIPTOR_RANGE descriptorRange{};
	descriptorRange.BaseShaderRegister = baseShaderRegister;									//- 開始する番号
	descriptorRange.NumDescriptors = numDescriptor;												//- 使用するTextureの数
	descriptorRange.RangeType = rangeType;														//- 使用するRangeType
	descriptorRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;	//- 

	descriptorRanges_.push_back(descriptorRange);
}


void PipelineStateObject::SetStaticSampler(uint32_t shaderRegister, D3D12_SHADER_VISIBILITY shaderVisibility) {
	D3D12_STATIC_SAMPLER_DESC staticSampler{};
	staticSampler.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;		//- バイリニアフィルタ
	staticSampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;	//- 0~1の範囲外をリピート
	staticSampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSampler.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;	//- 比較しない
	staticSampler.MaxLOD = D3D12_FLOAT32_MAX;					//- ありったけのMipMapを使う
	staticSampler.ShaderRegister = shaderRegister;				//- 使用するRegister番号
	staticSampler.ShaderVisibility = shaderVisibility;			//- 使用するShader

	staticSamplers_.push_back(staticSampler);
}


void PipelineStateObject::SetCommandList(ID3D12GraphicsCommandList* commandList) {
	commandList->SetGraphicsRootSignature(rootSignature_.Get());
	commandList->SetPipelineState(pipelineState_.Get());
}


void PipelineStateObject::CreateRootSignature(ID3D12Device* device) {
	HRESULT result = S_FALSE;
	ComPtr<ID3DBlob> signatureBlob;
	ComPtr<ID3DBlob> errorBlob;

	D3D12_ROOT_SIGNATURE_DESC desc{};
	desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	desc.pParameters = rootParameters_.data();							//- RootParameter配列へのポインタ
	desc.NumParameters = static_cast<UINT>(rootParameters_.size());		//- RootParameterの配列の長さ
	desc.pStaticSamplers = staticSamplers_.data();						//- StaticSampler配列へのポインタ
	desc.NumStaticSamplers = static_cast<UINT>(staticSamplers_.size());	//- StaticSamplerの配列の長さ

	///- シリアライズしてバイナリ
	result = D3D12SerializeRootSignature(
		&desc, D3D_ROOT_SIGNATURE_VERSION_1,
		&signatureBlob, &errorBlob);
	if(FAILED(result)) {
		Engine::ConsolePrint(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(false);
	}

	///- バイナリを元に生成
	result = device->CreateRootSignature(
		0, signatureBlob->GetBufferPointer(),
		signatureBlob->GetBufferSize(),
		IID_PPV_ARGS(&rootSignature_)
	);
	assert(SUCCEEDED(result));

}


void PipelineStateObject::CreatePipelineState(ID3D12Device* device, ShaderBlob* shaderBlob) {

	/// --------------------------------
	/// ↓ DepthStencilの設定
	/// --------------------------------
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	depthStencilDesc.DepthEnable = true;							//- Depth機能の有効化or無効化
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;	//- 書き込み設定
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;	//- 比較関数の設定


	/// --------------------------------
	/// ↓ InputLayoutDescの設定
	/// --------------------------------
	for(uint32_t index = 0; index < inputElements_.size(); ++index) {
		inputElements_[index].SemanticName = semanticNames_[index].c_str();
	}
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc;
	inputLayoutDesc.pInputElementDescs = inputElements_.data();
	inputLayoutDesc.NumElements = static_cast<UINT>(inputElements_.size());


	/// --------------------------------
	/// ↓ BlendStateの設定
	/// --------------------------------
	D3D12_BLEND_DESC blendDesc{};
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;


	/// --------------------------------
	/// ↓ RasterRizerの設定
	/// --------------------------------
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;		//- 裏面の表示設定
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;	//- 三角形の中を塗りつぶすかどうか


	/// --------------------------------
	/// ↓ PSOの設定
	/// --------------------------------
	D3D12_GRAPHICS_PIPELINE_STATE_DESC desc{};
	desc.pRootSignature = rootSignature_.Get();	//- RootSignature
	desc.InputLayout = inputLayoutDesc;			//- InputLayout

	///- Shaderの設定
	desc.VS = {
		shaderBlob->GetVS()->GetBufferPointer(),
		shaderBlob->GetVS()->GetBufferSize()
	};
	desc.PS = {
		shaderBlob->GetPS()->GetBufferPointer(),
		shaderBlob->GetPS()->GetBufferSize()
	};


	desc.BlendState = blendDesc;			//- BlendState
	desc.RasterizerState = rasterizerDesc;  //- RasterizerState

	desc.NumRenderTargets = 1;												//- 
	desc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;					//- 
	desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;	//- 使用する形状Type
	desc.SampleDesc.Count = 1;												//- 
	desc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;							//- 
	desc.DepthStencilState = depthStencilDesc;								//- 
	desc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;							//- 

	///- 生成
	HRESULT result = device->CreateGraphicsPipelineState(
		&desc, IID_PPV_ARGS(&pipelineState_)
	);
	assert(SUCCEEDED(result));

}


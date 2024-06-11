#include <PipelineStateObjectManager.h>

#include <PipelineStateObject.h>
#include <ShaderCompile.h>
#include <ShaderBlob.h>


PipelineStateObjectManager* PipelineStateObjectManager::GetInstance() {
	static PipelineStateObjectManager instance;
	return &instance;
}


void PipelineStateObjectManager::Initialize(ID3D12Device* device) {

	shaderCompile_ = std::make_unique<ShaderCompile>();
	shaderCompile_->Initialize();


	/// ----------------------------------------------
	/// ↓ Object3d Shader
	/// ----------------------------------------------
	std::unique_ptr<ShaderBlob> shaderBlob = std::make_unique<ShaderBlob>();
	shaderBlob->Initialize(
		shaderCompile_.get(),
		L"./Shader/Object3D/Object3d.VS.hlsl", L"vs_6_0",
		L"./Shader/Object3D/Object3d.PS.hlsl", L"ps_6_0"
	);

	pipelineStateObjects_.push_back(std::make_unique<PipelineStateObject>());

	pipelineStateObjects_.back()->SetInputElement("POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);
	pipelineStateObjects_.back()->SetInputElement("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT);
	pipelineStateObjects_.back()->SetInputElement("NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT);

	pipelineStateObjects_.back()->SetDescriptorRange(0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV);

	pipelineStateObjects_.back()->SetRootParameter(D3D12_ROOT_PARAMETER_TYPE_CBV, D3D12_SHADER_VISIBILITY_PIXEL, 0);
	pipelineStateObjects_.back()->SetRootParameter(D3D12_ROOT_PARAMETER_TYPE_CBV, D3D12_SHADER_VISIBILITY_VERTEX, 0);
	pipelineStateObjects_.back()->SetRootParameterDescriptorTable(D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE, D3D12_SHADER_VISIBILITY_PIXEL, 0);
	pipelineStateObjects_.back()->SetRootParameter(D3D12_ROOT_PARAMETER_TYPE_CBV, D3D12_SHADER_VISIBILITY_PIXEL, 1);

	pipelineStateObjects_.back()->SetStaticSampler(0, D3D12_SHADER_VISIBILITY_PIXEL);

	pipelineStateObjects_.back()->Initialize(device, shaderBlob.get());



	/// ----------------------------------------------
	/// ↓ Terrain Shader
	/// ----------------------------------------------

	shaderBlob.reset(new ShaderBlob());
	shaderBlob->Initialize(
		shaderCompile_.get(),
		L"./Shader/Terrain/Terrain.VS.hlsl", L"vs_6_0",
		L"./Shader/Terrain/Terrain.PS.hlsl", L"ps_6_0"
	);

	pipelineStateObjects_.push_back(std::make_unique<PipelineStateObject>());

	pipelineStateObjects_.back()->SetInputElement("POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);
	pipelineStateObjects_.back()->SetInputElement("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT);
	pipelineStateObjects_.back()->SetInputElement("NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT);

	pipelineStateObjects_.back()->SetDescriptorRange(0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV);
	pipelineStateObjects_.back()->SetDescriptorRange(0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV);
	pipelineStateObjects_.back()->SetDescriptorRange(3, 1, D3D12_DESCRIPTOR_RANGE_TYPE_UAV);

	pipelineStateObjects_.back()->SetRootParameter(D3D12_ROOT_PARAMETER_TYPE_CBV, D3D12_SHADER_VISIBILITY_PIXEL, 0);
	pipelineStateObjects_.back()->SetRootParameter(D3D12_ROOT_PARAMETER_TYPE_CBV, D3D12_SHADER_VISIBILITY_VERTEX, 0);
	pipelineStateObjects_.back()->SetRootParameterDescriptorTable(D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE, D3D12_SHADER_VISIBILITY_PIXEL, 0);
	pipelineStateObjects_.back()->SetRootParameterDescriptorTable(D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE, D3D12_SHADER_VISIBILITY_VERTEX, 1);
	pipelineStateObjects_.back()->SetRootParameterDescriptorTable(D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE, D3D12_SHADER_VISIBILITY_ALL, 2);
	pipelineStateObjects_.back()->SetRootParameter(D3D12_ROOT_PARAMETER_TYPE_CBV, D3D12_SHADER_VISIBILITY_PIXEL, 1);
	pipelineStateObjects_.back()->SetRootParameter(D3D12_ROOT_PARAMETER_TYPE_CBV, D3D12_SHADER_VISIBILITY_ALL, 3);

	pipelineStateObjects_.back()->SetStaticSampler(0, D3D12_SHADER_VISIBILITY_PIXEL);
	pipelineStateObjects_.back()->SetStaticSampler(1, D3D12_SHADER_VISIBILITY_VERTEX);

	pipelineStateObjects_.back()->Initialize(device, shaderBlob.get());


	/// ----------------------------------------------
	/// ↓ Polygon Shader
	/// ----------------------------------------------


	shaderBlob.reset(new ShaderBlob());
	shaderBlob->Initialize(
		shaderCompile_.get(),
		L"./Shader/Polygon3d/Polygon3d.VS.hlsl", L"vs_6_0",
		L"./Shader/Polygon3d/Polygon3d.PS.hlsl", L"ps_6_0"
	);

	pipelineStateObjects_.push_back(std::make_unique<PipelineStateObject>());

	pipelineStateObjects_.back()->SetInputElement("POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);
	pipelineStateObjects_.back()->SetInputElement("NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT);

	pipelineStateObjects_.back()->SetRootParameter(D3D12_ROOT_PARAMETER_TYPE_CBV, D3D12_SHADER_VISIBILITY_PIXEL, 0); ///- Material
	pipelineStateObjects_.back()->SetRootParameter(D3D12_ROOT_PARAMETER_TYPE_CBV, D3D12_SHADER_VISIBILITY_PIXEL, 1); ///- DirectionalLight
	pipelineStateObjects_.back()->SetRootParameter(D3D12_ROOT_PARAMETER_TYPE_CBV, D3D12_SHADER_VISIBILITY_VERTEX, 0); ///- TransformationMatrix

	pipelineStateObjects_.back()->Initialize(device, shaderBlob.get());


	/// ----------------------------------------------
	/// ↓ Terrain Collision Shader
	/// ----------------------------------------------


	shaderBlob.reset(new ShaderBlob());
	shaderBlob->Initialize(
		shaderCompile_.get(),
		L"./Shader/Terrain/TerrainCollision.CS.hlsl", L"csMain", L"cs_6_0"
	);

	pipelineStateObjects_.push_back(std::make_unique<PipelineStateObject>());

	pipelineStateObjects_.back()->SetDescriptorRange(0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_UAV);

	pipelineStateObjects_.back()->SetRootParameter(D3D12_ROOT_PARAMETER_TYPE_CBV, D3D12_SHADER_VISIBILITY_ALL, 0); ///- texcoord
	pipelineStateObjects_.back()->SetRootParameterDescriptorTable(D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE, D3D12_SHADER_VISIBILITY_ALL, 0); ///- texture

	pipelineStateObjects_.back()->SetStaticSampler(0, D3D12_SHADER_VISIBILITY_ALL);

	pipelineStateObjects_.back()->InitializeComputeShaderVer(device, shaderBlob.get());



	shaderBlob.reset();
}


void PipelineStateObjectManager::Finalize() {
	pipelineStateObjects_.clear();
	shaderCompile_.reset();
}


void PipelineStateObjectManager::SetCommandList(uint32_t index, ID3D12GraphicsCommandList* commandList) {
	pipelineStateObjects_[index]->SetCommandList(commandList);
}

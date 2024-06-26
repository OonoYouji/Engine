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

	pipelineStateObjects_["Object3d"] = std::make_unique<PipelineStateObject>();
	PipelineStateObject* pso = pipelineStateObjects_.at("Object3d").get();

	pso->SetInputElement("POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);
	pso->SetInputElement("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT);
	pso->SetInputElement("NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT);

	pso->SetDescriptorRange(0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV);

	pso->SetCBV(D3D12_SHADER_VISIBILITY_PIXEL, 0);
	pso->SetCBV(D3D12_SHADER_VISIBILITY_VERTEX, 0);
	pso->SetDescriptorTable(D3D12_SHADER_VISIBILITY_PIXEL, 0);
	pso->SetCBV(D3D12_SHADER_VISIBILITY_PIXEL, 1);

	pso->SetStaticSampler(0, D3D12_SHADER_VISIBILITY_PIXEL);

	pso->Initialize(device, shaderBlob.get());



	/// ----------------------------------------------
	/// ↓ Terrain Shader
	/// ----------------------------------------------

	shaderBlob.reset(new ShaderBlob());
	shaderBlob->Initialize(
		shaderCompile_.get(),
		L"./Shader/Terrain/Terrain.VS.hlsl", L"vs_6_0",
		L"./Shader/Terrain/Terrain.PS.hlsl", L"ps_6_0"
	);

	pipelineStateObjects_["Terrain"] = std::make_unique<PipelineStateObject>();
	pso = pipelineStateObjects_.at("Terrain").get();

	pso->SetInputElement("POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);
	pso->SetInputElement("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT);
	pso->SetInputElement("NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT);

	pso->SetDescriptorRange(0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV);
	pso->SetDescriptorRange(0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV);
	pso->SetDescriptorRange(3, 1, D3D12_DESCRIPTOR_RANGE_TYPE_UAV);

	pso->SetCBV(D3D12_SHADER_VISIBILITY_PIXEL, 0);
	pso->SetCBV(D3D12_SHADER_VISIBILITY_VERTEX, 0);
	pso->SetDescriptorTable(D3D12_SHADER_VISIBILITY_PIXEL, 0);
	pso->SetDescriptorTable(D3D12_SHADER_VISIBILITY_VERTEX, 1);
	pso->SetDescriptorTable(D3D12_SHADER_VISIBILITY_ALL, 2);
	pso->SetCBV(D3D12_SHADER_VISIBILITY_PIXEL, 1);
	pso->SetCBV(D3D12_SHADER_VISIBILITY_ALL, 3);

	pso->SetStaticSampler(0, D3D12_SHADER_VISIBILITY_PIXEL);
	pso->SetStaticSampler(1, D3D12_SHADER_VISIBILITY_VERTEX);

	pso->Initialize(device, shaderBlob.get());


	/// ----------------------------------------------
	/// ↓ Polygon Shader
	/// ----------------------------------------------


	shaderBlob.reset(new ShaderBlob());
	shaderBlob->Initialize(
		shaderCompile_.get(),
		L"./Shader/Polygon3d/Polygon3d.VS.hlsl", L"vs_6_0",
		L"./Shader/Polygon3d/Polygon3d.PS.hlsl", L"ps_6_0"
	);

	pipelineStateObjects_["Polygon3d"] = std::make_unique<PipelineStateObject>();
	pso = pipelineStateObjects_.at("Polygon3d").get();

	pso->SetInputElement("POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);
	pso->SetInputElement("NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT);

	pso->SetCBV(D3D12_SHADER_VISIBILITY_PIXEL, 0); ///- Material
	pso->SetCBV(D3D12_SHADER_VISIBILITY_PIXEL, 1); ///- DirectionalLight
	pso->SetCBV(D3D12_SHADER_VISIBILITY_VERTEX, 0); ///- TransformationMatrix

	pso->Initialize(device, shaderBlob.get());


	/// ----------------------------------------------
	/// ↓ Terrain Collision Shader
	/// ----------------------------------------------


	shaderBlob.reset(new ShaderBlob());
	shaderBlob->InitializeCS(
		shaderCompile_.get(),
		L"./Shader/Terrain/TerrainCollision.CS.hlsl",
		L"cs_6_6"
	);

	pipelineStateObjects_["TerrainCS"] = std::make_unique<PipelineStateObject>();
	pso = pipelineStateObjects_.at("TerrainCS").get();

	pso->SetDescriptorRange(0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV); ///- texture
	pso->SetDescriptorRange(1, 1, D3D12_DESCRIPTOR_RANGE_TYPE_UAV); ///- texture

	pso->SetCBV(D3D12_SHADER_VISIBILITY_ALL, 0); ///- texcoord
	pso->SetDescriptorTable(D3D12_SHADER_VISIBILITY_ALL, 0); ///- texture
	pso->SetUAV(D3D12_SHADER_VISIBILITY_ALL, 0); ///- 
	pso->SetDescriptorTable(D3D12_SHADER_VISIBILITY_ALL, 1); ///- uavTexture

	pso->SetStaticSampler(0, D3D12_SHADER_VISIBILITY_ALL);

	pso->InitializeComputeShaderVer(device, shaderBlob.get());



	shaderBlob.reset();
}


void PipelineStateObjectManager::Finalize() {
	pipelineStateObjects_.clear();
	shaderCompile_.reset();
}


void PipelineStateObjectManager::SetCommandList(const std::string& key, ID3D12GraphicsCommandList* commandList) {
	pipelineStateObjects_.at(key)->SetCommandList(commandList);
}

void PipelineStateObjectManager::SetComputeCommandList(const std::string& key, ID3D12GraphicsCommandList* commandList) {
	pipelineStateObjects_.at(key)->SetComputeCommandList(commandList);
}

#include <Polygon3d.h>

#include <DxCommand.h>
#include <PipelineStateObjectManager.h>


Polygon3d::Polygon3d() {}
Polygon3d::~Polygon3d() {}


void Polygon3d::Initialize() {

}


void Polygon3d::Draw() {
	ID3D12GraphicsCommandList* commandList = DxCommand::GetInstance()->GetList();

	///- psoの設定
	PipelineStateObjectManager::GetInstance()->SetCommandList("Polygon3d", commandList);



}

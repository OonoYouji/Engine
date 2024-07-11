#include <LineDrawer.h>

#include <DxCommand.h>
#include <PipelineStateObjectManager.h>



LineDrawer* LineDrawer::GetInstance() {
	static LineDrawer instance;
	return &instance;
}



void LineDrawer::PostDraw() {

}



void LineDrawer::Draw(const Vec3f& v1, const Vec3f& v2, const Vec4f& color) {
	ID3D12GraphicsCommandList* commandList = DxCommand::GetInstance()->GetList();
	PipelineStateObjectManager::GetInstance()->SetCommandList("Line", commandList);


	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);


}

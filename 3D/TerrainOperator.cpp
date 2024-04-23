#include "TerrainOperator.h"

#include <Engine.h>
#include <ImGuiManager.h>

#include "Terrain.h"
#include "Brush.h"



TerrainOperator::TerrainOperator() {}
TerrainOperator::~TerrainOperator() {}



void TerrainOperator::Init(Terrain* terrain, Brush* brush) {

	input_ = Input::GetInstance();

	pTerrain_ = terrain;
	pBrush_ = brush;


	isOperating_ = false;

	raisePower_ = 0.5f;

}



void TerrainOperator::Update() {



#ifdef _DEBUG

	ImGui::Begin("Brush");

	ImGui::Checkbox("IsOperating", &isOperating_);
	if(isOperating_) {
		ImGui::Text("IsOperating: True");
	} else {
		ImGui::Text("IsOperating: False");
	}

	ImGui::Spacing();

	ImGui::SliderFloat("raisePower", &raisePower_, 0.0f, 1.0f);

	ImGui::End();

#endif // _DEBUG





	/// -------------------------------------
	/// ↓ ブラシと地形の当たり判定をとる
	/// -------------------------------------


	///- 色リセット
	pBrush_->SetColot({ 1.0f,1.0f,1.0f,1.0f });

	///- ブラシのWorld座標計算
	BrushPositionCalc();


	///- 地形との当たり判定; XZのみで判定を取る
	if(Collision() && isOperating_) {
		if(input_->GetMouse().leftButton) {
			pBrush_->SetColot({ 0.25f,0.0f,0.0f,1.0f });

			///- 地形上のどこにブラシがあるか計算
			Vec3f localPos = TerrainLocalPosition();
			int rowIndex = (pTerrain_->kSubdivision / 2) + static_cast<int>(localPos.z);
			int colIndex = (pTerrain_->kSubdivision / 2) + static_cast<int>(localPos.x);

			///- 操作する範囲の設定
			int operatorRange = static_cast<int>(pBrush_->GetRadius());

			for(int32_t row = -operatorRange; row < operatorRange; row++) {
				for(int32_t col = -operatorRange; col < operatorRange; col++) {

					float height = 1.0f - (row / operatorRange) * (col / operatorRange);
					pTerrain_->SetVertexHeight(rowIndex + row, colIndex + col, height * raisePower_);

				}
			}

			///- 頂点を操作したのでフラット化する
			pTerrain_->TransferFlattenedVertexData();

		}
	}

}



bool TerrainOperator::Collision() {



	///- 地形の範囲内かどうかを計算

	///- Y座標を考慮しない
	///- X,Zのみで判定を取る


	///- X軸判定
	if(pBrush_->GetWorldPos().x > pTerrain_->GetLTPos().x
	   && pBrush_->GetWorldPos().x < pTerrain_->GetRBPos().x) {


		if(pBrush_->GetWorldPos().z > pTerrain_->GetRBPos().z
		   && pBrush_->GetWorldPos().z < pTerrain_->GetLTPos().z) {




			return true;
		}

	}






	return false;
}

void TerrainOperator::BrushPositionCalc() {

	///- 地形の法線ベクトルとブラシの差分ベクトル; 内積を計算し垂直かどうかの判定を得る
	float dot = Vector3::Dot(pTerrain_->GetNormalVector(), pBrush_->GetLayDir());

	///- 0割り防止; 0のときは↑の計算に使ったベクトルが垂直になる
	if(dot == 0.0f) { return; }

	///- マウスのnearからfarの線形補完のためのtを計算する
	float t = (pTerrain_->GetDistance() - Vector3::Dot(pBrush_->GetNearPos(), pTerrain_->GetNormalVector())) / dot;

	///- ↑で計算したtで線形補完する
	///- ブラシの座標を地形の上ちょうどに表示されるように計算する
	pBrush_->SetWorldPosition(Vector3::Lerp(pBrush_->GetNearPos(), pBrush_->GetFarPos(), t));

}




Vec3f TerrainOperator::TerrainLocalPosition() {
	return  pBrush_->GetWorldPos() - pTerrain_->GetWorldPos();
}

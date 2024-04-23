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


}



void TerrainOperator::Update() {

	/// -------------------------------------
	/// ↓ ブラシと地形の当たり判定をとる
	/// -------------------------------------

	///- 色リセット
	pBrush_->SetColot({ 1.0f,1.0f,1.0f,1.0f });

	///- ブラシのWorld座標計算
	BrushPositionCalc();


	///- 地形との当たり判定; XZのみで判定を取る
	if(Collision()) {
		if(input_->GetMouse().leftButton) {
			pBrush_->SetColot({ 0.25f,0.0f,0.0f,1.0f });

			///- 地形上のどこにブラシがあるか計算


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
	return pTerrain_->GetWorldPos() - pBrush_->GetWorldPos();
}

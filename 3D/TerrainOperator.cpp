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

	if(Collision()) {

		pBrush_->SetColot({ 1.0f,1.0f,1.0f,1.0f });
		if(input_->GetMouse().leftButton) {
			pBrush_->SetColot({ 0.5f,0.0f,0.0f,1.0f });

		}
	}

}



bool TerrainOperator::Collision() {

	///- 地形の法線ベクトルとブラシの差分ベクトル; 内積を計算し垂直かどうかの判定を得る
	float dot = Vector3::Dot(pTerrain_->GetNormalVector(), pBrush_->GetLayDir());

	///- 0割り防止; 0のときは↑の計算に使ったベクトルが垂直になる
	if(dot == 0.0f) { return false; }

	///- マウスのnearからfarの線形補完のためのtを計算する
	float t = (pTerrain_->GetDistance() - Vector3::Dot(pBrush_->GetNearPos(), pTerrain_->GetNormalVector())) / dot;

	///- ↑で計算したtで線形補完する
	///- ブラシの座標を地形の上ちょうどに表示されるように計算する
	pBrush_->SetWorldPosition(Vector3::Lerp(pBrush_->GetNearPos(), pBrush_->GetFarPos(), t));

	return true;
}

#pragma once

#include "Input.h"
#include "Vector3.h"

class Terrain;
class Brush;


/// <summary>
/// 地形を操作するクラス
/// </summary>
class TerrainOperator {
public:


	TerrainOperator();
	~TerrainOperator();


	void Init(Terrain* terrain, Brush* brush);
	void Update();

private:

	Input* input_ = nullptr;

	Terrain* pTerrain_;
	Brush* pBrush_;



	bool Collision();

	void BrushPositionCalc();

	Vec3f TerrainLocalPosition();

};
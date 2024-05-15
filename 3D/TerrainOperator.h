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

	InputManager* input_ = nullptr;

	Terrain* pTerrain_;
	Brush* pBrush_;

	bool isOperating_;
	float raisePower_;


	bool Collision();

	void BrushPositionCalc();

	Vec3f TerrainLocalPosition();

	/// <summary>
	/// 地形の一部を上げる
	/// </summary>
	void Raise();


	/// <summary>
	/// 地形の一部を下げる
	/// </summary>
	void Lower();


};
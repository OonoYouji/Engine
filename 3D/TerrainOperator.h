#pragma once


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


	Terrain* pTerrain_;
	Brush* pBrush_;


	bool Collision();

};
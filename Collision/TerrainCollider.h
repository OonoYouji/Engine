#pragma once

#include <wrl/client.h>
#include <d3d12.h>

#include <vector>

#include <Vector2.h>
#include <Vector3.h>
#include <Matrix4x4.h>

using namespace Microsoft::WRL;

class Player;
class Terrain;

/// <summary>
/// 地形との衝突判定を取る
/// </summary>
class TerrainCollider final {
public:

	TerrainCollider();
	~TerrainCollider();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 
	/// </summary>
	void Update();

	void SetPlayer(Player* player);

	void SetTerrain(Terrain* terrain);

	/// <summary>
	/// texcoordから高さを得る
	/// </summary>
	/// <param name="texcoord"></param>
	/// <returns></returns>
	float GetHeight(const Vec2f& texcoord);

private:

	/// <summary>
	/// Playerが地形に衝突しているか取る
	/// </summary>
	bool IsCollision();

	/// <summary>
	/// World座標でのpositionを地形のtexcoordに変換する
	/// </summary>
	Vec2f ConvertTexcoord(const Vec3f& position);

	/// <summary>
	/// Playerが範囲内にいるか確認する
	/// </summary>
	/// <returns></returns>
	bool IsWithinRange();

	/// <summary>
	/// outputBufferの生成
	/// </summary>
	void CreateOutputBuffer();

	/// <summary>
	/// ImGuiでデバッグ表示
	/// </summary>
	void ImGuiDebug();

private: ///- OBJECTS

	Player* pPlayer_ = nullptr;
	Terrain* pTerrain_ = nullptr;

	std::vector<Vec3f> terrainVertices_;
	Vec3f terrainMinPosition_;
	Vec3f terrainMaxPosition_;
	Vec3f terrainSize_;

	///- TerrainのWorldMatrixの逆行列
	Matrix4x4 inverseTerrainWorldMatrix_;
	Matrix4x4 terrainWorldMatrix_;

	///- TerrainのLocal空間でのPlayerの座標
	Vec3f playerTerrainLocalPosition_;

	///- CSの計算結果の格納場所
	ComPtr<ID3D12Resource> outputBuffer_;
	ComPtr<ID3D12Resource> readBackBuffer_;
	Vec4f* outputData_ = nullptr;


	///- Texcoord用 定数バッファ
	ComPtr<ID3D12Resource> texcoordResource_;
	Vec2f* texcoordData_ = nullptr;

	Vec2f texcoord_;
	float preHeight_;
	float height_;
	Vec4f outputColor_{};

	const float kWallHeight_ = 0.25f;


};


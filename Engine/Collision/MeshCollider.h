#pragma once

#include <array>
#include <vector>

#include <Vector3.h>

#include <WorldTransform.h>
#include <Model.h>

#include <BaseCollider.h>


/// <summary>
/// 当たり判定用メッシュ
/// </summary>
class MeshCollider
	: public BaseCollider {
public:

	MeshCollider();
	~MeshCollider();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

	/// <summary>
	/// 当たり判定メッシュの描画
	/// </summary>
	void Draw();

public:

	/// <summary>
	/// ImGuiを使用してデバッグ表示する
	/// </summary>
	void DebugDraw(const std::string& windowName);

	/// <summary>
	/// 当たり判定を取る
	/// </summary>
	/// <param name="vertices"></param>
	/// <returns></returns>
	bool IsCollision(const std::vector<Vec3f>& vertices, const WorldTransform& worldTransform);

	/// <summary>
	/// モデルへのポインタのSetter
	/// </summary>
	/// <param name="model">特定のモデルへのポインタ</param>
	void SetModel(const Model* model) {
		pModel_ = model;
	}

	/// <summary>
	/// WorldTransformへのポインタのSetter
	/// </summary>
	/// <param name="worldTransform"></param>
	void SetWorldTransform(const WorldTransform* worldTransform) {
		pWorldTarnsform_ = worldTransform;
	}

private: ///- METHODS


private:

	/////- パラメータ -/////
	//- 当たり判定があるか
	//- 動くオブジェクトか
	//- 

	const Model* pModel_;
	const WorldTransform* pWorldTarnsform_;

	std::vector<Vec3f> vertices_; //- world座標系
	std::vector<Vec3f> minkowskiDiff_;

	uint32_t repeatNum_;

};

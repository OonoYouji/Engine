#pragma once 

#include <Model.h>
#include <WorldTransform.h>
#include <GameObject.h>

/// <summary>
/// プレイヤー
/// </summary>
class Player final
	: public GameObject {
public:

	Player();
	~Player();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize() override;

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update() override;

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw() override;

	/// <summary>
	/// WorldTransformの取得
	/// </summary>
	/// <returns></returns>
	const WorldTransform& GetWorldTransform() const { return worldTransform_; }

	void SetHeight(float height);

	/// <summary>
	/// 今のheightと前のheightの差分
	/// </summary>
	/// <returns></returns>
	float GetDiffHeight();

private: ///- METHODS

	void Move();

	void ImGuiDebug();

private: ///- OBJECTS

	Model* model_;

	Vec3f velocity_;
	const float kSpeed_ = 0.125f;
	const float kGravity_ = -0.025f;

	float preTerrainHeight_ = 0;
	float terrainHeight_ = 0;
	
	bool isKinematic_ = false;
};
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
	void Initialize();

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

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

private: ///- METHODS

	void Move();

private: ///- OBJECTS

	std::unique_ptr<Model> model_;

	Vec3f velocity_;
	const float kSpeed_ = 0.125f;

};
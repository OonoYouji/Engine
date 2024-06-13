#pragma once 

#include <Model.h>
#include <WorldTransform.h>

/// <summary>
/// プレイヤー
/// </summary>
class Player final {
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
	void Draw();

	/// <summary>
	/// WorldTransformの取得
	/// </summary>
	/// <returns></returns>
	const WorldTransform& GetWorldTransform() const { return worldTransform_; }

	void SetHeight(float height);

private: ///- METHODS

	void Move();

	/// <summary>
	/// ImGuiDebug
	/// </summary>
	void ImGuiDebug();

private: ///- OBJECTS

	std::unique_ptr<Model> model_;
	WorldTransform worldTransform_;

	Vec3f velocity_;
	const float kSpeed_ = 0.125f;

};
#pragma once

#include <string>

#include <Camera.h>
#include <Vector3.h>
#include <WorldTransform.h>
#include <Model.h>

#include <GameObject.h>

/// <summary>
/// デバッグカメラ
/// </summary>
class DebugCamera
	: public GameObject {
public:

	DebugCamera();
	~DebugCamera();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initalize();

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw();

	/// <summary>
	/// ImGuiでデバッグ表示
	/// </summary>
	/// <param name="imguiName"></param>
	void ImGuiDebug() override;

private:

	ViewProjection viewProjection_;

	Vec3f velocity_;
	const float kSpeed_ = 0.025f;


public:
	const ViewProjection& GetViewProjection() const { return viewProjection_; }
};
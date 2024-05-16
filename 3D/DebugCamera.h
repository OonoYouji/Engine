#pragma once

#include <string>

#include <Camera.h>
#include <Vector3.h>
#include <WorldTransform.h>

/// <summary>
/// デバッグカメラ
/// </summary>
class DebugCamera {
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
	/// ImGuiでデバッグ表示
	/// </summary>
	/// <param name="imguiName"></param>
	void DebugDraw();

private:

	WorldTransform worldTransform_;
	ViewProjection viewProjection_;

public:
	const ViewProjection& GetViewProjection() const { return viewProjection_; }
};
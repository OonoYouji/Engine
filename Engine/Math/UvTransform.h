#pragma once

#include <Vector3.h>
#include <Matrix4x4.h>

/// <summary>
/// textureのSRT
/// </summary>
struct UvTransform final {

	/// <summary>
	/// 初期化
	/// </summary>
	void Initilize();

	/// <summary>
	/// 行列の更新
	/// </summary>
	void UpdateMatrix();

	Vec2f scale = { 1.0f, 1.0f };
	float rotate = 0.0f;
	Vec2f tranalate = { 0.0f,0.0f };

	Mat4 matTransform;

};
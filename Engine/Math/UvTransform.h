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

	Vec2f scale;
	float rotate;
	Vec2f tranalate;

	Mat4 matTransform;

};
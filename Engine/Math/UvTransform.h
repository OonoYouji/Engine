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

	Vec3f scale;
	Vec3f rotate;
	Vec3f tranalate;

	Mat4 matTransform;

};
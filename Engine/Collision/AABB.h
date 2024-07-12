#pragma once

#include <Vector3.h>
#include <Vector4.h>

/// ===================================================
/// AABB
/// ===================================================
struct AABB final {
public:

	AABB();
	~AABB();

	/// ===================================================
	/// public : methods
	/// ===================================================

	/// <summary>
	/// 引数のpointでmin maxの再計算
	/// </summary>
	/// <param name="point"></param>
	void ExpandToFit(const Vec3f& point);

	/// <summary>
	/// AABBを描画
	/// </summary>
	void Draw(const Vector4& color = { 1.0f, 1.0f, 1.0f, 1.0f });

public:

	/// ===================================================
	/// public : objects
	/// ===================================================

	Vec3f min = { 0.0f,0.0f,0.0f };
	Vec3f max = { 0.0f,0.0f,0.0f };
	Vec3f translation = { 0.0f, 0.0f, 0.0f };

};
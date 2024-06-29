#pragma once

#include <string>

#include <Vector3.h>
#include <Matrix4x4.h>


/// <summary>
/// World
/// </summary>
struct WorldTransform {

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 
	/// </summary>
	void UpdateMatTransform();

	/// <summary>
	/// 
	/// </summary>
	void ImGuiTreeNodeDebug();

	/// <summary>
	/// 
	/// </summary>
	/// <returns></returns>
	Vec3f GetWorldPosition();

	Vec3f scale;
	Vec3f rotate;
	Vec3f translate;

	Mat4 matTransform;



};
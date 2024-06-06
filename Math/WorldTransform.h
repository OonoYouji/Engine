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

	
	/// -------------------------
	/// メンバ関数
	/// -------------------------

	void UpdateWorldMatrix();

	void ImGuiTreeNodeDebug();

	void ImGuiDebug(const std::string& windowName);

	/// -------------------------
	/// メンバ変数
	/// -------------------------

	Vec3f scale;
	Vec3f rotate;
	Vec3f translate;

	Mat4 worldMatrix;



};
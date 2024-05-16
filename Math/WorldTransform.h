#pragma once

#include <Vector3.h>
#include <Matrix4x4.h>


/// <summary>
/// World
/// </summary>
struct WorldTransform {
public:


	/// -------------------------
	/// 初期化関数
	/// -------------------------

	void Initialize();


	/// -------------------------
	/// メンバ変数
	/// -------------------------

	Vec3f scale;
	Vec3f rotate;
	Vec3f translate;

	Mat4 worldMatrix;


	/// -------------------------
	/// メンバ関数
	/// -------------------------

	void UpdateWorldMatrix();

	void ImGuiTreeNodeDebug();

};
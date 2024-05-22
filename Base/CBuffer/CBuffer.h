#pragma once 

#include <cmath>

#include <Vector2.h>
#include <Vector3.h>
#include <Vector4.h>
#include <Matrix4x4.h>



/// <summary>
/// Textureあり図形の頂点情報
/// </summary>
struct VertexData {
	Vec4f position;
	Vec2f texcoord;
	Vec3f normal;
};


/// <summary>
/// Textureなし図形の頂点情報
/// </summary>
struct Polygon3dVertexData {
	Vec4f position;
	Vec3f normal;
};


/// <summary>
/// 色情報
/// </summary>
struct Material {
	Vec4f color;
	int32_t enableLighting;
};


/// <summary>
/// 行列情報
/// </summary>
struct TransformMatrix {
	Mat4 WVP;
	Mat4 World;
};
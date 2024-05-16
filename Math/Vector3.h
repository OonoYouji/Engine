#pragma once

#include "Vector4.h"

class Vector3 final {
public:

	Vector3() = default;
	Vector3(float x, float y, float z);
	Vector3(const Vector3& obj);


	~Vector3() = default;

public:

	float x, y, z;

	static const Vector3 up;
	static const Vector3 down;
	static const Vector3 left;
	static const Vector3 right;
	static const Vector3 back;
	static const Vector3 front;

public:

	/// <summary>
	/// ベクトルの長さ
	/// </summary>
	static float Length(const Vector3& v);

	/// <summary>
	/// 正規化
	/// </summary>
	static Vector3 Normalize(const Vector3& v);

	/// <summary>
	/// 外積
	/// </summary>
	static Vector3 Cross(const Vector3& v1, const Vector3& v2);

	/// <summary>
	/// 内積
	/// </summary>
	static float Dot(const Vector3& v1, const Vector3& v2);

	/// <summary>
	/// 線形補完
	/// </summary>
	static Vector3 Lerp(const Vector3& v1, const Vector3& v2, float t);

	/// <summary>
	/// Vector4をVector3に変換
	/// </summary>
	static Vector3 Convert4To3(const Vec4f& v);

public:

	inline Vector3 operator+(const Vector3& other) const;
	inline Vector3 operator-(const Vector3& other) const;
	inline Vector3 operator/(const Vector3& other) const;
	inline Vector3 operator*(const Vector3& other) const;
	inline Vector3 operator/(float value) const;
	inline Vector3 operator*(float value) const;


};


inline Vector3 Vector3::operator+(const Vector3& other) const {
	return Vector3(
		this->x + other.x,
		this->y + other.y,
		this->z + other.z
	);
}
inline Vector3 Vector3::operator-(const Vector3& other) const {
	return Vector3(
		this->x - other.x,
		this->y - other.y,
		this->z - other.z
	);
}
inline Vector3 Vector3::operator/(const Vector3& other) const {
	return Vector3(
		this->x / other.x,
		this->y / other.y,
		this->z / other.z
	);
}
inline Vector3 Vector3::operator*(const Vector3& other) const {
	return Vector3(
		this->x * other.x,
		this->y * other.y,
		this->z * other.z
	);
}
inline Vector3 Vector3::operator/(float value) const {
	return Vector3(
		this->x / value,
		this->y / value,
		this->z / value
	);
}
inline Vector3 Vector3::operator*(float value) const {
	return Vector3(
		this->x * value,
		this->y * value,
		this->z * value
	);
}




using Vec3f = Vector3;
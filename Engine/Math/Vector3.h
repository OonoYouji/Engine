#pragma once

#include "Vector4.h"
#include "Vector2.h"

class Vector3 final {
public:

	Vector3() = default;
	Vector3(float x, float y, float z);
	Vector3(const Vector3& obj);

	Vector3(const Vector2& v, float x);

	~Vector3() = default;

public:

	float x, y, z;

#pragma region 各方向への単位ベクトル
	/// <summary>
	/// (0, 1, 0)
	/// </summary>
	static const Vector3 up;

	/// <summary>
	/// (0, -1, 0)
	/// </summary>
	static const Vector3 down;

	/// <summary>
	/// (-1, 0, 0)
	/// </summary>
	static const Vector3 left;

	/// <summary>
	/// (1, 0, 0)
	/// </summary>
	static const Vector3 right;

	/// <summary>
	/// (0, 0, -1)
	/// </summary>
	static const Vector3 back;

	/// <summary>
	/// (0, 0, 1)
	/// </summary>
	static const Vector3 front;
#pragma endregion

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

	inline Vector3& operator+=(const Vector3& other);
	inline Vector3& operator-=(const Vector3& other);
	inline Vector3& operator*=(const Vector3& other);
	inline Vector3& operator/=(const Vector3& other);
	inline Vector3& operator/=(float value);
	inline Vector3& operator*=(float value);

	inline Vector3 operator-() const;
	
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

inline Vector3& Vector3::operator+=(const Vector3& other) {
	(*this) = (*this) + other;
	return *this;
}
inline Vector3& Vector3::operator-=(const Vector3& other) {
	(*this) = (*this) - other;
	return *this;
}
inline Vector3& Vector3::operator*=(const Vector3& other) {
	(*this) = (*this) * other;
	return *this;
}
inline Vector3& Vector3::operator/=(const Vector3& other) {
	(*this) = (*this) / other;
	return *this;
}
inline Vector3& Vector3::operator/=(float value) {
	(*this) = (*this) / value;
	return *this;
}
inline Vector3& Vector3::operator*=(float value) {
	(*this) = (*this) * value;
	return *this;
}


inline Vector3 Vector3::operator-() const {
	return (*this) * -1.0f;
}


using Vec3f = Vector3;
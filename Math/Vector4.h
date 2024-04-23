#pragma once

class Vector4 final {
public:

	Vector4() = default;
	Vector4(const Vector4& other);
	Vector4(Vector4&& other);
	Vector4(float x, float y, float z, float w);

	~Vector4() = default;

public:

	float x;
	float y;
	float z;
	float w;

public:

	/*-----------------------------------
		演算子オーバーロード
	-----------------------------------*/

	inline Vector4 operator+(const Vector4& other)const;
	inline Vector4 operator-(const Vector4& other)const;
	inline Vector4 operator/(const Vector4& other)const;

	inline Vector4 operator/(float value)const;

	inline Vector4& operator-= (const Vector4& other);

	inline Vector4& operator= (const Vector4& other);

};


inline Vector4 Vector4::operator+(const Vector4& other) const {
	return Vector4(
		this->x + other.x,
		this->y + other.y,
		this->z + other.z,
		this->w + other.w
	);
}
inline Vector4 Vector4::operator-(const Vector4& other) const {
	return Vector4(
		this->x - other.x,
		this->y - other.y,
		this->z - other.z,
		this->w - other.w
	);
}
inline Vector4 Vector4::operator/(const Vector4& other) const {
	return Vector4(
		this->x / other.x,
		this->y / other.y,
		this->z / other.z,
		this->w / other.w
	);
}

inline Vector4 Vector4::operator/(float value) const {
	return Vector4(
		this->x / value,
		this->y / value,
		this->z / value,
		this->w / value
	);
}


inline Vector4& Vector4::operator-=(const Vector4& other) {
	*this = *this - other;
	return *this;
}


inline Vector4& Vector4::operator=(const Vector4& other) {
	this->x = other.x;
	this->y = other.y;
	this->z = other.z;
	this->w = other.w;
	return *this;
}


///- 名前の追加
using Vec4f = Vector4;
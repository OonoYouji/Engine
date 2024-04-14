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

	Vector4& operator= (const Vector4& other);

};


inline Vector4 Vector4::operator+(const Vector4& other) const {
	return Vector4(
		this->x + other.x,
		this->y + other.y,
		this->z + other.z,
		this->w + other.w
	);
}

///- 名前の追加
using Vec4f = Vector4;
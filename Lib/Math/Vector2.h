#pragma once

class Vector2 {
public:

	Vector2() = default;
	Vector2(const Vector2& other);
	Vector2(Vector2&& other);
	Vector2(float x, float y);

	~Vector2() = default;

public:

	/// 2次元ベクトル
	float x, y;

public:

	inline Vector2 operator+ (const Vector2& other) const;
	inline Vector2 operator- (const Vector2& other) const;
	inline Vector2 operator/ (const Vector2& other) const;
	inline Vector2 operator* (const Vector2& other) const;
	inline Vector2 operator/ (float value) const;
	inline Vector2 operator* (float value) const;

	inline Vector2& operator+= (const Vector2& other);
	inline Vector2& operator-= (const Vector2& other);
	inline Vector2& operator/= (const Vector2& other);
	inline Vector2& operator*= (const Vector2& other);
	inline Vector2& operator/= (float value);
	inline Vector2& operator*= (float value);

};


inline Vector2 Vector2::operator+(const Vector2& other) const {
	return Vector2(
		this->x + other.x,
		this->y + other.y
	);
}

inline Vector2 Vector2::operator-(const Vector2& other) const {
	return Vector2(
		this->x - other.x,
		this->y - other.y
	);
}

inline Vector2 Vector2::operator/(const Vector2& other) const {
	return Vector2(
		this->x / other.x,
		this->y / other.y
	);
}

inline Vector2 Vector2::operator*(const Vector2& other) const {
	return Vector2(
		this->x * other.x,
		this->y * other.y
	);
}

inline Vector2 Vector2::operator/(float value) const {
	return Vector2(
		this->x / value,
		this->y / value
	);
}

inline Vector2 Vector2::operator*(float value) const {
	return Vector2(
		this->x * value,
		this->y * value
	);
}

inline Vector2& Vector2::operator+=(const Vector2& other) {
	this->x += other.x;
	this->y += other.y;
	return *this;
}

inline Vector2& Vector2::operator-=(const Vector2& other) {
	this->x -= other.x;
	this->y -= other.y;
	return *this;
}

inline Vector2& Vector2::operator/=(const Vector2& other) {
	this->x /= other.x;
	this->y /= other.y;
	return *this;
}

inline Vector2& Vector2::operator*=(const Vector2& other) {
	this->x *= other.x;
	this->y *= other.y;
	return *this;
}

inline Vector2& Vector2::operator/=(float value) {
	this->x /= value;
	this->y /= value;
	return *this;
}

inline Vector2& Vector2::operator*=(float value) {
	this->x *= value;
	this->y *= value;
	return *this;
}

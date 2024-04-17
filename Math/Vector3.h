#pragma once

class Vector3 final {
public:

	Vector3() = default;
	Vector3(float x, float y, float z);
	Vector3(const Vector3& obj);


	~Vector3() = default;

public:

	float x, y, z;

public:

	static float Length(const Vector3& v);

	static Vector3 Normalize(const Vector3& v);

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
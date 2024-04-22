#include <Vector3.h>

#include <cmath>
#include <iostream>

Vector3::Vector3(float x, float y, float z) {
	this->x = x;
	this->y = y;
	this->z = z;
}

Vector3::Vector3(const Vector3& obj) {
	*this = obj;
}


float Vector3::Length(const Vector3& v) {
	return std::sqrtf(std::powf(v.x, 2.0f) + std::powf(v.y, 2.0f) + std::powf(v.z, 2.0f));
}

Vector3 Vector3::Normalize(const Vector3& v) {
	float len = Length(v);
	if(len == 0) {
		return v;
	}
	return v / len;
}

Vector3 Vector3::Cross(const Vector3& v1, const Vector3& v2) {
	return Vector3(
		v1.y * v2.z - v1.z * v2.y,
		v1.z * v2.x - v1.x * v2.z,
		v1.x * v2.y - v1.y * v2.x
	);
}


float Vector3::Dot(const Vector3& v1, const Vector3& v2) {
	return (v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z);
}


Vector3 Vector3::Lerp(const Vector3& v1, const Vector3& v2, float t) {
	return Vector3(
		std::lerp(v1.x, v2.x, t),
		std::lerp(v1.y, v2.y, t),
		std::lerp(v1.z, v2.z, t)
	);
}
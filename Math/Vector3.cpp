#include <Vector3.h>

#include <cmath>


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

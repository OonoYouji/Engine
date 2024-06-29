#include <Vector4.h>

#include <cmath>


Vector4::Vector4(const Vector4& other) {
	this->x = other.x;
	this->y = other.y;
	this->z = other.z;
	this->w = other.w;
}

Vector4::Vector4(Vector4&& other) {
	this->x = other.x;
	this->y = other.y;
	this->z = other.z;
	this->w = other.w;
}

Vector4::Vector4(float x, float y, float z, float w) {
	this->x = x;
	this->y = y;
	this->z = z;
	this->w = w;
}


Vector4 Vector4::Normalize(const Vector4& v) {
	float len = Length(v);
	if(len == 0) {
		return v;
	}
	return v / len;
}

float Vector4::Length(const Vector4& v) {
	return std::sqrtf(std::pow(v.x, 2.0f) + std::pow(v.y, 2.0f) + std::pow(v.z, 2.0f) + std::pow(v.w, 2.0f));
}

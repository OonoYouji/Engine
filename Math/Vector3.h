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

	


};


using Vec3f = Vector3;
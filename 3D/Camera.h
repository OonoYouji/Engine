#pragma once

#include <Vector3.h>
#include <Matrix4x4.h>

class Camera final {
public:

	Camera();
	~Camera();

	void Init();
	void Update();
	void Draw();
	void Finalize();

private:

	Vec3f scale_;
	Vec3f rotate_;
	Vec3f worldPos_;

	Vec3f localFront_;
	Vec3f direction_;

	Matrix4x4 worldMatrix_;
	Matrix4x4 viewMatrix_;
	Matrix4x4 projectionMatrix_;

	Matrix4x4 vpMatrix_;

public:

	const Matrix4x4& GetVpMatrix() const { return vpMatrix_; }

};
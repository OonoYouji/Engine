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

	float fovY_;
	float farZ_;

	Matrix4x4 worldMatrix_;
	Matrix4x4 viewMatrix_;
	Matrix4x4 projectionMatrix_;

	Matrix4x4 vpMatrix_;

	/// <summary>
	/// ProjectionMatrixの再計算
	/// </summary>
	void MakeProjectionMatrix();

public:

	const Matrix4x4& GetVpMatrix() const { return vpMatrix_; }

	const Matrix4x4& GetProjectionMatrix() const { return projectionMatrix_; }

	const Matrix4x4& GetViewMatrix() const { return viewMatrix_; }

	const Matrix4x4& GetWorldMatrix() const { return worldMatrix_; }

	const Vec3f& GetPosition() const { return worldPos_; }

};
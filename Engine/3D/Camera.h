#pragma once

#include <memory>

#include <Vector3.h>
#include <Matrix4x4.h>
#include <WorldTransform.h>

#include <GameObject.h>

class DebugCamera;

/// <summary>
/// カメラに必須な構造体
/// </summary>
struct ViewProjection {
	Mat4 matView;
	Mat4 matProjection;
	float fovY;
	float farZ;
	void UpdateProjection();
	ViewProjection& operator=(const ViewProjection& other);
};

class Camera final
	: public GameObject {
public:

	Camera();
	~Camera();

	void Initialize() override;
	void Update() override;
	void Draw() override;
	void ImGuiDebug();
	void Finalize();

private:

	ViewProjection viewProjection_;

	Vec3f localFront_;
	Vec3f direction_;
	Mat4 vpMatrix_;

#ifdef _DEBUG
	DebugCamera* debugCamera_;
#endif // _DEBUG
public:

	const Mat4& GetVpMatrix() const { return vpMatrix_; }
	const Mat4& GetProjectionMatrix() const { return viewProjection_.matProjection; }
	const Mat4& GetViewMatrix() const { return viewProjection_.matView; }
	const Mat4& GetWorldMatrix() const { return worldTransform_.matTransform; }
	const Vec3f& GetPosition() const { return worldTransform_.translate; }

};


#include <Matrix4x4.h>

#include <cmath>

Matrix4x4::Matrix4x4(float aa, float ab, float ac, float ad, float ba, float bb, float bc, float bd, float ca, float cb, float cc, float cd, float da, float db, float dc, float dd) {

	m_[0][0] = aa;
	m_[0][1] = ab;
	m_[0][2] = ac;
	m_[0][3] = ad;

	m_[1][0] = ba;
	m_[1][1] = bb;
	m_[1][2] = bc;
	m_[1][3] = bd;

	m_[2][0] = ca;
	m_[2][1] = cb;
	m_[2][2] = cc;
	m_[2][3] = cd;

	m_[3][0] = da;
	m_[3][1] = db;
	m_[3][2] = dc;
	m_[3][3] = dd;

}

Matrix4x4 Matrix4x4::MakeIdentity() {
	return Matrix4x4(
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	);
}

Matrix4x4 Matrix4x4::MakeScale(const Vec3f& scale) {
	return Matrix4x4{
		scale.x, 0.0f, 0.0f, 0.0f,
		0.0f, scale.y, 0.0f, 0.0f,
		0.0f, 0.0f, scale.z, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};
}

Matrix4x4 Matrix4x4::MakeRotateX(float theta) {
	return Matrix4x4{
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, std::cosf(theta), std::sinf(theta), 0.0f,
		0.0f, -std::sinf(theta) , std::cosf(theta) , 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};
}

Matrix4x4 Matrix4x4::MakeRotateY(float theta) {
	return Matrix4x4{
		std::cosf(theta), 0.0f, -std::sinf(theta), 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		std::sinf(theta), 0.0f, std::cosf(theta), 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};
}

Matrix4x4 Matrix4x4::MakeRotateZ(float theta) {
	return Matrix4x4{
		std::cosf(theta), std::sinf(theta), 0.0f, 0.0f,
		-std::sinf(theta), std::cosf(theta), 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};
}

Matrix4x4 Matrix4x4::MakeRotate(const Vec3f& rotate) {
	return MakeRotateX(rotate.x) * MakeRotateY(rotate.y) * MakeRotateZ(rotate.z);
}

Matrix4x4 Matrix4x4::MakeTranslate(const Vec3f& translate) {
	return Matrix4x4{
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		translate.x, translate.y, translate.z, 1.0f
	};
}

Matrix4x4 Matrix4x4::MakeAffine(const Vec3f& scale, const Vec3f& rotate, const Vec3f& translate) {
	Matrix4x4 S = MakeScale(scale);
	Matrix4x4 R = MakeRotate(rotate);
	Matrix4x4 T = MakeTranslate(translate);
	return S * R * T;
}

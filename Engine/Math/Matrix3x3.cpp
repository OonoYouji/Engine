#include <Matrix3x3.h>

#include <cmath>

#include <Matrix4x4.h>

Matrix3x3::Matrix3x3(float aa, float ab, float ac, float ba, float bb, float bc, float ca, float cb, float cc) {

	this->m[0][0] = aa;
	this->m[0][1] = ab;
	this->m[0][2] = ac;

	this->m[1][0] = ba;
	this->m[1][1] = bb;
	this->m[1][2] = bc;

	this->m[2][0] = ca;
	this->m[2][1] = cb;
	this->m[2][2] = cc;

}


Matrix3x3 Matrix3x3::MakeIdentity() {
	return Matrix3x3(
		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 1.0f
	);
}

Matrix3x3 Matrix3x3::MakeScale(const Vec2f& scale) {
	return Matrix3x3(
		scale.x, 0.0f, 0.0f,
		0.0f, scale.y, 0.0f,
		0.0f, 0.0f, 1.0f
	);
}

Matrix3x3 Matrix3x3::MakeRotate(float rotate) {
	return Matrix3x3(
		std::cos(rotate), std::sin(rotate), 0.0f,
		-std::sin(rotate), std::cos(rotate), 0.0f,
		0.0f, 0.0f, 1.0f
	);
}

Matrix3x3 Matrix3x3::MakeTranslate(const Vec2f& translate) {
	return Matrix3x3(
		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		translate.x, translate.y, 1.0f
	);
}

Matrix3x3 Matrix3x3::MakeAffine(const Vec2f& scale, float rotate, const Vec2f& translate) {
	Mat3 S = MakeScale(scale);
	Mat3 R = MakeRotate(rotate);
	Mat3 T = MakeTranslate(translate);
	return S * R * T;
}

Matrix4x4 Matrix3x3::CopyMatrix(const Matrix3x3& m) {
	return {
		m.m[0][0],m.m[0][1],m.m[0][2],0.0f,
		m.m[1][0],m.m[1][1],m.m[1][2],0.0f,
		m.m[2][0],m.m[2][1],m.m[2][2],0.0f,
		0.0f, 0.0f, 0.0f, 0.0f
	};
}

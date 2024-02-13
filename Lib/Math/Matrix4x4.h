#pragma once

#include <Vector3.h>

class Matrix4x4 final {
public:

	Matrix4x4() = default;

	Matrix4x4(
		float aa, float ab, float ac, float ad,
		float ba, float bb, float bc, float bd,
		float ca, float cb, float cc, float cd,
		float da, float db, float dc, float dd
	);


private:

	float m_[4][4];

public:

	static Matrix4x4 MakeIdentity();

	static Matrix4x4 MakeScale(const Vec3f& scale);

	static Matrix4x4 MakeRotateX(float theta);
	static Matrix4x4 MakeRotateY(float theta);
	static Matrix4x4 MakeRotateZ(float theta);
	static Matrix4x4 MakeRotate(const Vec3f& rotate);

	static Matrix4x4 MakeTranslate(const Vec3f& translate);

	static Matrix4x4 MakeAffine(const Vec3f& scale, const Vec3f& rotate, const Vec3f& translate);

public:

	inline Matrix4x4 operator+ (const Matrix4x4& other) const;
	inline Matrix4x4 operator- (const Matrix4x4& other) const;
	inline Matrix4x4 operator* (const Matrix4x4& other) const;

	inline Matrix4x4& operator*= (const Matrix4x4& other);
	
};

//inline float* Matrix4x4::operator[](int index) const {
//	return m_[index];
//}

//inline float* Matrix4x4::operator[](int index) {
//	return m_[index];
//}


inline Matrix4x4 Matrix4x4::operator+(const Matrix4x4& other) const {
	Matrix4x4 result;
	for (int r = 0; r < 4; r++) {
		for (int c = 0; c < 4; c++) {
			result.m_[r][c] = this->m_[r][c] + other.m_[r][c];
		}
	}
	return result;
}

inline Matrix4x4 Matrix4x4::operator-(const Matrix4x4& other) const {
	Matrix4x4 result;
	for (int r = 0; r < 4; r++) {
		for (int c = 0; c < 4; c++) {
			result.m_[r][c] = this->m_[r][c] - other.m_[r][c];
		}
	}
	return result;
}

inline Matrix4x4 Matrix4x4::operator*(const Matrix4x4& other) const {
	Matrix4x4 result;
	for (int r = 0; r < 4; r++) {
		for (int c = 0; c < 4; c++) {
			result.m_[r][c] =
				this->m_[r][0] * other.m_[0][c]
				+ this->m_[r][1] * other.m_[1][c]
				+ this->m_[r][2] * other.m_[2][c]
				+ this->m_[r][3] * other.m_[3][c];
		}
	}
	return result;
}

inline Matrix4x4& Matrix4x4::operator*=(const Matrix4x4& other) {
	Matrix4x4 result;
	for (int r = 0; r < 4; r++) {
		for (int c = 0; c < 4; c++) {
			result.m_[r][c] =
				this->m_[r][0] * other.m_[0][c]
				+ this->m_[r][1] * other.m_[1][c]
				+ this->m_[r][2] * other.m_[2][c]
				+ this->m_[r][3] * other.m_[3][c];
		}
	}

	*this = result;
	return *this;
}

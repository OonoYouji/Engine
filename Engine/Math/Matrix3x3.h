#pragma once 

#include <Vector2.h>

/// <summary>
/// 3x3行列
/// </summary>
class Matrix3x3 final {
public:

	Matrix3x3() = default;
	Matrix3x3(
		float aa, float ab, float ac,
		float ba, float bb, float bc,
		float ca, float cb, float cc
	);

public:

	float m[3][3];

public:

	static Matrix3x3 MakeIdentity();

	static Matrix3x3 MakeScale(const Vec2f& scale);
	static Matrix3x3 MakeRotate(float rotate);
	static Matrix3x3 MakeTranslate(const Vec2f& translate);

	static Matrix3x3 MakeAffine(const Vec2f& scale, float rotate, const Vec2f& translate);


	inline Matrix3x3 operator* (const Matrix3x3& other) const;
	inline Matrix3x3& operator*= (const Matrix3x3& other);

};


inline Matrix3x3 Matrix3x3::operator*(const Matrix3x3& other) const {
	Matrix3x3 result{};
	for(int r = 0; r < 3; ++r) {
		for(int c = 0; c < 3; ++c) {
			for(int i = 0; i < 3; ++i) {
				result.m[r][c] += this->m[r][i] * other.m[i][c];
			}
		}
	}

	return result;
}

inline Matrix3x3& Matrix3x3::operator*=(const Matrix3x3& other) {
	(*this) = (*this) * other;
	return *this;
}


using Mat3 = Matrix3x3;
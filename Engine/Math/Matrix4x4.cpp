#include <Matrix4x4.h>

#include <cmath>
#include <cassert>


namespace {

	float Cot(float t) {
		return 1.0f / std::tanf(t);
	}

} /// namespace

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

Matrix4x4 Matrix4x4::MakeInverse(const Matrix4x4& m) {
	Matrix4x4 result = MakeIdentity();

	float a =
		(m.m_[0][0] * m.m_[1][1] * m.m_[2][2] * m.m_[3][3])
		+ (m.m_[0][0] * m.m_[1][2] * m.m_[2][3] * m.m_[3][1])
		+ (m.m_[0][0] * m.m_[1][3] * m.m_[2][1] * m.m_[3][2])

		- (m.m_[0][0] * m.m_[1][3] * m.m_[2][2] * m.m_[3][1])
		- (m.m_[0][0] * m.m_[1][2] * m.m_[2][1] * m.m_[3][3])
		- (m.m_[0][0] * m.m_[1][1] * m.m_[2][3] * m.m_[3][2])

		- (m.m_[0][1] * m.m_[1][0] * m.m_[2][2] * m.m_[3][3])
		- (m.m_[0][2] * m.m_[1][0] * m.m_[2][3] * m.m_[3][1])
		- (m.m_[0][3] * m.m_[1][0] * m.m_[2][1] * m.m_[3][2])

		+ (m.m_[0][3] * m.m_[1][0] * m.m_[2][2] * m.m_[3][1])
		+ (m.m_[0][2] * m.m_[1][0] * m.m_[2][1] * m.m_[3][3])
		+ (m.m_[0][1] * m.m_[1][0] * m.m_[2][3] * m.m_[3][2])

		+ (m.m_[0][1] * m.m_[1][2] * m.m_[2][0] * m.m_[3][3])
		+ (m.m_[0][2] * m.m_[1][3] * m.m_[2][0] * m.m_[3][1])
		+ (m.m_[0][3] * m.m_[1][1] * m.m_[2][0] * m.m_[3][2])

		- (m.m_[0][3] * m.m_[1][2] * m.m_[2][0] * m.m_[3][1])
		- (m.m_[0][2] * m.m_[1][1] * m.m_[2][0] * m.m_[3][3])
		- (m.m_[0][1] * m.m_[1][3] * m.m_[2][0] * m.m_[3][2])

		- (m.m_[0][1] * m.m_[1][2] * m.m_[2][3] * m.m_[3][0])
		- (m.m_[0][2] * m.m_[1][3] * m.m_[2][1] * m.m_[3][0])
		- (m.m_[0][3] * m.m_[1][1] * m.m_[2][2] * m.m_[3][0])

		+ (m.m_[0][3] * m.m_[1][2] * m.m_[2][1] * m.m_[3][0])
		+ (m.m_[0][2] * m.m_[1][1] * m.m_[2][3] * m.m_[3][0])
		+ (m.m_[0][1] * m.m_[1][3] * m.m_[2][2] * m.m_[3][0]);


	result.m_[0][0] =
		(m.m_[1][1] * m.m_[2][2] * m.m_[3][3]
		 + m.m_[1][2] * m.m_[2][3] * m.m_[3][1]
		 + m.m_[1][3] * m.m_[2][1] * m.m_[3][2]
		 - m.m_[1][3] * m.m_[2][2] * m.m_[3][1]
		 - m.m_[1][2] * m.m_[2][1] * m.m_[3][3]
		 - m.m_[1][1] * m.m_[2][3] * m.m_[3][2]);

	result.m_[0][1] =
		(-m.m_[0][1] * m.m_[2][2] * m.m_[3][3]
		 - m.m_[0][2] * m.m_[2][3] * m.m_[3][1]
		 - m.m_[0][3] * m.m_[2][1] * m.m_[3][2]
		 + m.m_[0][3] * m.m_[2][2] * m.m_[3][1]
		 + m.m_[0][2] * m.m_[2][1] * m.m_[3][3]
		 + m.m_[0][1] * m.m_[2][3] * m.m_[3][2]);

	result.m_[0][2] =
		(m.m_[0][1] * m.m_[1][2] * m.m_[3][3]
		 + m.m_[0][2] * m.m_[1][3] * m.m_[3][1]
		 + m.m_[0][3] * m.m_[1][1] * m.m_[3][2]
		 - m.m_[0][3] * m.m_[1][2] * m.m_[3][1]
		 - m.m_[0][2] * m.m_[1][1] * m.m_[3][3]
		 - m.m_[0][1] * m.m_[1][3] * m.m_[3][2]);

	result.m_[0][3] =
		(-m.m_[0][1] * m.m_[1][2] * m.m_[2][3]
		 - m.m_[0][2] * m.m_[1][3] * m.m_[2][1]
		 - m.m_[0][3] * m.m_[1][1] * m.m_[2][2]
		 + m.m_[0][3] * m.m_[1][2] * m.m_[2][1]
		 + m.m_[0][2] * m.m_[1][1] * m.m_[2][3]
		 + m.m_[0][1] * m.m_[1][3] * m.m_[2][2]);



	result.m_[1][0] =
		(-m.m_[1][0] * m.m_[2][2] * m.m_[3][3]
		 - m.m_[1][2] * m.m_[2][3] * m.m_[3][0]
		 - m.m_[1][3] * m.m_[2][0] * m.m_[3][2]
		 + m.m_[1][3] * m.m_[2][2] * m.m_[3][0]
		 + m.m_[1][2] * m.m_[2][0] * m.m_[3][3]
		 + m.m_[1][0] * m.m_[2][3] * m.m_[3][2]);

	result.m_[1][1] =
		(m.m_[0][0] * m.m_[2][2] * m.m_[3][3]
		 + m.m_[0][2] * m.m_[2][3] * m.m_[3][0]
		 + m.m_[0][3] * m.m_[2][0] * m.m_[3][2]
		 - m.m_[0][3] * m.m_[2][2] * m.m_[3][0]
		 - m.m_[0][2] * m.m_[2][0] * m.m_[3][3]
		 - m.m_[0][0] * m.m_[2][3] * m.m_[3][2]);

	result.m_[1][2] =
		(-(m.m_[0][0] * m.m_[1][2] * m.m_[3][3])
		 - m.m_[0][2] * m.m_[1][3] * m.m_[3][0]
		 - m.m_[0][3] * m.m_[1][0] * m.m_[3][2]
		 + m.m_[0][3] * m.m_[1][2] * m.m_[3][0]
		 + m.m_[0][2] * m.m_[1][0] * m.m_[3][3]
		 + m.m_[0][0] * m.m_[1][3] * m.m_[3][2]);

	result.m_[1][3] =
		(m.m_[0][0] * m.m_[1][2] * m.m_[2][3]
		 + m.m_[0][2] * m.m_[1][3] * m.m_[2][0]
		 + m.m_[0][3] * m.m_[1][0] * m.m_[2][2]
		 - m.m_[0][3] * m.m_[1][2] * m.m_[2][0]
		 - m.m_[0][2] * m.m_[1][0] * m.m_[2][3]
		 - m.m_[0][0] * m.m_[1][3] * m.m_[2][2]);



	result.m_[2][0] =
		(m.m_[1][0] * m.m_[2][1] * m.m_[3][3]
		 + m.m_[1][1] * m.m_[2][3] * m.m_[3][0]
		 + m.m_[1][3] * m.m_[2][0] * m.m_[3][1]
		 - m.m_[1][3] * m.m_[2][1] * m.m_[3][0]
		 - m.m_[1][1] * m.m_[2][0] * m.m_[3][3]
		 - m.m_[1][0] * m.m_[2][3] * m.m_[3][1]);

	result.m_[2][1] =
		(-m.m_[0][0] * m.m_[2][1] * m.m_[3][3]
		 - m.m_[0][1] * m.m_[2][3] * m.m_[3][0]
		 - m.m_[0][3] * m.m_[2][0] * m.m_[3][1]
		 + m.m_[0][3] * m.m_[2][1] * m.m_[3][0]
		 + m.m_[0][1] * m.m_[2][0] * m.m_[3][3]
		 + m.m_[0][0] * m.m_[2][3] * m.m_[3][1]);

	result.m_[2][2] =
		(m.m_[0][0] * m.m_[1][1] * m.m_[3][3]
		 + m.m_[0][1] * m.m_[1][3] * m.m_[3][0]
		 + m.m_[0][3] * m.m_[1][0] * m.m_[3][1]
		 - m.m_[0][3] * m.m_[1][1] * m.m_[3][0]
		 - m.m_[0][1] * m.m_[1][0] * m.m_[3][3]
		 - m.m_[0][0] * m.m_[1][3] * m.m_[3][1]);

	result.m_[2][3] =
		(-m.m_[0][0] * m.m_[1][1] * m.m_[2][3]
		 - m.m_[0][1] * m.m_[1][3] * m.m_[2][0]
		 - m.m_[0][3] * m.m_[1][0] * m.m_[2][1]
		 + m.m_[0][3] * m.m_[1][1] * m.m_[2][0]
		 + m.m_[0][1] * m.m_[1][0] * m.m_[2][3]
		 + m.m_[0][0] * m.m_[1][3] * m.m_[2][1]);



	result.m_[3][0] =
		(-m.m_[1][0] * m.m_[2][1] * m.m_[3][2]
		 - m.m_[1][1] * m.m_[2][2] * m.m_[3][0]
		 - m.m_[1][2] * m.m_[2][0] * m.m_[3][1]
		 + m.m_[1][2] * m.m_[2][1] * m.m_[3][0]
		 + m.m_[1][1] * m.m_[2][0] * m.m_[3][2]
		 + m.m_[1][0] * m.m_[2][2] * m.m_[3][1]);

	result.m_[3][1] =
		(m.m_[0][0] * m.m_[2][1] * m.m_[3][2]
		 + m.m_[0][1] * m.m_[2][2] * m.m_[3][0]
		 + m.m_[0][2] * m.m_[2][0] * m.m_[3][1]
		 - m.m_[0][2] * m.m_[2][1] * m.m_[3][0]
		 - m.m_[0][1] * m.m_[2][0] * m.m_[3][2]
		 - m.m_[0][0] * m.m_[2][2] * m.m_[3][1]);

	result.m_[3][2] =
		(-m.m_[0][0] * m.m_[1][1] * m.m_[3][2]
		 - m.m_[0][1] * m.m_[1][2] * m.m_[3][0]
		 - m.m_[0][2] * m.m_[1][0] * m.m_[3][1]
		 + m.m_[0][2] * m.m_[1][1] * m.m_[3][0]
		 + m.m_[0][1] * m.m_[1][0] * m.m_[3][2]
		 + m.m_[0][0] * m.m_[1][2] * m.m_[3][1]);

	result.m_[3][3] =
		(m.m_[0][0] * m.m_[1][1] * m.m_[2][2]
		 + m.m_[0][1] * m.m_[1][2] * m.m_[2][0]
		 + m.m_[0][2] * m.m_[1][0] * m.m_[2][1]
		 - m.m_[0][2] * m.m_[1][1] * m.m_[2][0]
		 - m.m_[0][1] * m.m_[1][0] * m.m_[2][2]
		 - m.m_[0][0] * m.m_[1][2] * m.m_[2][1]);

	float a2 = 1.0f / a;
	for(uint32_t row = 0; row < 4; row++) {
		for(uint32_t col = 0; col < 4; col++) {
			result.m_[row][col] *= a2;
		}
	}


	return result;

}

Matrix4x4 Matrix4x4::MakeTranspose(const Matrix4x4& m) {
	Matrix4x4 result;
	for(int r = 0; r < 4; r++) {
		for(int c = 0; c < 4; c++) {
			result.m_[r][c] = m.m_[c][r];
		}
	}
	return result;
}

Vec3f Matrix4x4::Transform(const Vec3f& v, const Matrix4x4& m) {
	//w=1がデカルト座標系であるので(x,y,1)のベクトルとしてmatrixとの積をとる
	Vec3f result{ 0.0f,0.0f,0.0f };

	result.x = v.x * m.m_[0][0] + v.y * m.m_[1][0] + v.z * m.m_[2][0] + 1.0f * m.m_[3][0];
	result.y = v.x * m.m_[0][1] + v.y * m.m_[1][1] + v.z * m.m_[2][1] + 1.0f * m.m_[3][1];
	result.z = v.x * m.m_[0][2] + v.y * m.m_[1][2] + v.z * m.m_[2][2] + 1.0f * m.m_[3][2];
	float w = v.x * m.m_[0][3] + v.y * m.m_[1][3] + v.z * m.m_[2][3] + 1.0f * m.m_[3][3];

	//ベクトルに対して基本的な操作を行う行列でwが0になることはありえない
	//wが0.0fになった場合プログラムを停止する
	assert(w != 0.0f);

	//w=1がデカルト座標系であるので、w除算することで同次座標をデカルト座標に戻す
	result.x /= w;
	result.y /= w;
	result.z /= w;

	return result;
}

Vec3f Matrix4x4::Transform(const Vec4f& v, const Matrix4x4& m) {
	//w=1がデカルト座標系であるので(x,y,1)のベクトルとしてmatrixとの積をとる
	Vec3f result{ 0.0f,0.0f,0.0f };

	result.x = v.x * m.m_[0][0] + v.y * m.m_[1][0] + v.z * m.m_[2][0] + 1.0f * m.m_[3][0];
	result.y = v.x * m.m_[0][1] + v.y * m.m_[1][1] + v.z * m.m_[2][1] + 1.0f * m.m_[3][1];
	result.z = v.x * m.m_[0][2] + v.y * m.m_[1][2] + v.z * m.m_[2][2] + 1.0f * m.m_[3][2];
	float w = v.x * m.m_[0][3] + v.y * m.m_[1][3] + v.z * m.m_[2][3] + 1.0f * m.m_[3][3];

	//ベクトルに対して基本的な操作を行う行列でwが0になることはありえない
	//wが0.0fになった場合プログラムを停止する
	assert(w != 0.0f);

	//w=1がデカルト座標系であるので、w除算することで同次座標をデカルト座標に戻す
	result.x /= w;
	result.y /= w;
	result.z /= w;

	return result;
}

Vec3f Matrix4x4::TransformNormal(const Vec3f& v, const Matrix4x4& m) {
	Vec3f result{
		v.x * m.m_[0][0] + v.y * m.m_[1][0] + v.z * m.m_[2][0],
		v.x * m.m_[0][1] + v.y * m.m_[1][1] + v.z * m.m_[2][1],
		v.x * m.m_[0][2] + v.y * m.m_[1][2] + v.z * m.m_[2][2]
	};

	return result;
}



Matrix4x4 Matrix4x4::MakeOrthographicMatrix(float l, float t, float r, float b, float near, float far) {
	return Matrix4x4{
		2.0f / (r - l), 0.0f, 0.0f, 0.0f,
		0.0f, 2.0f / (t - b), 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f / (far - near), 0.0f,
		(l + r) / (l - r), (t + b) / (b - t), near / (near - far), 1.0f
	};
}

Matrix4x4 Matrix4x4::MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip) {
	return Matrix4x4{
		(1 / aspectRatio) * Cot(fovY / 2.0f), 0.0f, 0.0f, 0.0f,
		0.0f, Cot(fovY / 2.0f), 0.0f, 0.0f,
		0.0f, 0.0f, farClip / (farClip - nearClip), 1.0f,
		0.0f, 0.0f, (-nearClip * farClip) / (farClip - nearClip), 0.0f
	};
}

Matrix4x4 Matrix4x4::MakeViewportMatrix(float top, float left, float width, float height, float minD, float maxD) {
	return Matrix4x4{
		width / 2.0f, 0.0f, 0.0f, 0.0f,
		0.0f, -(height / 2.0f), 0.0f, 0.0f,
		0.0f, 0.0f, maxD - minD, 0.0f,
		left + (width / 2.0f), top + (height / 2.0f), minD, 1.0f
	};
}

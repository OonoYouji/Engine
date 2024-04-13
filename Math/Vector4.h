#pragma once

class Vector4 final {
public:

	Vector4() = default;
	Vector4(const Vector4& other);
	Vector4(Vector4&& other);
	Vector4(float x, float y, float z, float w);

	~Vector4() = default;

public:

	float x;
	float y;
	float z;
	float w;

public:

	/*-----------------------------------
		演算子オーバーロード
	-----------------------------------*/

	Vector4& operator= (const Vector4& other);

};



///- 名前の追加
using Vec4f = Vector4;
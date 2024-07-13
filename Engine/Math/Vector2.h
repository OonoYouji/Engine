#pragma once




class Vector2 {
public:

	Vector2() = default;
	Vector2(const Vector2& other);
	Vector2(Vector2&& other);
	Vector2(float x, float y);

	~Vector2() = default;

public:

	/// 2次元ベクトル
	float x, y;

public:

	inline Vector2 operator+ (const Vector2& other) const;
	inline Vector2 operator- (const Vector2& other) const;
	inline Vector2 operator/ (const Vector2& other) const;
	inline Vector2 operator* (const Vector2& other) const;
	inline Vector2 operator/ (float value) const;
	inline Vector2 operator* (float value) const;

	inline Vector2& operator+= (const Vector2& other);
	inline Vector2& operator-= (const Vector2& other);
	inline Vector2& operator/= (const Vector2& other);
	inline Vector2& operator*= (const Vector2& other);
	inline Vector2& operator/= (float value);
	inline Vector2& operator*= (float value);

	inline Vector2& operator= (const Vector2& other);

	inline bool operator!= (const Vector2& other);
	inline bool operator== (const Vector2& other);

};



/*=======================================================
	四則演算のオペレーター
=======================================================*/
inline Vector2 Vector2::operator+(const Vector2& other) const {
	return Vector2(
		this->x + other.x,
		this->y + other.y
	);
}
inline Vector2 Vector2::operator-(const Vector2& other) const {
	return Vector2(
		this->x - other.x,
		this->y - other.y
	);
}
inline Vector2 Vector2::operator/(const Vector2& other) const {
	return Vector2(
		this->x / other.x,
		this->y / other.y
	);
}
inline Vector2 Vector2::operator*(const Vector2& other) const {
	return Vector2(
		this->x * other.x,
		this->y * other.y
	);
}
inline Vector2 Vector2::operator/(float value) const {
	return Vector2(
		this->x / value,
		this->y / value
	);
}
inline Vector2 Vector2::operator*(float value) const {
	return Vector2(
		this->x * value,
		this->y * value
	);
}
inline Vector2& Vector2::operator+=(const Vector2& other) {
	this->x += other.x;
	this->y += other.y;
	return *this;
}
inline Vector2& Vector2::operator-=(const Vector2& other) {
	this->x -= other.x;
	this->y -= other.y;
	return *this;
}
inline Vector2& Vector2::operator/=(const Vector2& other) {
	this->x /= other.x;
	this->y /= other.y;
	return *this;
}
inline Vector2& Vector2::operator*=(const Vector2& other) {
	this->x *= other.x;
	this->y *= other.y;
	return *this;
}
inline Vector2& Vector2::operator/=(float value) {
	this->x /= value;
	this->y /= value;
	return *this;
}
inline Vector2& Vector2::operator*=(float value) {
	this->x *= value;
	this->y *= value;
	return *this;
}

inline Vector2& Vector2::operator=(const Vector2& other) {
	this->x = other.x;
	this->y = other.y;
	return *this;
}

inline bool Vector2::operator!=(const Vector2& other) {
	return this->x != other.x 
		|| this->y != other.y;
}

inline bool Vector2::operator==(const Vector2& other) {
	return !(*this != other);
}



class Vector2Int {
public:

	Vector2Int() = default;
	Vector2Int(const Vector2Int& other);
	Vector2Int(Vector2Int&& other);
	Vector2Int(int x, int y);

	~Vector2Int() = default;

public:

	/// 2次元ベクトル
	int x, y;

public:

	inline Vector2 castFloat();

public:

	inline Vector2Int operator+ (const Vector2Int& other) const;
	inline Vector2Int operator- (const Vector2Int& other) const;
	inline Vector2Int operator/ (const Vector2Int& other) const;
	inline Vector2Int operator* (const Vector2Int& other) const;
	inline Vector2Int operator/ (int value) const;
	inline Vector2Int operator* (int value) const;

	inline Vector2Int& operator+= (const Vector2Int& other);
	inline Vector2Int& operator-= (const Vector2Int& other);
	inline Vector2Int& operator/= (const Vector2Int& other);
	inline Vector2Int& operator*= (const Vector2Int& other);
	inline Vector2Int& operator/= (int value);
	inline Vector2Int& operator*= (int value);

};

inline Vector2 Vector2Int::castFloat() {
	return Vector2(
		static_cast<float>(this->x),
		static_cast<float>(this->y)
	);
}

/*=======================================================
	四則演算のオペレーター
=======================================================*/
inline Vector2Int Vector2Int::operator+(const Vector2Int& other) const {
	return Vector2Int(
		this->x + other.x,
		this->y + other.y
	);
}
inline Vector2Int Vector2Int::operator-(const Vector2Int& other) const {
	return Vector2Int(
		this->x - other.x,
		this->y - other.y
	);
}
inline Vector2Int Vector2Int::operator/(const Vector2Int& other) const {
	return Vector2Int(
		this->x / other.x,
		this->y / other.y
	);
}
inline Vector2Int Vector2Int::operator*(const Vector2Int& other) const {
	return Vector2Int(
		this->x * other.x,
		this->y * other.y
	);
}
inline Vector2Int Vector2Int::operator/(int value) const {
	return Vector2Int(
		this->x / value,
		this->y / value
	);
}
inline Vector2Int Vector2Int::operator*(int value) const {
	return Vector2Int(
		this->x * value,
		this->y * value
	);
}
inline Vector2Int& Vector2Int::operator+=(const Vector2Int& other) {
	this->x += other.x;
	this->y += other.y;
	return *this;
}
inline Vector2Int& Vector2Int::operator-=(const Vector2Int& other) {
	this->x -= other.x;
	this->y -= other.y;
	return *this;
}
inline Vector2Int& Vector2Int::operator/=(const Vector2Int& other) {
	this->x /= other.x;
	this->y /= other.y;
	return *this;
}
inline Vector2Int& Vector2Int::operator*=(const Vector2Int& other) {
	this->x *= other.x;
	this->y *= other.y;
	return *this;
}
inline Vector2Int& Vector2Int::operator/=(int value) {
	this->x /= value;
	this->y /= value;
	return *this;
}
inline Vector2Int& Vector2Int::operator*=(int value) {
	this->x *= value;
	this->y *= value;
	return *this;
}


///
/// Vector2クラスに他の名前を設定する
///
using Vec2f = Vector2;
using Vec2 = Vector2Int;

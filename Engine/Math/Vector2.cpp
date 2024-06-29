#include <Vector2.h>

Vector2::Vector2(const Vector2& other) {
	this->x = other.x;
	this->y = other.y;
}

Vector2::Vector2(Vector2&& other) {
	this->x = other.x;
	this->y = other.y;
}

Vector2::Vector2(float x, float y) {
	this->x = x;
	this->y = y;
}

Vector2Int::Vector2Int(const Vector2Int& other) {
	this->x = other.x;
	this->y = other.y;
}

Vector2Int::Vector2Int(Vector2Int&& other) {
	this->x = other.x;
	this->y = other.y;
}

Vector2Int::Vector2Int(int x, int y) {
	this->x = x;
	this->y = y;
}
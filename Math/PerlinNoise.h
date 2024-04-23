#pragma once

#include <array>
#include <cmath>

#include "Vector3.h"
#include "Vector2.h"


class  PerlinNoise {
public:

	PerlinNoise(uint32_t seed);
	~PerlinNoise();

	void Init();

	void Update();

private:

	std::array<std::array<float, 10>, 10> noises_;


	std::array<uint8_t, 512> p_;

	void SetSeed(uint32_t seed);


	float Fade(float t);

	float Lerp(float t, float a, float b);

	float Grad(int hash, float x, float y, float z);

	float Grad(int hash, float x, float y);

	float Noise(const Vec3f& vector);

	float Noise(const Vec2f& vector);

public:

	float GetNoise(const Vec3f& v);

	float GetNoise(const Vec2f& v);

};
#include "PerlinNoise.h"

#include <algorithm>
#include <random>

#include <ImGuiManager.h>



static const int permutation[] = { 151,160,137,91,90,15,
	131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
	190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
	88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
	77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
	102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
	135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,
	5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
	223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,
	129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
	251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,
	49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
	138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180
};



PerlinNoise::PerlinNoise(uint32_t seed) {
	SetSeed(seed);
}

PerlinNoise::~PerlinNoise() {}


void PerlinNoise::Init() {


}

void PerlinNoise::Update() {


}



void PerlinNoise::SetSeed([[maybe_unused]] uint32_t seed) {

	///- 0~255までの値を格納
	for(size_t i = 0; i < 256; i++) {
		p_[i] = static_cast<uint32_t>(permutation[i]);
	}

	///- p_の中身をランダムにシャッフルする
	std::shuffle(p_.begin(), p_.begin() + 256, std::default_random_engine(seed));

	///- 256以降に上記で生成した値をコピー
	for(size_t i = 0; i < 256; i++) {
		p_[256 + i] = p_[i];
	}


}



float PerlinNoise::Fade(float t) {
	return t * t * t * (t * (t * 6 - 15) + 10);
}


float PerlinNoise::Lerp(float t, float a, float b) {
	return a + t * (b - a);
}



float PerlinNoise::Grad(int hash, float x, float y, float z) {
	int h = hash & 15;
	float u = h < 8 ? x : y;
	float v = h < 4 ? y : h == 12 || h == 4 ? x : z;
	return ((h & 1) ? -u : u) + ((h & 2) ? -v : v);
}



float PerlinNoise::Grad(int hash, float x, float y) {
	int h = hash & 15;
	float u = h < 8 ? x : y;
	float v = h < 4 ? y : h == 12 || h == 14 ? x : y;
	return ((h & 1) ? -u : u) + ((h & 2) ? -v : v);
}



float PerlinNoise::Noise(const Vec3f& vector) {
	Vec3f V = vector;
	const int x = static_cast<int>(floor(V.x)) & 255;
	const int y = static_cast<int>(floor(V.y)) & 255;
	const int z = static_cast<int>(floor(V.z)) & 255;

	V.x -= floor(V.x);
	V.y -= floor(V.y);
	V.z -= floor(V.z);

	const float u = Fade(V.x);
	const float v = Fade(V.y);
	const float w = Fade(V.z);

	const int A = p_[x] + y;
	const int AA = p_[A] + x;
	const int AB = p_[A + 1] + z;
	const int B = p_[x + 1] + y;
	const int BA = p_[B] + z;
	const int BB = p_[B + 1] + z;

	return Lerp(w, Lerp(v, Lerp(u, Grad(p_[AA], V.x, V.y, V.z), Grad(p_[BA], V.x - 1, V.y, V.z)),
						Lerp(u, Grad(p_[AB], V.x, V.y - 1, V.z), Grad(p_[BB], V.x - 1, V.y - 1, V.z))),
				Lerp(v, Lerp(u, Grad(p_[AA + 1], V.x, V.y, V.z - 1), Grad(p_[BA + 1], V.x - 1, V.y, V.z - 1)),
					 Lerp(u, Grad(p_[AB + 1], V.x, V.y - 1, V.z - 1), Grad(p_[BB + 1], V.x - 1, V.y - 1, V.z - 1))));
}

float PerlinNoise::Noise(const Vec2f& vector) {
	Vec2f V = vector;
	int x = static_cast<int>(floor(vector.x)) & 255;
	int y = static_cast<int>(floor(vector.y)) & 255;

	V.x -= floor(V.x);
	V.y -= floor(V.y);

	float u = Fade(V.x);
	float v = Fade(V.y);

	int A = p_[x] + y;
	int AA = p_[A];
	int AB = p_[A + 1];
	int B = p_[x + 1] + y;
	int BA = p_[B];
	int BB = p_[B + 1];

	return Lerp(v, Lerp(u, Grad(p_[AA], V.x, V.y), Grad(p_[BA], V.x - 1, V.y)), Lerp(u, Grad(p_[AB], V.x, V.y - 1), Grad(p_[BB], V.x - 1, V.y - 1)));
}

float PerlinNoise::GetNoise(const Vec3f& v) {
	return Noise(v) * 0.5f + 0.5f;
}

float PerlinNoise::GetNoise(const Vec2f& v) {
	return Noise(v) * 0.5f + 0.5f;
}

void PerlinNoise::ResetSheed(uint32_t seed) {
	SetSeed(seed);
}



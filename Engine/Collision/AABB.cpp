#include "AABB.h"

#define NOMINMAX

#include <cmath>
#include <array>

#include <LineDrawer.h>


AABB::AABB() {}

AABB::~AABB() {}



/// ===================================================
/// 引数のpointでmin maxの再計算
/// ===================================================
void AABB::ExpandToFit(const Vec3f& point) {

	min.x = std::min(min.x, point.x);
	min.y = std::min(min.y, point.y);
	min.z = std::min(min.z, point.z);

	max.x = std::max(max.x, point.x);
	max.y = std::max(max.y, point.y);
	max.z = std::max(max.z, point.z);

}



/// ===================================================
/// AABBを描画
/// ===================================================
void AABB::Draw(const Vector4& color) {

	LineDrawer* lineDrawer = LineDrawer::GetInstance();

	///- 頂点の配列
	std::array<Vec3f, 8> points{
		/// 0 ~ 3 -> 奥
		Vec3f(min.x, max.y, max.z),	//- LT
		Vec3f(max.x, max.y, max.z),	//- RT
		Vec3f(max.x, min.y, max.z),	//- RB
		Vec3f(min.x, min.y, max.z),	//- LB
		/// 4 ~ 7 -> 手前
		Vec3f(min.x, max.y, min.z),	//- LT
		Vec3f(max.x, max.y, min.z),	//- RT
		Vec3f(max.x, min.y, min.z),	//- RB
		Vec3f(min.x, min.y, min.z)	//- LB
	};

	for(auto& point : points) {
		point += translation;
	}


	for(uint32_t i = 0; i < 4; ++i) {
		lineDrawer->Draw(points[i], points[(i + 1) % 4], color);
		uint32_t j = i + 4;
		lineDrawer->Draw(points[j], points[(j + 1) % 4 + 4], color);
		lineDrawer->Draw(points[i], points[j], color);
	}

}

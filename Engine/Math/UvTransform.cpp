#include "UvTransform.h"


/// ===================================================
/// 初期化
/// ===================================================
void UvTransform::Initilize() {
	scale = { 1.0f, 1.0f };
	rotate = 0.0f;
	tranalate = { 0.0f, 0.0f };
	UpdateMatrix();
}



/// ===================================================
/// 行列の更新
/// ===================================================
void UvTransform::UpdateMatrix() {
	matTransform = Mat4::MakeAffine(
		{ scale.x, scale.y, 1.0f },
		{ 0.0f, 0.0f ,rotate },
		{ tranalate.x, tranalate.y, 0.0f }
	);
}

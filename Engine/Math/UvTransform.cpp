#include "UvTransform.h"

#include <Matrix3x3.h>

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
	Mat3 matrix = Mat3::MakeAffine(scale, rotate, tranalate);
	matTransform = Mat3::CopyMatrix(matrix);
}

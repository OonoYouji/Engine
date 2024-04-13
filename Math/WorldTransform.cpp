#include <WorldTransform.h>





/// -------------------------
///	初期化関数
/// -------------------------
void WorldTransform::Init() {

	scale = { 1.0f,1.0f,1.0f };
	rotate = { 0.0f,0.0f,0.0f };
	translate = { 0.0f,0.0f,0.0f };
	MakeWorldMatrix();
}



/// -------------------------
/// world行列の生成
/// -------------------------
void WorldTransform::MakeWorldMatrix() {
	worldMatrix = Mat4::MakeAffine(scale, rotate, translate);
}

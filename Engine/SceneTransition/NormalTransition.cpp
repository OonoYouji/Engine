#include "NormalTransition.h"

// ----- Include ----- //

// Math -----

// Constructor & Destructor
NormalTransition::NormalTransition() { Init(); }
NormalTransition::~NormalTransition() { Finalize(); }


/*===========================================
	初期化関数
===========================================*/
void NormalTransition::Init() {

	// 継承した変数 ------------

	/*inFrame_ = 30;
	outFrame_ = 60;

	inFrameLimit_ = static_cast<float>(inFrame_);
	outFrameLimit_ = static_cast<float>(outFrame_);*/

	isReturn_ = false;
	preIsReturn_ = isReturn_;

	isEnd_ = false;
	// ----------------------


	// このクラスで宣言した変数 ↓↓↓

	color_ = 0x00000000;

}



/*===========================================
	更新処理関数
===========================================*/
void NormalTransition::Update() {
	//// 前フレームの状態を保存
	//preIsReturn_ = isReturn_;

	//// シーンの遷移始まり
	//if (inFrame_ > 0) {

	//	// デクリメント
	//	inFrame_--;
	//
	//	color_ = MyMath::ShiftColor(
	//		inFrame_ / 30.0f,
	//		0x000000ff,
	//		0x00000000
	//	);

	//	if (inFrame_ <= 0) { isReturn_ = true; }

	//} else { // 折り返し -----

	//	// シーン遷移の終わり -----
	//	if (outFrame_ > 0) {

	//		// デクリメント
	//		outFrame_--;

	//		color_ = MyMath::ShiftColor(
	//			outFrame_ / 60.0f,
	//			0x00000000,
	//			0x000000ff
	//		);

	//	} else {

	//		// ここに入ったら終了 -----
	//		isEnd_ = true;
	//	}

	//}


}


/*===========================================
	描画処理関数
===========================================*/
void NormalTransition::Draw() {

	/*NovicePlus::DrawBox(
		{ 640.0f,360.0f },
		{ 1280.0f, 720.0f },
		0.0f,
		color_
	);*/

}


/*===========================================
	終了処理関数
===========================================*/
void NormalTransition::Finalize() {}

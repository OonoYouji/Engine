#include "GameManager.h"


#include <Scene_Game.h>


/*----------------------------------------
	static member object Initialize
----------------------------------------*/
std::deque<IScene*> GameManager::scene_;
ITransition* GameManager::transition_ = nullptr;
int GameManager::choiceAround_ = 0;
bool GameManager::isKeep_ = false;
bool GameManager::isFinished_ = false;

// Constructor & Destructor -----
GameManager::GameManager() { Init(); }
GameManager::~GameManager() { Finalize(); }



/*===========================================
	初期化関数
===========================================*/
void GameManager::Init() {

	scene_.push_back(new Scene_Game());

}


/*===========================================
	更新処理関数
===========================================*/
void GameManager::Update() {


	if (transition_) {
		TransitionUpdate();
		return;
	}

	// シーンの更新;
	if (!scene_.empty()) {
		scene_[choiceAround_]->Update();
	}
}


/*===========================================
	描画処理関数
===========================================*/
void GameManager::Draw() {

	// シーンの描画;

	if (!scene_.empty()) {
		scene_[choiceAround_]->Draw();
	}

	if (transition_) {
		transition_->Draw();
	}


}


/*===========================================
	終了処理関数
===========================================*/
void GameManager::Finalize() {
	scene_.clear();
}


/*===========================================
	その他メンバ関数
===========================================*/

/// <summary>
/// ゲームループ
/// </summary>
void GameManager::Run(void) {

	// ウィンドウの×ボタンが押されるまでループ
	while (!Engine::ProcessMessage()) {
		// フレームの開始
		Engine::BeginFrame();

		/// ↓更新処理ここから
		GameManager::Update();

		/// ↓描画処理ここから
		GameManager::Draw();

		// フレームの終了
		Engine::EndFrame();

		if (isFinished_) {
			break;
		}
	}
}

void GameManager::SetNextScene(IScene* next, bool isKeep, ITransition* transition) {

	// 遷移を一度消し、
	if (transition_) { SafeDelete(transition_); }
	transition_ = transition;

	isKeep_ = isKeep;
	if (isKeep) {
		/// キープするのでbackのsceneを
		choiceAround_ = 1;
	} else {
		/// キープしないのでfrontのsceneを
		choiceAround_ = 0;
	}

	// 次のシーンを格納;
	scene_.push_back(next);

	if (!isKeep_ && !transition_) {
		while (scene_.size() > 1) {
			scene_.front()->Finalize();
			scene_.pop_front();
		}
	}

}

void GameManager::TransitionUpdate(void) {

	transition_->Update();

	if (transition_->GetTriggerIsReturn()) {
		// frontのsceneに変更
		if (isKeep_) {
			choiceAround_ = 1;

			// キープできるシーンは2つまで;
			while (scene_.size() > 2) {
				scene_.front()->Finalize();
				scene_.pop_front();
			}

			scene_.back()->Init();

		} else {
			choiceAround_ = 0;

			// いらないsceneをpopする;
			while (scene_.size() > 1) {
				scene_.front()->Finalize();
				scene_.pop_front();
			}

			scene_.front()->Init();
		}
	}

	// 遷移が終わったら消去
	if (transition_->GetIsEnd()) {
		SafeDelete(transition_);
		return;
	}

}

void GameManager::Pop_Back() {
	// いらないsceneをpopする;
	while (scene_.size() > 1) {
		scene_.back()->Finalize();
		scene_.pop_back();
	}

	choiceAround_ = 0;
}



#include "GameManager.h"

#include <Scene_Game.h>
#include <memory>

#include <FrameTimer.h>


namespace {

	template<class T>
	void SafeDelete(T* t) {
		delete t;
		t = nullptr;
	}



	DirectXCommon* dxCommon = DirectXCommon::GetInstance();





	/// <summary>
	/// ゲームループなどやシーンの管理
	/// </summary>
	class GameManagerSystem {
		friend class GameManager;
	public:

		GameManagerSystem() = default;
		~GameManagerSystem();

		void Init(IScene* scene);
		void Update();
		void Draw();
		void Finalize();

	private:

		std::deque<IScene*> scene_;
		ITransition* transition_;

		bool isKeep_;
		bool isFinished_;

	public:

		/// <summary>
		/// ゲームループ
		/// </summary>
		void Run();

		/// <summary>
		/// 遷移の更新処理
		/// </summary>
		void TransitionUpdate();

		/// <summary>
		/// 次のシーンと遷移をセットする
		/// </summary>
		void SetNextScene(IScene* next, bool isKeep, ITransition* transition);

		/// <summary>
		/// sceneをpopする
		/// </summary>
		/// <param name="index">残したいsceneの数</param>
		void PopBack(int index);

		void PopFront(int index);

		void PopScene(PopAround around, int index);

		IScene* GetScene() const;

	};

	GameManagerSystem::~GameManagerSystem() { Finalize(); }

	/// <summary>
	/// 初期化
	/// </summary>
	void GameManagerSystem::Init(IScene* scene) {
		scene_.push_back(scene);
		transition_ = nullptr;

		isKeep_ = false;
		isFinished_ = false;
	}

	/// <summary>
	/// 更新処理
	/// </summary>
	void GameManagerSystem::Update() {

		/// 遷移処理があるのみ更新を行う
		if(transition_) {
			TransitionUpdate();
			return;
		}

		/// シーンの更新;
		if(!scene_.empty()) { scene_.front()->Update(); }

	}

	/// <summary>
	/// 描画処理
	/// </summary>
	void GameManagerSystem::Draw() {

		/// シーンの描画処理; 遷移の描画処理
		if(!scene_.empty()) {

			auto& scene = scene_.front();


			scene->BeginRenderTarget(IScene::kBack);
			scene->BackDraw();
			scene->EndRenderTarget(IScene::kBack);
			dxCommon->ClearDepthBuffer();


			scene->BeginRenderTarget(IScene::k3dObject);
			scene->ObjectDraw();
			scene->EndRenderTarget(IScene::k3dObject);
			dxCommon->ClearDepthBuffer();


			scene->BeginRenderTarget(IScene::kFront);
			scene->FrontDraw();
			scene->EndRenderTarget(IScene::kFront);
			dxCommon->ClearDepthBuffer();

		}


		if(transition_) { transition_->Draw(); }

	}

	/// <summary>
	/// 終了処理
	/// </summary>
	void GameManagerSystem::Finalize() {
		while(!scene_.empty()) {
			scene_.pop_front();
		}
		SafeDelete(transition_);
	}

	void GameManagerSystem::Run() {
		// ウィンドウの×ボタンが押されるまでループ
		while(!Engine::ProcessMessage()) {
			// フレームの開始
			FrameTimer::GetInstance()->Begin();
			Engine::BeginFrame();

			/// ↓更新処理ここから
			this->Update();

			Engine::PreDraw();

			/// ↓描画処理ここから
			this->Draw();

			// フレームの終了
			FrameTimer::GetInstance()->End();
			Engine::EndFrame();

			if(isFinished_) {
				break;
			}
		}
	}

	void GameManagerSystem::TransitionUpdate() {

		/// 遷移の更新
		transition_->Update();

		if(transition_->GetTriggerIsReturn()) {
			// frontのsceneに変更
			if(isKeep_) {
				/// 現在のsceneとkeepするsceneの2つ
				PopBack(2);
			} else {
				PopBack(1);
			}
			scene_.front()->Init();
		}

		// 遷移が終わったら消去
		if(transition_->GetIsEnd()) {
			SafeDelete(transition_);
			return;
		}
	}

	void GameManagerSystem::SetNextScene(IScene* next, bool isKeep, ITransition* transition) {

		/// 引数の遷移に変更
		if(transition_) { SafeDelete(transition_); }
		transition_ = transition;

		isKeep_ = isKeep;
		scene_.push_front(next);

		if(!isKeep_ && !transition_) {
			PopBack(1);
		}

	}

	void GameManagerSystem::PopBack(int index) {
		while(scene_.size() > index) {
			scene_.pop_back();
		}
	}

	void GameManagerSystem::PopFront(int index) {
		while(scene_.size() > index) {
			scene_.pop_front();
		}
	}

	void GameManagerSystem::PopScene(PopAround around, int index) {
		if(around == kBack) {
			PopBack(index);
		} else {
			PopFront(index);
		}
	}

	IScene* GameManagerSystem::GetScene() const {
		return scene_.front();
	}

	std::unique_ptr<GameManagerSystem> sGameManagerSystem_;

} /// namespace







GameManager::GameManager() { Init(); }
GameManager::~GameManager() { Finalize(); }

void GameManager::Init() {
	sGameManagerSystem_ = std::make_unique<GameManagerSystem>();
	sGameManagerSystem_->Init(new Scene_Game());
	//Epm::GetInstance()->Initialize();
}

void GameManager::Finalize() {
	sGameManagerSystem_.reset();
}

void GameManager::Run(void) {
	sGameManagerSystem_->Run();
}

void GameManager::SetNextScene(IScene* next, bool isKeep, ITransition* transition) {
	sGameManagerSystem_->SetNextScene(next, isKeep, transition);
}

void GameManager::PopScene(PopAround around, int index) {
	sGameManagerSystem_->PopScene(around, index);
}

void GameManager::SetIsFinished(bool isFinished) {
	sGameManagerSystem_->isFinished_ = isFinished;
}

IScene* GameManager::GetScene() {
	return sGameManagerSystem_->GetScene();
}



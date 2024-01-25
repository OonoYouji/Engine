#pragma once

#include <deque>
#include <Engine.h>

// Scenes -----
#include <IScene.h>
#include <ITransition.h>

class GameManager final {
public: // member method

	// Constructor & Destructor
	GameManager();
	~GameManager();

	// default method
	static void Init();
	static void Update();
	static void Draw();
	static void Finalize();

private: // member object

	static std::deque<IScene*> scene_;
	static ITransition* transition_;

	static int choiceAround_;
	static bool isKeep_;

	static bool isFinished_;

public: // user method

	/// <summary>
	/// ゲームループ
	/// </summary>
	static void Run(void);

	/// <summary>
	/// 次のシーンを決める関数
	/// </summary>
	/// <param name="next"> 次のscene </param>
	/// <param name="isKeep"> 今のsceneをキープするか </param>
	/// <param name="transition"> sceneの遷移 </param>
	static void SetNextScene(IScene* next, bool isKeep, ITransition* transition);

	static void TransitionUpdate(void);

	static IScene* GetFrontScene(void) { return scene_.front(); }

	static void Pop_Back();

	/// <summary>
	/// ここにTrueをセットするとゲームループを抜ける
	/// </summary>
	static void SetIsFinished(bool isFinished) {
		isFinished_ = isFinished;
	}

};


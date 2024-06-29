#pragma once

#include <deque>
#include <Engine.h>

// Scenes -----
#include <IScene.h>
#include <ITransition.h>

enum PopAround {
	kFront,
	kBack,
};

class GameManager final {
public: // member method

	// Constructor & Destructor
	GameManager();
	~GameManager();

	// default method
	static void Init();
	static void Finalize();

public: // user method

	/// <summary>
	/// ゲームループ
	/// </summary>
	static void Run(void);

	/// <summary>
	/// 次のシーンを決める関数
	/// </summary>
	/// <param name="next">		次のscene				</param>
	/// <param name="isKeep">		今のsceneをキープするか	</param>
	/// <param name="transition">	sceneの遷移			</param>
	static void SetNextScene(IScene* next, bool isKeep, ITransition* transition);

	/// <summary>
	/// 
	/// </summary>
	static void PopScene(PopAround around, int index);

	/// <summary>
	/// ここにTrueをセットするとゲームループを抜ける
	/// </summary>
	static void SetIsFinished(bool isFinished);

};


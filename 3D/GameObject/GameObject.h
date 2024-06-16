#pragma once

#include <string>
#include <list>

#include <WorldTransform.h>


/// <summary>
/// ゲームオブジェクトの基底クラス
/// </summary>
class GameObject {
public:

	virtual ~GameObject() = default;

	/// <summary>
	/// 描画処理
	/// </summary>
	virtual void Draw() = 0;

public:

	/// <summary>
	/// タグのSetter
	/// </summary>
	/// <param name="tag"> : thisオブジェクトのタグ </param>
	void SetTag(const std::string& tag);

	/// <summary>
	/// ImGuiでのデバッグ表示
	/// </summary>
	void ImGuiDebug();

protected:

	std::string tag_;

	WorldTransform worldTransform_;

};



/// <summary>
/// GameObjectの管理用
/// </summary>
class GameObjectManager final {
public:

	/// <summary>
	/// インスタンスの取得
	/// </summary>
	/// <returns></returns>
	GameObjectManager* GetInstance();

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

	/// <summary>
	/// GameObjectクラスへのポインタを追加する
	/// </summary>
	/// <param name="gameObject">GameObjectへのポインタ</param>
	void AddGameObject(GameObject* gameObject);

	/// <summary>
	/// pGameObjectのクリア
	/// </summary>
	void ClearList();

private:

	/// <summary>
	/// ImGuiでのデバッグ表示
	/// </summary>
	void ImGuiDebug();

private:

	std::list<GameObject*> pGameObjects_;



private:
	GameObjectManager() = default;
	~GameObjectManager() = default;
	GameObjectManager(const GameObjectManager&) = delete;
	GameObjectManager& operator= (const GameObjectManager&) = delete;
};
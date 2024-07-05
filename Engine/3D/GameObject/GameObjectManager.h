#pragma once

#include <list>
#include <memory>

#include <GameObject.h>

/// <summary>
/// GameObjectの管理用
/// </summary>
class GameObjectManager final {
public:

	/// <summary>
	/// インスタンスの取得
	/// </summary>
	/// <returns></returns>
	static GameObjectManager* GetInstance();

	void Finalize();

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw();

	/// <summary>
	/// GameObjectクラスへのポインタを追加する
	/// </summary>
	/// <param name="gameObject">GameObjectへのポインタ</param>
	void AddGameObject(GameObject* gameObject);

	/// <summary>
	/// pGameObjectのクリア
	/// </summary>
	void ClearList();

	std::list<GameObject*> GetGameObjcetList() const;

	/// <summary>
	/// 引数のポインタがListないにあるか探索する
	/// </summary>
	/// <param name="key"></param>
	/// <returns>あったときはそのポインタ : ないときはnullptrを返す</returns>
	GameObject* Find(GameObject* key);
	GameObject* Find(const std::string& key);

private:

	/// <summary>
	/// ImGuiでのデバッグ表示
	/// </summary>
	void ImGuiDebug();

	void ImGuiSelectChilds(const std::list<GameObject*>& childs);

	void ImGuiCreateObejct3d();

	void ImGuiSaveFileAll();

	/// <summary>
	/// Object3d型のオブジェクトを追加
	/// </summary>
	/// <param name="modelKey"></param>
	void AddObject3d(const std::string& modelKey);

	void ImGuiParentSetting();

private:

	std::list<std::unique_ptr<GameObject>> gameObjects_;
	GameObject* selectObject_ = nullptr;

private:
	GameObjectManager() = default;
	~GameObjectManager() = default;
	GameObjectManager(const GameObjectManager&) = delete;
	GameObjectManager& operator= (const GameObjectManager&) = delete;
};
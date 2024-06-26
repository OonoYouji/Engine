#pragma once

#include <string>
#include <list>
#include <memory>

#include <WorldTransform.h>


/// <summary>
/// ゲームオブジェクトの基底クラス
/// </summary>
class GameObject {
public:

	GameObject();
	virtual ~GameObject() = default;

	/// <summary>
	/// 更新処理
	/// </summary>
	virtual void Update() = 0;

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
	/// タグのGetter
	/// </summary>
	/// <returns></returns>
	const std::string& GetTag() const;

	/// <summary>
	/// 名前のSetter
	/// </summary>
	/// <param name="name"></param>
	void SetName(const std::string& name);

	/// <summary>
	/// 名前のGetter
	/// </summary>
	/// <returns></returns>
	const std::string& GetName() const;

	/// <summary>
	/// ImGuiでのデバッグ表示
	/// </summary>
	virtual void ImGuiDebug();

	/// <summary>
	/// worldTransformのGetter
	/// </summary>
	/// <returns>worldTransformのconst参照</returns>
	const WorldTransform& GetWorldTransform() const;

	void SetPosition(const Vec3f& position);
	void SetPositionX(float x);
	void SetPositionY(float y);
	void SetPositionZ(float z);

	/// <summary>
	/// worldTransformの行列更新
	/// </summary>
	void UpdateMatrix();

public:

	bool isActive_;

protected:

	std::string tag_;
	std::string name_;


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

private:

	/// <summary>
	/// ImGuiでのデバッグ表示
	/// </summary>
	void ImGuiDebug();

private:

	std::list<std::unique_ptr<GameObject>> gameObjects_;
	GameObject* selectObject_ = nullptr;


private:
	GameObjectManager() = default;
	~GameObjectManager() = default;
	GameObjectManager(const GameObjectManager&) = delete;
	GameObjectManager& operator= (const GameObjectManager&) = delete;
};
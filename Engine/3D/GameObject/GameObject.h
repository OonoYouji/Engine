#pragma once

#include <string>
#include <list>
#include <memory>

#include <WorldTransform.h>
#include <ExternalParamManager.h>


/// <summary>
/// ゲームオブジェクトの基底クラス
/// </summary>
class GameObject {
public:

	GameObject();
	virtual ~GameObject() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新処理
	/// </summary>
	virtual void Update() = 0;

	/// <summary>
	/// 描画処理
	/// </summary>
	virtual void Draw() = 0;

public:

#pragma region 名前 タグ setter getter
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
#pragma endregion

#pragma region 親子関係


	/// <summary>
	/// 親の設定
	/// </summary>
	/// <param name="parent"></param>
	void SetParent(GameObject* parent);

	/// <summary>
	/// 親のポインタを取得する
	/// </summary>
	/// <returns></returns>
	GameObject* GetParent() const;

	/// <summary>
	/// 子供の追加
	/// </summary>
	/// <param name="child">追加するポインタ</param>
	/// <returns>成功 true : 失敗 false</returns>
	bool AddChild(GameObject* child);

	/// <summary>
	/// 子供の削除
	/// </summary>
	/// <param name="child">childsリストの要素</param>
	/// <returns>成功 true : 失敗 false</returns>
	bool SubChild(GameObject* child);

	/// <summary>
	/// Childのポインタを返す
	/// </summary>
	/// <returns></returns>
	const std::list<GameObject*>& GetChilds() const;

#pragma endregion



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

	/// <summary>
	/// 
	/// </summary>
	void CreatCategory();

public:

	bool isActive_ = true;

protected:

	std::string tag_;
	std::string name_;

	WorldTransform worldTransform_;

	///- 親子関係
	GameObject* parent_;
	std::list<GameObject*> childs_;

	Epm::Category* category_;
};


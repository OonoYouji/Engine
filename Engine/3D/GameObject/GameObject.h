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

#pragma region name tag setter getter
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

	/// <summary>
	/// 親の設定
	/// </summary>
	/// <param name="parent"></param>
	void SetParent(const GameObject* parent);

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

	bool isActive_ = true;

protected:

	std::string tag_;
	std::string name_;


	WorldTransform worldTransform_;
	const GameObject* parent_;

};


#pragma once

#include <string>
#include <list>
#include <memory>
#include <variant>
#include <unordered_map>

#include <WorldTransform.h>
#include <UvTransform.h>


/// <summary>
/// ゲームオブジェクトの基底クラス
/// </summary>
class GameObject {
protected:
#pragma region JSON保存,読み込み
	/// ---------------------------------------------------
	/// 変数一個当たりの情報
	/// ---------------------------------------------------
	struct Item final {
		using Pointer = std::variant<int*, float*, Vector3*, bool*, std::string*>;	///- ポインタ
		using Value = std::variant<int, float, Vector3, bool, std::string>;		///- 実体
		std::pair<Pointer, Value> variable;
	};

	/// ---------------------------------------------------
	/// Itemの集合
	/// ---------------------------------------------------
	struct Group final {
		/// <summary>
		/// 値のsetter
		/// </summary>
		/// <param name="key">: itemsへのkey</param>
		/// <param name="value">: itemsへセットする値</param>
		template<typename T>
		void SetPtr(const std::string& key, T* ptr);

		template<typename T>
		void SetValue(const std::string& key, const T& value);

		/// <summary>
		/// 値のGetter
		/// </summary>
		/// <param name="key"></param>
		/// <returns></returns>
		template<typename T>
		const T& GetItem(const std::string& key);

		/// <summary>
		/// デバッグ
		/// </summary>
		void ImGuiDebug();

		/// <summary>
		/// Itemの集合
		/// </summary>
		std::unordered_map<std::string, Item> items;
	};


#pragma endregion
public:

	GameObject();
	virtual ~GameObject() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	virtual void Initialize() = 0;

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
	void ImGuiDebug();

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
	void CreateObejct();
	void CreateObejct(const std::string& objectName);

	void CreateTransformGroup();

	/// <summary>
	/// 
	/// </summary>
	/// <param name="groupName"></param>
	Group& CreateGroup(const std::string& groupName);

	/// <summary>
	/// jsonに保存
	/// </summary>
	void SaveFile();

	/// <summary>
	/// jsonから読み込み
	/// </summary>
	/// <param name="key"></param>
	/// <param name="filePath"></param>
	void LoadFile(const std::string& key, const std::string& filePath);

public:

	bool isActive_ = true;

protected:

	std::string tag_;
	std::string name_;

	WorldTransform worldTransform_;

	///- 親子関係
	GameObject* parent_;
	std::list<GameObject*> childs_;

	///- jsonに保存するデータの集まり
	std::unordered_map<std::string, Group> groups_;

};


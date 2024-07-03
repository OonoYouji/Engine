#pragma once

#include <json.hpp>

#include <variant>
#include <string>
#include <list>
#include <unordered_map>

#include <Vector3.h>

using json = nlohmann::json;


/// ===================================================
/// External Parameter Manager
/// ===================================================
class Epm final {
public:

	/// ---------------------------------------------------
	/// 変数1個1個のこと
	/// ---------------------------------------------------
	struct Item final {
		using Ptr = std::variant<int*, float*, Vec3f*, bool*, std::string*>;
		using Value = std::variant<int, float, Vec3f, bool, std::string>;
		std::pair<Ptr, Value> variable; //- 変数
	};

	/// ---------------------------------------------------
	/// Itemの集まり
	/// ---------------------------------------------------
	struct Group final {

		template<typename T>
		void SetValue(const std::string& key, const T& value);

		template<typename T>
		void SetPtr(const std::string& key, T* ptr);

		std::unordered_map<std::string, Item> items; //- 変数の集まり
	};

	/// ---------------------------------------------------
	/// インスタンス
	/// ---------------------------------------------------
	struct Object final {

		Group& CreateGroup(const std::string& groupName);

		void ImGuiDebug();

		using Pair = std::pair<std::string, Group>;
		std::list<Pair> groups;
		std::string name;
	};

	/// ---------------------------------------------------
	/// クラス
	/// ---------------------------------------------------
	struct ObjectType final {

		Object* CreateObject(const std::string& objectName);

		std::unordered_map<std::string, Object> objects;
	};




	static Epm* GetInstance();

	ObjectType* CreateObjectType(const std::string& key);

private:



	std::unordered_map<std::string, ObjectType> datas_;

};

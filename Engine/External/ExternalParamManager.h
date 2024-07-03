#pragma once

#include <json.hpp>

#include <variant>
#include <string>
#include <list>
#include <unordered_map>

#include <Vector3.h>

using json = nlohmann::json;


/// ===================================================
/// 外部パラメータの管理クラス
/// ===================================================
class ExternalParamManager final {
private:
	ExternalParamManager() = default;
	~ExternalParamManager() = default;
public:


	/// ===================================================
	/// nested struct
	/// ===================================================


	/// ---------------------------------------------------
	/// 変数一個当たりの情報
	/// ---------------------------------------------------
	using Item_first = std::variant<int*, float*, Vector3*>;	///- ポインタ
	using Item_second = std::variant<int, float, Vector3>;		///- 実体
	using Item = std::pair<Item_first, Item_second>;

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

	/// ---------------------------------------------------
	/// Groupの集合
	/// ---------------------------------------------------
	struct Category final {
		/// <summary>
		/// Groupのセット
		/// </summary>
		/// <param name="key"></param>
		Group& CraeteGroup(const std::string& key);

		/// <summary>
		/// Groupのゲット
		/// </summary>
		/// <param name="key"></param>
		/// <returns></returns>
		Group& GetGroup(const std::string& key);

		/// <summary>
		/// ファイルに書き出し
		/// </summary>
		void SaveFile(json& root, const std::string& categoryName);

		/// <summary>
		/// デバッグ
		/// </summary>
		void ImGuiDebug();

	private:
		/// <summary>
		/// Groupの集合
		/// </summary>
		using pair = std::pair<std::string, Group>;
		std::list<pair> groups;
	};



	/// ===================================================
	/// member methos : public
	/// ===================================================

	/// <summary>
	/// インスタンス確保
	/// </summary>
	/// <returns></returns>
	static ExternalParamManager* GetInstance();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// グループの生成
	/// </summary>
	/// <param name="key"></param>
	Category& CreateCategory(const std::string& key);

	/// <summary>
	/// カテゴリーのゲット
	/// </summary>
	/// <param name="key"></param>
	/// <returns></returns>
	Category& GetCategory(const std::string& key);

	/// <summary>
	/// ファイルに書き出す
	/// </summary>
	/// <param name="category"></param>
	void SaveFile(const std::string& categoryName);

	/// <summary>
	/// ファイルから読み込む
	/// </summary>
	/// <param name="filePath"></param>
	void LoadFile(const std::string& categoryName);

	/// <summary>
	/// すべてのファイルを読み込む
	/// </summary>
	void LoadFiles();

private:

	/// ===================================================
	/// member objects : private
	/// ===================================================

	///- 
	static const std::string kDirectoryPath_;

	///- データの集まり
	std::unordered_map<std::string, Category> datas_;


private:
	ExternalParamManager& operator=(const ExternalParamManager&) = delete;
	ExternalParamManager(const ExternalParamManager&) = delete;
	ExternalParamManager(ExternalParamManager&&) = delete;
};



/// ---------------------------------------------------
/// 別名を設定
/// ---------------------------------------------------
using Epm = ExternalParamManager;



////////////////////////////////////////////////////////////////////////////
///
/// item   member methods
///
////////////////////////////////////////////////////////////////////////////


/// ===================================================
/// Epm::Group::pointerのsetter
/// ===================================================
template<typename T>
inline void ExternalParamManager::Group::SetPtr(const std::string& key, T* ptr) {

	auto it = items.find(key);
	if(it != items.end()) { //- あったら

		///- ptrを設定してptrに今はいっている値を代入する
		Item_first& first = items.at(key).first;
		Item_second& second = items.at(key).second;

		if(std::is_same_v<T, int>
		   || std::is_same_v<T, float>
		   || std::is_same_v<T, Vector3>) {

			first = ptr; // ポインタをセット
			*ptr = std::get<T>(second); // 値をポインタに代入
		}

	} else { //- なかったら

		if(std::is_same_v<T, int>
		   || std::is_same_v<T, float>
		   || std::is_same_v<T, Vector3>) {

			items[key] = std::make_pair(ptr, *ptr);
		}

	}
}


/// ===================================================
/// Epm::Group::値のsetter
/// ===================================================
template<typename T>
inline void ExternalParamManager::Group::SetValue(const std::string& key, const T& value) {
	///- なかったら
	if(items.find(key) == items.end()) {
		items[key] = std::make_pair(static_cast<T*>(nullptr), value);
	}

	///- あったら
	const Item_second& second = items.at(key).second;
	if(std::holds_alternative<T>(second)) {
		//static_cast<std::decay_t<decltype(&value)>>(nullptr)
		//std::get<T>(second) = value;
		Item& item = items.at(key);
		item.second = static_cast<T>(value);
	}
}


/// ===================================================
/// Epm::Group::値のgetter
/// ===================================================
template<typename T>
inline const T& Epm::Group::GetItem(const std::string& key) {
	if (std::is_same_v<T, int> || std::is_same_v<T, float> || std::is_same_v<T, Vector3>) {
		return std::get<T>(items.at(key).second);
	}
}

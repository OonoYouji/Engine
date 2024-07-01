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
	using Item = std::variant<int*, float*, Vector3*>;

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
		void SetValue(const std::string& key, T* value);
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
/// Epm::Group::値のsetter
/// ===================================================
template<typename T>
inline void Epm::Group::SetValue(const std::string& key, T* value) {
	items[key] = value;
}


/// ===================================================
/// Epm::Group::値のgetter
/// ===================================================
template<typename T>
inline const T& Epm::Group::GetItem(const std::string& key) {
	return std::get<T>(items.at(key));
}

#pragma once

#include <variant>
#include <string>
#include <map>
#include <unordered_map>

#include <Vector3.h>
//#include <WorldTransform.h>

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

	private:
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
		void SetGroup(const std::string& key);
		/// <summary>
		/// Groupのゲット
		/// </summary>
		/// <param name="key"></param>
		/// <returns></returns>
		Group& GetGroup(const std::string& key);

		/// <summary>
		/// デバッグ
		/// </summary>
		void ImGuiDebug();

	private:
		/// <summary>
		/// Groupの集合
		/// </summary>
		std::unordered_map<std::string, Group> groups;
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

private:

	/// ===================================================
	/// member objects : private
	/// ===================================================

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

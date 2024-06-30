#pragma once

#include <variant>
#include <string>
#include <map>

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

	/// <summary>
	/// 値一個当たり変数
	/// </summary>
	using Item = std::variant<int32_t, float, Vector3>;

	/// <summary>
	/// Itemの集合
	/// </summary>
	struct Group final {
		std::map<std::string, Item> items;
		/// <summary>
		/// 値のsetter
		/// </summary>
		/// <param name="key">: itemsへのkey</param>
		/// <param name="value">: itemsへセットする値</param>
		template<typename T>
		void SetValue(const std::string& key, const T& value);
		/// <summary>
		/// 値のGetter
		/// </summary>
		/// <param name="key"></param>
		/// <returns></returns>
		template<typename T>
		const T& GetItem(const std::string& key);
	};



	/// <summary>
	/// インスタンス確保
	/// </summary>
	/// <returns></returns>
	static ExternalParamManager* GetInstance();

	/// <summary>
	/// グループの生成
	/// </summary>
	/// <param name="key"></param>
	Group& CreateGroup(const std::string& key);

private:

	///- データの集まり
	std::map<std::string, Group> datas_;


private:
	ExternalParamManager& operator=(const ExternalParamManager&) = delete;
	ExternalParamManager(const ExternalParamManager&) = delete;
	ExternalParamManager(ExternalParamManager&&) = delete;
};



/// ---------------------------------------------------
/// 別名を設定
/// ---------------------------------------------------
using Epm = ExternalParamManager;



/// ===================================================
/// Epm::Group::値のsetter
/// ===================================================
template<typename T>
inline void Epm::Group::SetValue(const std::string& key, const T& value) {
	items[key] = value;
}



/// ===================================================
/// Epm::Group::値のgetter
/// ===================================================
template<typename T>
inline const T& Epm::Group::GetItem(const std::string& key) {
	return std::get<T>(items.at(key));
}

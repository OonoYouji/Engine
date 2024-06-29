#pragma once

#include <string>

/// <summary>
/// コライダーの基底クラス
/// </summary>
class BaseCollider {
public:

	virtual ~BaseCollider() = default;

	/// <summary>
	/// nameのGetter
	/// </summary>
	/// <param name="name"></param>
	void SetName(const std::string& name);

	/// <summary>
	/// nameのSetter
	/// </summary>
	/// <returns></returns>
	const std::string& GetName() const;

protected:

	std::string name_;

};
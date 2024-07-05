#pragma once

#include <string>
#include <unordered_map>

class GameObject;


/// ===================================================
/// GameObjectを継承したクラスを文字列からインスタンス化するクラス
/// ===================================================
class GameObjectFactory final {
	GameObjectFactory() = default;
	~GameObjectFactory() = default;
public:
	using CreateFunc = GameObject * (*)();


	/// <summary>
	/// インスタンス確保
	/// </summary>
	/// <returns>インスタンスへのポインタ</returns>
	static GameObjectFactory* GetInstance();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 文字列からインスタンスを生成
	/// </summary>
	/// <param name="className">: GameObjectを継承したクラスの名前</param>
	/// <returns>生成したクラスへのポインタ</returns>
	GameObject* CreateGameObject(const std::string& className);

	void RegistryClass(const std::string& key, CreateFunc func);

private:

	std::unordered_map<std::string, CreateFunc> registry_;

};


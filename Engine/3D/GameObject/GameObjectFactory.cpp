#include "GameObjectFactory.h"

#pragma region Include
#include <GameObject.h>
#include <Camera.h>
#include <DebugCamera.h>
#include <Player.h>
#include <Object3d.h>
#include <Object2d.h>
#pragma endregion

#pragma region CreateFunc
namespace {
	GameObject* CreateCamera() { return new Camera(); }
	GameObject* CrateDebugCamera() { return new DebugCamera(); }
	GameObject* CreatePlayer() { return new Player(); }
	GameObject* CreateObject3d() { return new Object3d(); }
	GameObject* CreateObject2d() { return new Object2d(); }
}
#pragma endregion

/// ===================================================
/// this インスタンス確保
/// ===================================================
GameObjectFactory* GameObjectFactory::GetInstance() {
	static GameObjectFactory  instance;
	return &instance;
}



/// ===================================================
/// 初期化
/// ===================================================
void GameObjectFactory::Initialize() {
	RegistryClass("Player", CreatePlayer);
	RegistryClass("Camera", CreateCamera);
	RegistryClass("DebugCamera", CrateDebugCamera);
	RegistryClass("Object3d", CreateObject3d);
	RegistryClass("Object2d", CreateObject2d);
}


/// ===================================================
/// GameObject インスタンスの生成
/// ===================================================
GameObject* GameObjectFactory::CreateGameObject(const std::string& className) {
	auto it = registry_.find(className);
	if(it != registry_.end()) {
		return (it->second)();
	}
	return nullptr;
}


/// ===================================================
/// 生成関数の登録関数
/// ===================================================
void GameObjectFactory::RegistryClass(const std::string& key, CreateFunc func) {
	registry_[key] = func;
}

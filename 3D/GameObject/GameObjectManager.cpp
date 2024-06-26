#include <GameObjectManager.h>

#include <iostream>
#include <filesystem>

#include <ImGuiManager.h>

#include <Object3d.h>


GameObjectManager* GameObjectManager::GetInstance() {
	static GameObjectManager instance;
	return &instance;
}

void GameObjectManager::Finalize() {
	ClearList();
}

void GameObjectManager::Update() {
	ImGuiDebug();

	for(auto& gameObject : gameObjects_) {
		if(gameObject->isActive_) {
			gameObject->Update();
		}
	}

}

void GameObjectManager::Draw() {

	for(auto& gameObject : gameObjects_) {
		if(gameObject->isActive_) {
			gameObject->Draw();
		}
	}

}

void GameObjectManager::AddGameObject(GameObject* gameObject) {
	///- 引数のポインタをuniqe_ptrにする
	std::unique_ptr<GameObject> other;
	other.reset(gameObject);

	///- objectの配列に所有権を渡す
	gameObjects_.push_back(std::move(other));
}

void GameObjectManager::ClearList() {
	gameObjects_.clear();
}

void GameObjectManager::ImGuiDebug() {
#ifdef _DEBUG


	ImGui::Begin("Hierarchy", nullptr, ImGuiWindowFlags_MenuBar);

	if(ImGui::BeginMenuBar()) {

		ImGuiMenu();

		ImGui::EndMenuBar();
	}


	/// ------------------------------------------------
	/// GameObject SelecTable
	/// ------------------------------------------------
	for(auto& gameObject : gameObjects_) {
		if(ImGui::Selectable(gameObject->GetTag().c_str(), selectObject_ == gameObject.get())) {
			selectObject_ = gameObject.get();
		}
	}
	ImGui::End();



	ImGui::Begin("Inspector");

	if(selectObject_) {
		ImGui::SetNextItemOpen(true, ImGuiCond_Always);
		if(ImGui::TreeNodeEx(selectObject_->GetTag().c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {

			ImGui::Checkbox("IsActive", &selectObject_->isActive_);

			selectObject_->ImGuiDebug();
			ImGui::TreePop();
		}
	}

	ImGui::End();


#endif // _DEBUG
}

void GameObjectManager::ImGuiMenu() {

	///- Menuが開かなければreturn
	if(!ImGui::BeginMenu("Menu")) { return; }

	if(ImGui::BeginMenu("3D Object		")) {

		if(ImGui::BeginMenu("Create		")) {

			///- directotyPath内のファイルを探索する
			for(const auto& entry : std::filesystem::directory_iterator("./Resources/Objects")) {
				if(entry.is_directory()) {
					std::string fileName = entry.path().string().substr(std::string("./Resources/Objects\\").length());

					///- Objectの追加
					if(ImGui::MenuItem(fileName.c_str())) {
						AddObject3d(fileName);
					}

				}
			}


			ImGui::EndMenu();
		}

		ImGui::EndMenu();
	}

	ImGui::EndMenu();

}

void GameObjectManager::AddObject3d(const std::string& modelKey) {
	///- 新しいObjectの生成
	Object3d* newGameObject = new Object3d();
	newGameObject->Initialize(modelKey);
}

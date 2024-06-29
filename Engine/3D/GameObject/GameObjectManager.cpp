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
	std::unique_ptr<GameObject> other(gameObject);

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

			///- 選択されている要素のデバッグ表示
			selectObject_->ImGuiDebug();

			ImGui::Separator();
			ImGui::NewLine();

			///- 親の設定を行う
			ImGuiParentSetting();
			
			ImGui::Separator();
			ImGui::NewLine();

			///- 選択されている要素を消す
			if(ImGui::Button("Delete")) {

				///- 要素探索
				auto it = std::find_if(gameObjects_.begin(), gameObjects_.end(),
									   [this](const std::unique_ptr<GameObject>& obj) {
					return obj.get() == selectObject_;
				});

				///- 見つかれば要素を消す
				if(it != gameObjects_.end()) {
					gameObjects_.erase(it);
					selectObject_ = nullptr;
				}


			}

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
			for(const auto& entry : std::filesystem::directory_iterator("./Engine/Resources/Objects/")) {
				if(entry.is_directory()) {
					std::string fileName = entry.path().string().substr(std::string("./Engine/Resources/Objects/").length());

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

void GameObjectManager::ImGuiParentSetting() {
	static int currentNumber = 0;
	std::vector<const char*> objectNames;
	objectNames.push_back("null");
	for(const auto& object : gameObjects_) {
		objectNames.push_back(object->GetTag().c_str());
	}

	ImGui::Combo("Parent", &currentNumber, objectNames.data(), static_cast<int>(objectNames.size()));

	if(ImGui::Button("Apply")) {
		for(const auto& object : gameObjects_) {
			if(!currentNumber){
				selectObject_->SetParent(nullptr);
				break;
			}
			if(object->GetTag() == objectNames[currentNumber]) {
				selectObject_->SetParent(object.get());
				break;
			}
		}
	}

}

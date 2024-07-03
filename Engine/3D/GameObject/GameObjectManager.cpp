#include <GameObjectManager.h>

#pragma region Include
#include <iostream>
#include <filesystem>

#include <ImGuiManager.h>

#include <Object3d.h>
#pragma endregion



/// ===================================================
/// インスタンス確保
/// ===================================================
GameObjectManager* GameObjectManager::GetInstance() {
	static GameObjectManager instance;
	return &instance;
}



/// ===================================================
/// 終了処理
/// ===================================================
void GameObjectManager::Finalize() {
	ClearList();
}



/// ===================================================
/// 更新処理
/// ===================================================
void GameObjectManager::Update() {
	ImGuiDebug();

	for(auto& gameObject : gameObjects_) {
		if(gameObject->isActive_) {
			gameObject->Update();
		}
	}

}



/// ===================================================
/// GameObejctを描画する
/// ===================================================
void GameObjectManager::Draw() {

	for(auto& gameObject : gameObjects_) {
		if(gameObject->isActive_) {
			gameObject->Draw();
		}
	}

}



/// ===================================================
/// GameObejctクラスの追加
/// ===================================================
void GameObjectManager::AddGameObject(GameObject* gameObject) {
	///- 引数のポインタをuniqe_ptrにする
	std::unique_ptr<GameObject> other(gameObject);

	///- objectの配列に所有権を渡す
	gameObjects_.push_back(std::move(other));
}



/// ===================================================
/// リストのクリア
/// ===================================================
void GameObjectManager::ClearList() {
	gameObjects_.clear();
}



/// ===================================================
/// ImGuiでデバッグ表示
/// ===================================================
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

		if(gameObject->GetParent()) { continue; }
		if(ImGui::Selectable(gameObject->GetName().c_str(), selectObject_ == gameObject.get())) {
			selectObject_ = gameObject.get();
		}

		ImGuiSelectChilds(gameObject->GetChilds());

	}
	ImGui::End();



	ImGui::Begin("Inspector");

	if(selectObject_) {

		ImGui::SetNextItemOpen(true, ImGuiCond_Always);
		if(ImGui::TreeNodeEx(selectObject_->GetName().c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {

			ImGui::Checkbox("IsActive", &selectObject_->isActive_);

			///- 選択されている要素のデバッグ表示
			selectObject_->ImGuiDebug();

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

					GameObject* parent = (*it)->GetParent();
					std::list<GameObject*> childs = (*it)->GetChilds();

					///- 親がいたら
					if(parent) {
						parent->SubChild((*it).get());
					}

					///- 子供がいたら
					if(!childs.empty()) {

						///- かつ、親がいたら
						if(parent) {

							for(auto& child : childs) {
								child->SetParent(parent);
								parent->AddChild(child);
							}

						} else {
							
							for(auto& child : childs) {
								child->SetParent(nullptr);
							}
						}

					}

					gameObjects_.erase(it);
					selectObject_ = nullptr;
				}


			}

			if(ImGui::Button("Save")) {
				//Epm::GetInstance()->SaveFile(selectObject_->GetName());
			}


			ImGui::TreePop();
		}
	}

	ImGui::End();


#endif // _DEBUG
}



/// ===================================================
/// ImGuiのGameObjectの子供をselectableで設定
/// ===================================================
void GameObjectManager::ImGuiSelectChilds(const std::list<GameObject*>& childs) {
	ImGui::Indent();
	for(auto& child : childs) {
		if(ImGui::Selectable(child->GetName().c_str(), selectObject_ == child)) {
			selectObject_ = child;
		}
		ImGuiSelectChilds(child->GetChilds());
	}
	ImGui::Unindent();
}



/// ===================================================
/// ImGuiで新しいオブジェクトの追加をする
/// ===================================================
void GameObjectManager::ImGuiMenu() {

	///- Menuが開かなければreturn
	if(!ImGui::BeginMenu("Menu")) { return; }

	if(ImGui::BeginMenu("3D Object		")) {

		if(ImGui::BeginMenu("Create		")) {

			///- directotyPath内のファイルを探索する
			for(const auto& entry : std::filesystem::directory_iterator("./Resources/Objects/")) {
				if(entry.is_directory()) {
					std::string fileName = entry.path().string().substr(std::string("./Resources/Objects/").length());

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



/// ===================================================
/// Objcet3dクラスを追加する
/// ===================================================
void GameObjectManager::AddObject3d(const std::string& modelKey) {
	///- 新しいObjectの生成
	Object3d* newGameObject = new Object3d();
	newGameObject->Initialize(modelKey);
}



/// ===================================================
/// ImGuiで親子関係の設定をする
/// ===================================================
void GameObjectManager::ImGuiParentSetting() {
	static int currentNumber = 0;
	std::vector<const char*> objectNames;
	objectNames.push_back("null");
	for(const auto& object : gameObjects_) {
		if(object.get() != selectObject_) {
			objectNames.push_back(object->GetName().c_str());
		}
	}

	ImGui::Combo("Parent", &currentNumber, objectNames.data(), static_cast<int>(objectNames.size()));

	if(ImGui::Button("Apply")) {
		for(const auto& object : gameObjects_) {

			///- 親を取り消す
			if(!currentNumber && selectObject_->GetParent()) {
				GameObject* parent = selectObject_->GetParent();
				parent->SubChild(selectObject_);
				selectObject_->SetParent(nullptr);
				break;
			}

			///- 親を設定する
			if(object->GetName() == objectNames[currentNumber]) {
				///- すでに親がいた場合
				if(selectObject_->GetParent()) {
					GameObject* parent = selectObject_->GetParent();
					parent->SubChild(selectObject_);
				}

				selectObject_->SetParent(object.get());
				object->AddChild(selectObject_);
				break;
			}
		}
	}

}

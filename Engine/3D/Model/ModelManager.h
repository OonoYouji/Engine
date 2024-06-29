#pragma once

#include <string>
#include <map>
#include <unordered_map>
#include <memory>

class Model;

/// <summary>
/// Modelの管理クラス
/// </summary>
class ModelManager {
public:

	/// <summary>
	/// インスタンス確保
	/// </summary>
	/// <returns></returns>
	static ModelManager* GetInstance();

	void Finalize();

	/// <summary>
	/// 描画前処理
	/// </summary>
	void PreDraw();

	/// <summary>
	/// 
	/// </summary>
	void PostDraw();

	void Update();


	Model* GetModelPtr(const std::string& key);

private:

	Model* Create(const std::string& fileName);
	void ImGuiDebug();
	void CreateModel(const std::string& directoryPath, const std::string& fileName, const std::string& key);

	const std::string directoryPath_ = "./Engine/Resources/Objects/";
	std::map<std::string, std::unique_ptr<Model>> models_;
	std::unordered_map<int, std::string> pairs_;

	ModelManager() = default;
	~ModelManager() = default;
	ModelManager& operator=(const ModelManager&) = delete;
	ModelManager(const ModelManager&) = delete;
	ModelManager(ModelManager&&) = delete;
};
